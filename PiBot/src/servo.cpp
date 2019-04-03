/*
 * servo.cpp
 *
 *  Created on: 29 mar 2019
 *      Author: Marcin
 */

#include "servo.h"
#include "program.h"

/*
 * creates the servo based on PCA9585 chip
 * servoNo = number of the servo 0-3
 */
ServoPCA::ServoPCA(SerialBusId busId, SerialPriority priority, I2cDeviceAddress address, uint8_t servoNo)
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
void ServoPCA::setValue(float value)
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
    writeDataRequest(PCA9685Registers::LED0_ON_L + 4 * PCA9685Servo[servoNo], dataToWrite);
}

/*
 * creates servo based on PWM GPIO pin (piGpio library)
 */
ServoGPIO::ServoGPIO(GpioPin gpioNumber)
    : pin(gpioNumber)
{
    GPIO(pin, PI_OUTPUT);
}

ServoGPIO::~ServoGPIO()
{
    gpioServo(pin, 0);
}

/*
 * sets the value of a servo
 * value in the range of 0.0..+1.0
 */
void ServoGPIO::setValue(float value)
{
    if(value < 0.0f)
    {
        value = 0.0f;
    }
    else if(value > 1.0f)
    {
        value = 1.0f;
    }

    gpioServo(pin, static_cast<unsigned>(posMin + value * (posMax - posMin)));
}
