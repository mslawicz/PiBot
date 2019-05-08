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

class AHRS
{
public:
    AHRS();
    void process(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, float dt);
    void process(float gx, float gy, float gz, float ax, float ay, float az, float dt);
    float getQ(uint8_t index) const { return q[index]; }
private:
    float fastInvSqrt(float x);     // fast inverse square-root
    float beta;                     // algorithm gain
    float q[4];                     // quaternion of sensor frame relative to auxiliary frame
};

#endif /* SRC_IMU_H_ */
