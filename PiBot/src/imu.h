/*
 * imu.h
 *
 *  Created on: 17 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_IMU_H_
#define SRC_IMU_H_

#include "gpio.h"
#include "i2c.h"


class Gyroscope : public I2cDevice
{
public:
    Gyroscope(I2cBusId busId, I2cDeviceAddress address, I2cPriority priority);
    ~Gyroscope();
};

#endif /* SRC_IMU_H_ */
