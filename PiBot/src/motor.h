/*
 * motor.h
 *
 *  Created on: 3 lut 2019
 *      Author: Marcin
 */

#ifndef SRC_MOTOR_H_
#define SRC_MOTOR_H_


#include "i2c.h"
#include "PCA9685.h"
#include <mutex>

#define CONST_1 (uint8_t)1
#define CONST_0 (uint8_t)0

/*
 * class of a PCA9685 control device
 * its purpose is an one-time configuration of the device
 */
class PCA9685 : public I2cDevice
{
public:
    static std::once_flag onceFlag;
private:
    PCA9685(I2cBusId busId, I2cDeviceAddress address, I2cPriority priority);
    void config(void);
    friend class Motor;
};

/*
 * class of a DC motor
 */
class Motor : public I2cDevice
{
public:
    Motor(I2cBusId busId, I2cDeviceAddress address, I2cPriority priority, uint8_t motorNo);
    ~Motor();
    void test(void);    //XXX test
    void setSpeed(float speed, bool motorOff = false);
private:
    std::vector<uint8_t> setChannelData(uint8_t bit);
    std::vector<uint8_t> setChannelData(float value);
    const uint8_t MaxMotors = 4;
    const std::vector<uint8_t> TB6612In1 {10, 11, 4, 5};
    const std::vector<uint8_t> TB6612In2 {9, 12, 3, 6};
    const std::vector<uint8_t> TB6612PWM {8, 13, 2, 7};
    float lastSpeed;
    uint8_t motorNo;
};

#endif /* SRC_MOTOR_H_ */
