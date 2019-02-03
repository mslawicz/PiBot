/*
 * motor.cpp
 *
 *  Created on: 3 lut 2019
 *      Author: Marcin
 */

#include "motor.h"

Motor::Motor(I2cBusId busId, I2cDeviceAddress address, I2cPriority priority)
    : I2cDevice(busId, address, priority)
{
    // TODO Auto-generated constructor stub

}

Motor::~Motor()
{
    // TODO Auto-generated destructor stub
}

