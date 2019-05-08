/*
 * imu.cpp
 *
 *  Created on: 17 sty 2019
 *      Author: Marcin
 */

#include "imu.h"
#include "LSM9DS1.h"
#include "program.h"
#include "logger.h"
#include <math.h>

Gyroscope::Gyroscope(SerialBusId busId, SerialPriority priority, I2cDeviceAddress address)
    : I2cDevice(busId, priority, address)
{
    // set gyroscope interrupt pin as input
    GPIO gyroscopeInterruptPin(GpioPin::GYRO_INT, PI_INPUT, PI_PUD_DOWN);

    // Gyroscope data ready on INT 1_A/G pin
    writeDataRequest(ImuRegisters::INT1_CTRL, std::vector<uint8_t>{0x02});
    // gyroscope output data rate 119 Hz, 245 dps, LPF=31 Hz
    // LPF2 output for interrupts and DataReg
    // low-power disabled, HPF 0.02 Hz
    // swap X/Y axis
    writeDataRequest(ImuRegisters::CTRL_REG1_G, std::vector<uint8_t>{0x63, 0x00, 0x00, 0x02});
}

Accelerometer::Accelerometer(SerialBusId busId, SerialPriority priority, I2cDeviceAddress address)
    : I2cDevice(busId, priority, address)
{
    // 238 Hz, +-2g, BW=auto (105 Hz)
    writeDataRequest(ImuRegisters::CTRL_REG6_XL, std::vector<uint8_t>{0x82});
}

AHRS::AHRS()
{
    beta = 5.0f;
    q[0] = 1.0f;
    q[1] = q[2] = q[3] = 0.0f;
}

void AHRS::process(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, float dt)
{
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float hx, hy;
    float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

    // Use IMU (6-DOF) algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
    if ((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f))
    {
        process(gx, gy, gz, ax, ay, az, dt);
        return;
    }

    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-q[1] * gx - q[2] * gy - q[3] * gz);
    qDot2 = 0.5f * (q[0] * gx + q[2] * gz - q[3] * gy);
    qDot3 = 0.5f * (q[0] * gy - q[1] * gz + q[3] * gx);
    qDot4 = 0.5f * (q[0] * gz + q[1] * gy - q[2] * gx);

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
    {

        // Normalise accelerometer measurement
        recipNorm = fastInvSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Normalise magnetometer measurement
        recipNorm = fastInvSqrt(mx * mx + my * my + mz * mz);
        mx *= recipNorm;
        my *= recipNorm;
        mz *= recipNorm;

        // Auxiliary variables to avoid repeated arithmetic
        _2q0mx = 2.0f * q[0] * mx;
        _2q0my = 2.0f * q[0] * my;
        _2q0mz = 2.0f * q[0] * mz;
        _2q1mx = 2.0f * q[1] * mx;
        _2q0 = 2.0f * q[0];
        _2q1 = 2.0f * q[1];
        _2q2 = 2.0f * q[2];
        _2q3 = 2.0f * q[3];
        _2q0q2 = 2.0f * q[0] * q[2];
        _2q2q3 = 2.0f * q[2] * q[3];
        q0q0 = q[0] * q[0];
        q0q1 = q[0] * q[1];
        q0q2 = q[0] * q[2];
        q0q3 = q[0] * q[3];
        q1q1 = q[1] * q[1];
        q1q2 = q[1] * q[2];
        q1q3 = q[1] * q[3];
        q2q2 = q[2] * q[2];
        q2q3 = q[2] * q[3];
        q3q3 = q[3] * q[3];

        // Reference direction of Earth's magnetic field
        hx = mx * q0q0 - _2q0my * q[3] + _2q0mz * q[2] + mx * q1q1 + _2q1 * my * q[2] + _2q1 * mz * q[3] - mx * q2q2 - mx * q3q3;
        hy = _2q0mx * q[3] + my * q0q0 - _2q0mz * q[1] + _2q1mx * q[2] - my * q1q1 + my * q2q2 + _2q2 * mz * q[3] - my * q3q3;
        _2bx = sqrtf(static_cast<float>(hx * hx) + static_cast<float>(hy * hy));
        _2bz = -_2q0mx * q[2] + _2q0my * q[1] + mz * q0q0 + _2q1mx * q[3] - mz * q1q1 + _2q2 * my * q[3] - mz * q2q2 + mz * q3q3;
        _4bx = 2.0f * _2bx;
        _4bz = 2.0f * _2bz;

        // Gradient decent algorithm corrective step
        s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * q[2] * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q[3] + _2bz * q[1]) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q[2] * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q[1] * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * q[3] * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q[2] + _2bz * q[0]) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q[3] - _4bz * q[1]) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q[2] * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * q[2] - _2bz * q[0]) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q[1] + _2bz * q[3]) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q[0] - _4bz * q[2]) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * q[3] + _2bz * q[1]) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q[0] + _2bz * q[2]) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q[1] * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        recipNorm = fastInvSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        // Apply feedback step
        qDot1 -= beta * s0;
        qDot2 -= beta * s1;
        qDot3 -= beta * s2;
        qDot4 -= beta * s3;
    }

    // Integrate rate of change of quaternion to yield quaternion
    q[0] += qDot1 * dt;
    q[1] += qDot2 * dt;
    q[2] += qDot3 * dt;
    q[3] += qDot4 * dt;

    // Normalise quaternion
    recipNorm = fastInvSqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
    q[0] *= recipNorm;
    q[1] *= recipNorm;
    q[2] *= recipNorm;
    q[3] *= recipNorm;
}

void AHRS::process(float gx, float gy, float gz, float ax, float ay, float az, float dt)
{
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-q[1] * gx - q[2] * gy - q[3] * gz);
    qDot2 = 0.5f * (q[0] * gx + q[2] * gz - q[3] * gy);
    qDot3 = 0.5f * (q[0] * gy - q[1] * gz + q[3] * gx);
    qDot4 = 0.5f * (q[0] * gz + q[1] * gy - q[2] * gx);

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
    {
        // Normalise accelerometer measurement
        recipNorm = fastInvSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Auxiliary variables to avoid repeated arithmetic
        _2q0 = 2.0f * q[0];
        _2q1 = 2.0f * q[1];
        _2q2 = 2.0f * q[2];
        _2q3 = 2.0f * q[3];
        _4q0 = 4.0f * q[0];
        _4q1 = 4.0f * q[1];
        _4q2 = 4.0f * q[2];
        _8q1 = 8.0f * q[1];
        _8q2 = 8.0f * q[2];
        q0q0 = q[0] * q[0];
        q1q1 = q[1] * q[1];
        q2q2 = q[2] * q[2];
        q3q3 = q[3] * q[3];

        // Gradient decent algorithm corrective step
        s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
        s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q[1] - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
        s2 = 4.0f * q0q0 * q[2] + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
        s3 = 4.0f * q1q1 * q[3] - _2q1 * ax + 4.0f * q2q2 * q[3] - _2q2 * ay;
        recipNorm = fastInvSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        // Apply feedback step
        qDot1 -= beta * s0;
        qDot2 -= beta * s1;
        qDot3 -= beta * s2;
        qDot4 -= beta * s3;
    }

    // Integrate rate of change of quaternion to yield quaternion
    q[0] += qDot1 * dt;
    q[1] += qDot2 * dt;
    q[2] += qDot3 * dt;
    q[3] += qDot4 * dt;

    // Normalise quaternion
    recipNorm = fastInvSqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
    q[0] *= recipNorm;
    q[1] *= recipNorm;
    q[2] *= recipNorm;
    q[3] *= recipNorm;
}

/*
    fast inverse square-root
    see: http://en.wikipedia.org/wiki/Fast_inverse_square_root
*/
float AHRS::fastInvSqrt(float x)
{
    float y = x;
    long i = *reinterpret_cast<long*>(&y);
    i = 0x5f3759df - (i >> 1);
    y = *reinterpret_cast<float*>(&i);
    y *= 1.5f - 0.5f * x * y * y;
    return y;
}
