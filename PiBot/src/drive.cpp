/*
 * Drive.cpp
 *
 *  Created on: 16 sty 2019
 *      Author: Marcin
 */

#include "drive.h"
#include "program.h"

Drive::Drive()
{
    // left motor
    pMotors.push_back(new Motor(SerialBusId::I2C1, SerialPriority::MOTOR_PR, I2cDeviceAddress::MOTOR_ADDR, 0));
    // right motor
    pMotors.push_back(new Motor(SerialBusId::I2C1, SerialPriority::MOTOR_PR, I2cDeviceAddress::MOTOR_ADDR, 2));
}

Drive::~Drive()
{
    for(auto pMotor : pMotors)
    {
        delete pMotor;
        pMotor = nullptr;
    }
}

/*
 * starts the control of motors
 */
void Drive::start(void)
{

}

/*
 * stops the control of motors
 */
void Drive::stop(void)
{
    pMotors[0]->setSpeed(0, true);
    pMotors[1]->setSpeed(0, true);
}

/*
 * set motor speed
 */
void Drive::setMotorSpeed(MotorName motorName, float speed)
{
    motorSpeed[motorName] = speed;
    pMotors[motorName]->setSpeed(motorSpeed[motorName]);
}
