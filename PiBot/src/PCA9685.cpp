/*
 * PCA9685.cpp
 *
 *  Created on: 7 lut 2019
 *      Author: Marcin
 */


#include "PCA9685.h"
#include "logger.h"

/*
 * constructor of a device for configuration of PCA9685 chip
 */
PCA9685::PCA9685(SerialBusId busId, SerialPriority priority, I2cDeviceAddress address)
    : I2cDevice(busId, priority, address)
{
    Logger::getInstance().logEvent(INFO, "PCA9685 setup: bus=", busId, ", address=0x", std::hex, address);
    // register auto increment enable
    // SLEEP on
    writeDataRequest(PCA9685Registers::MODE1, std::vector<uint8_t>{0x30});
    // prescale 1279 Hz
    writeDataRequest(PCA9685Registers::PRE_SCALE, std::vector<uint8_t>{0x04});
    // SLEEP off
    writeDataRequest(PCA9685Registers::MODE1, std::vector<uint8_t>{0x20});
}
