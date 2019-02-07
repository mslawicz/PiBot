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
PCA9685::PCA9685(I2cBusId busId, I2cDeviceAddress address, I2cPriority priority)
    : I2cDevice(busId, address, priority)
{
    Logger::getInstance().logEvent(INFO, "PCA9685 setup: bus=", busId, ", address=0x", std::hex, address);
    // register auto increment enable
    // change on STOP, SLEEP on, output totem pole
    writeData(PCA9685Registers::MODE1, std::vector<uint8_t>{0x30, 0x04});
    // prescale
    writeData(PCA9685Registers::PRE_SCALE, std::vector<uint8_t>{0x0A});
    // change on STOP, SLEEP off, output totem pole
    writeData(PCA9685Registers::MODE1, std::vector<uint8_t>{0x20, 0x04});
}
