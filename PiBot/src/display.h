/*
 * display.h
 *
 *  Created on: 14 lut 2019
 *      Author: Marcin
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

#include "spi.h"

class Display : public SpiDevice
{
public:
    Display(SpiChannelId spiChannelId, SpiPriority devicePriority);
    ~Display();
};

#endif /* SRC_DISPLAY_H_ */
