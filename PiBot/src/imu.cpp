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
    GPIO gyroscopeInterruptPin(GpioPin::GYRO_INT, PI_INPUT, PI_PUD_DOWN);

    // Gyroscope data ready on INT 1_A/G pin
    writeData(ImuRegisters::INT1_CTRL, std::vector<uint8_t>{0x02});
    // gyroscope output data rate 238 Hz, 245 dps, LPF=29 Hz
    // LPF2 output for interrupts and DataReg
    // low-power disabled, HPF enabled, HPF 1 Hz
    writeData(ImuRegisters::CTRL_REG1_G, std::vector<uint8_t>{0x81, 0x0F, 0x44});
}

Gyroscope::~Gyroscope()
{

}
