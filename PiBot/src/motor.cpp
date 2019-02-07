/*
 * motor.cpp
 *
 *  Created on: 3 lut 2019
 *      Author: Marcin
 */

#include "motor.h"
#include "logger.h"
#include "program.h"
#include <vector>
#include <map>
#include <cmath>

// once-call flag for PCA9685 configuration
std::once_flag PCA9685::onceFlag;

/*
 * constructor of a device for configuration of PCA9685 chip
 */
PCA9685::PCA9685(I2cBusId busId, I2cDeviceAddress address, I2cPriority priority)
    : I2cDevice(busId, address, priority)
{

}

/*
 * creates the motor
 * all drive motors should be created with the same priority
 * motorNo = number of the motor 0-3 (described as M1..M4)
 */
Motor::Motor(I2cBusId busId, I2cDeviceAddress address, I2cPriority priority, uint8_t motorNo)
    : I2cDevice(busId, address, priority)
    , motorNo(motorNo)
{
    if(motorNo >= MaxMotors)
    {
        Program::getInstance().terminate(ExitCode::BAD_MOTOR_NO);
    }
    // create device for PCA9685 chip configuration
    PCA9685 chipPCA9685(busId, address, priority);
    // register auto increment enable
    // change on STOP, SLEEP on, output totem pole
    writeData(PCA9685Registers::MODE1, std::vector<uint8_t>{0x30, 0x04});
    // prescale
    writeData(PCA9685Registers::PRE_SCALE, std::vector<uint8_t>{0x0A});
    // change on STOP, SLEEP off, output totem pole
    writeData(PCA9685Registers::MODE1, std::vector<uint8_t>{0x20, 0x04});
    lastSpeed = 0.0F;
}

Motor::~Motor()
{
    // TODO Auto-generated destructor stub
}

void Motor::test()
{
    // PWM0 always on, PWM1 always off
    writeData(PCA9685Registers::LED0_ON_L, std::vector<uint8_t>{0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10});
    // PWM14 ~50%, PWM15 ~25% delayed ~0.5T
    writeData(PCA9685Registers::LED0_ON_L + 14*4, std::vector<uint8_t>{0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x0C});
}

/*
 * sets the speed of a DC motor
 * speed in the range of -1.0 (max reverse) ..0 (stop) ..+1.0 (max forward)
 */
void Motor::setSpeed(float speed, bool motorOff)
{
    std::map<uint8_t, std::vector<uint8_t>> registers;
    if(motorOff)
    {
        // motor off - high impedance
        speed = 0.0;
        registers[TB6612In1[motorNo]] = setChannelData(CONST_0);
        registers[TB6612In2[motorNo]] = setChannelData(CONST_0);
    }
    else if((speed == 0) && (lastSpeed != 0))
    {
        // stop motor - short brake
        registers[TB6612In1[motorNo]] = setChannelData(CONST_1);
        registers[TB6612In2[motorNo]] = setChannelData(CONST_1);
    }
    else if(((speed > 0) && (lastSpeed > 0)) || ((speed < 0) && (lastSpeed < 0)))
    {
        // speed not zero and the sign is not changed - change only PWM value
        registers[TB6612PWM[motorNo]] = setChannelData(speed);
    }
    else if(speed != 0)
    {
        // speed not zero, but the sign has changed - set all registers
        registers[TB6612In1[motorNo]] = setChannelData(speed > 0 ? CONST_1 : CONST_0);
        registers[TB6612In2[motorNo]] = setChannelData(speed > 0 ? CONST_0 : CONST_1);
        registers[TB6612PWM[motorNo]] = setChannelData(speed);
    }
    std::vector<uint8_t> dataToWrite;
    for(auto reg : registers)
    {
        dataToWrite.insert(dataToWrite.end(), reg.second.begin(), reg.second.end());
    }

    // write data to PWM channels registers
    writeData(PCA9685Registers::LED0_ON_L + 4 * registers.begin()->first, dataToWrite);
    // store the current speed
    lastSpeed = speed;
}

/*
 * set PCA9685 channel data to generate constant 1 or 0
 */
std::vector<uint8_t> Motor::setChannelData(uint8_t bit)
{
    return std::vector<uint8_t>{0x00, (uint8_t)(bit ? 0x10 : 0x00), 0x00, (uint8_t)(bit ? 0x00 : 0x10)};
}

/*
 * set PCA9685 channel data to generate PWM wave
 * the abs(value) should be 0.0 ... 1.0
 */
std::vector<uint8_t> Motor::setChannelData(float value)
{
    value = fabs(value);
    if(value > 1.0)
    {
        value = 1.0;
    }
    uint16_t pwmValue = 0x0FFF * value + 0.5;
    return std::vector<uint8_t>{0x00, 0x00, (uint8_t)(pwmValue & 0xFF), (uint8_t)((pwmValue >> 8) & 0x0F)};
}
