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
    Motor(I2cBusId busId, I2cDeviceAddress address, I2cPriority priority);
    ~Motor();
};

#endif /* SRC_MOTOR_H_ */
