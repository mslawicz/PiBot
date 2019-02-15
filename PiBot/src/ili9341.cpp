/*
 * ili9341.cpp
 *
 *  Created on: 15 lut 2019
 *      Author: Marcin
 */

#include "ili9341.h"

Ili9341::Ili9341(SpiChannelId spiChannelId, SpiPriority priority)
    : SpiDevice(spiChannelId, priority, 100000)
{

}


Ili9341::~Ili9341()
{
    // TODO Auto-generated destructor stub
}

