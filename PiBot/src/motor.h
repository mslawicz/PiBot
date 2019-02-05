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

class Motor : public I2cDevice
{
public:
    Motor(I2cBusId busId, I2cDeviceAddress address, I2cPriority priority, uint8_t motorNo);
    ~Motor();
    void test(void);    //XXX test
    void setSpeed(float speed);
private:
    const uint8_t MaxMotors = 4;
    const std::vector<uint8_t> TB6612In1 {10, 11, 4, 5};
    const std::vector<uint8_t> TB6612In2 {9, 12, 3, 6};
    const std::vector<uint8_t> TB6612PWM {8, 13, 2, 7};
    float lastSpeed;
};

#endif /* SRC_MOTOR_H_ */
