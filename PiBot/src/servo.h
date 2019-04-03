/*
 * servo.h
 *
 *  Created on: 29 mar 2019
 *      Author: Marcin
 */

#ifndef SRC_SERVO_H_
#define SRC_SERVO_H_

#include "i2c.h"
#include "gpio.h"
#include <pigpio.h>
#include <vector>

/*
 * servo based on PCA9685 chip
 */
class ServoPCA : public I2cDevice
{
public:
    ServoPCA(SerialBusId busId, SerialPriority priority, I2cDeviceAddress address, uint8_t servoNo);
    void setValue(float value);
private:
    const uint8_t MaxServos = 4;
    uint8_t servoNo;
    const std::vector<uint8_t> PCA9685Servo {0, 1, 14, 15};
};

/*
 * servo based on piGPIO function
 */
class ServoGPIO
{
public:
    ServoGPIO(GpioPin gpioNumber);
    ~ServoGPIO();
    void setValue(float value);
private:
    GpioPin pin;
    const unsigned posMin = 600;
    const unsigned posMax = 2300;
};

#endif /* SRC_SERVO_H_ */
