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

Gyroscope::Gyroscope(I2cBusId busId, I2cDeviceAddress address, I2cPriority priority)
    : I2cDevice(busId, address, priority)
{
    // set gyroscope interrupt pin as input
    GPIO gyroscopeInterruptPin(GYRO_INT_PIN, PI_INPUT, PI_PUD_DOWN);

    // Gyroscope data ready on INT 1_A/G pin
    std::vector<uint8_t> data = {0x02};
    writeData(ImuRegisters::INT1_CTRL, data);
    // gyroscope output data rate 238 Hz, 245 dps, LPF=29 Hz
    // LPF2 output for interrupts and DataReg
    // low-power disabled, HPF enabled, HPF 1 Hz
    data = {0x81, 0x0F, 0x44};
    writeData(ImuRegisters::CTRL_REG1_G, data);
}

Gyroscope::~Gyroscope()
{

}
