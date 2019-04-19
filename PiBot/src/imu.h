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
    Gyroscope(SerialBusId busId, SerialPriority priority, I2cDeviceAddress address);
    // gyroscope full scale range in radians / second
    const float range = 4.27605666739;
};

class Accelerometer : public I2cDevice
{
public:
    Accelerometer(SerialBusId busId, SerialPriority priority, I2cDeviceAddress address);
    // accelerometer full scale range in g
    const float range = 2.0;
};

#endif /* SRC_IMU_H_ */
