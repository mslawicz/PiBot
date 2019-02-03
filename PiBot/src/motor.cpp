/*
 * motor.cpp
 *
 *  Created on: 3 lut 2019
 *      Author: Marcin
 */

#include "motor.h"
#include <vector>

Motor::Motor(I2cBusId busId, I2cDeviceAddress address, I2cPriority priority)
    : I2cDevice(busId, address, priority)
{
    // register auto increment enable
    writeData(PCA9685Registers::MODE1, std::vector<uint8_t>{0x20});

}

Motor::~Motor()
{
    // TODO Auto-generated destructor stub
}

