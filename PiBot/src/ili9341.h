/*
 * ili9341.h
 *
 *  Created on: 15 lut 2019
 *      Author: Marcin
 */

#ifndef SRC_ILI9341_H_
#define SRC_ILI9341_H_

#include "spi.h"

class Ili9341 : public SpiDevice
{
public:
    Ili9341(SerialBusId spiBusId, SerialPriority priority);
    ~Ili9341();
private:
};



#endif /* SRC_ILI9341_H_ */
