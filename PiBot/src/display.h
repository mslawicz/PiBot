/*
 * display.h
 *
 *  Created on: 14 lut 2019
 *      Author: Marcin
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

#include "spi.h"
#include "ili9341.h"

class Display : public Ili9341
{
public:
    Display();
    ~Display();
private:
    const SpiChannelId driverChannelId = SpiChannelId::SPI_MAIN;
    const SpiPriority driverPriority = SpiPriority::DISPLAY;
};

#endif /* SRC_DISPLAY_H_ */
