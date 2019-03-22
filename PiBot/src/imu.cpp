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

Gyroscope::Gyroscope(SerialBusId busId, SerialPriority priority, I2cDeviceAddress address)
    : I2cDevice(busId, priority, address)
{
    // set gyroscope interrupt pin as input
    GPIO gyroscopeInterruptPin(GpioPin::GYRO_INT, PI_INPUT, PI_PUD_DOWN);

    // Gyroscope data ready on INT 1_A/G pin
    writeDataRequest(ImuRegisters::INT1_CTRL, std::vector<uint8_t>{0x02});
    // gyroscope output data rate 238 Hz, 245 dps, LPF=63 Hz
    // LPF2 output for interrupts and DataReg
    // low-power disabled, HPF enabled, HPF 0.1 Hz
    // swap X/Y axis
    writeDataRequest(ImuRegisters::CTRL_REG1_G, std::vector<uint8_t>{0x82, 0x0F, 0x47, 0x02});
}

Gyroscope::~Gyroscope()
{

}
