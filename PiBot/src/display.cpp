/*
 * display.cpp
 *
 *  Created on: 14 lut 2019
 *      Author: Marcin
 */

#include "display.h"

Display::Display()
    : Ili9341(SerialBusId::SPI_MAIN, SerialPriority::DISPLAY_PR)
{
    initialize();
}

Display::~Display()
{
    // TODO Auto-generated destructor stub
}

void Display::setBackLight(float level)
{
    if(level < 0.0)
    {
        level = 0.0;
    }

    if(level > 1.0)
    {
        level = 1.0f;
    }

    gpioPWM(GpioPin::BACKLIGHT, static_cast<unsigned>(level * 255.0 + 0.5));
}
