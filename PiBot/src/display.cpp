/*
 * display.cpp
 *
 *  Created on: 14 lut 2019
 *      Author: Marcin
 */

#include "display.h"

Display::Display()
    : Ili9341(driverChannelId, driverPriority)
{


}

Display::~Display()
{
    // TODO Auto-generated destructor stub
}

