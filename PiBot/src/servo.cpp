/*
 * servo.cpp
 *
 *  Created on: 29 mar 2019
 *      Author: Marcin
 */

#include "servo.h"
#include "program.h"

/*
 * creates the servo
 * servoNo = number of the servo 0-3
 */
Servo::Servo(SerialBusId busId, SerialPriority priority, I2cDeviceAddress address, uint8_t servoNo)
    : I2cDevice(busId, priority, address)
    , servoNo(servoNo)
{
    if(servoNo >= MaxServos)
    {
        Program::getInstance().terminate(ExitCode::BAD_SERVO_NO);
    }

}

/*
 * sets the value of a servo
 * value in the range of 0.0..+1.0
 */
void Servo::setValue(float value)
{
    if(value < 0)
    {
        value = 0.0f;
    }
    else if(value > 1.0)
    {
        value = 1.0f;
    }
    uint16_t pwmValue = 0x0FFF * value + 0.5;
    std::vector<uint8_t> dataToWrite {0x00, 0x00, (uint8_t)(pwmValue & 0xFF), (uint8_t)((pwmValue >> 8) & 0x0F)};
    writeDataRequest(PCA9685Registers::LED0_ON_L + 4 * TB6612Servo[servoNo], dataToWrite);
}
