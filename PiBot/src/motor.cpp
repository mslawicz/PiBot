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

/*
 * creates the motor
 * all drive motors should be created with the same priority
 * motorNo = number of the motor 0-3 (described as M1..M4)
 */
Motor::Motor(I2cBusId busId, I2cDeviceAddress address, I2cPriority priority, uint8_t motorNo)
    : I2cDevice(busId, address, priority)
{
    if(motorNo >= MaxMotors)
    {
        Program::getInstance().terminate(ExitCode::BAD_MOTOR_NO);
    }
    // register auto increment enable
    // change on ACK, output totem pole
    writeData(PCA9685Registers::MODE1, std::vector<uint8_t>{0x20, 0x0C});
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
void Motor::setSpeed(float speed)
{
    if((speed == 0) && (lastSpeed != 0))
    {
        // stop motor - short brake
    }
    else if(((speed > 0) && (lastSpeed > 0)) || ((speed < 0) && (lastSpeed < 0)))
    {
        // speed not zero and the sign is not changed - change only PWM value
    }
    else if(speed != 0)
    {
        // speed not zero, but the sign has changed - set all registers
    }

    // store the current speed
    lastSpeed = speed;
}
