/*
 * ili9341.cpp
 *
 *  Created on: 15 lut 2019
 *      Author: Marcin
 */

#include "ili9341.h"
#include "gpio.h"

Ili9341::Ili9341(SerialBusId spiBusId, SerialPriority priority)
    : SpiDevice(spiBusId, priority, 100000, GpioPin::ILI9341_CD)
{

}


Ili9341::~Ili9341()
{
    // TODO Auto-generated destructor stub
}

