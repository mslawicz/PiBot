/*
 * ili9341.h
 *
 *  Created on: 15 lut 2019
 *      Author: Marcin
 */

#ifndef SRC_ILI9341_H_
#define SRC_ILI9341_H_

#include "spi.h"

enum Ili9341Registers
{
    NOP = 0x00,
    SWRESET = 0x01,
    RDDID = 0x04,
    RDDST = 0x09,
    SLPIN = 0x10,
    SLPOUT = 0x11,
    PTLON = 0x12,
    NORON = 0x13,
    RDMODE = 0x0A,
    RDMADCTL = 0x0B,
    RDPIXFMT = 0x0C,
    RDIMGFMT = 0x0A,
    RDSELFDIAG = 0x0F,
    INVOFF = 0x20,
    INVON = 0x21,
    GAMMASET = 0x26,
    DISPOFF = 0x28,
    DISPON = 0x29,
    CASET = 0x2A,
    PASET = 0x2B,
    RAMWR = 0x2C,
    RAMRD = 0x2E,
    PTLAR = 0x30,
    MADCTL = 0x36,
    PIXFMT = 0x3A,
    WRDISBV = 0x51,
    FRMCTR1 = 0xB1,
    FRMCTR2 = 0xB2,
    FRMCTR3 = 0xB3,
    INVCTR = 0xB4,
    DFUNCTR = 0xB6,
    PWCTR1 = 0xC0,
    PWCTR2 = 0xC1,
    PWCTR3 = 0xC2,
    PWCTR4 = 0xC3,
    PWCTR5 = 0xC4,
    VMCTR1 = 0xC5,
    VMCTR2 = 0xC7,
    RDID1 = 0xDA,
    RDID2 = 0xDB,
    RDID3 = 0xDC,
    RDID4 = 0xDD,
    GMCTRP1 = 0xE0,
    GMCTRN1 = 0xE1,
    PWCTR6 = 0xFC
};

enum Ili9341Color
{
    BLACK = 0x0000,
    WHITE = 0xFFFF,
    GRAY = 0x7BEF,
    LIGHT_GRAY = 0xC618,
    GREEN = 0x07E0,
    LIME = 0x87E0,
    BLUE = 0x001F,
    RED = 0xF800,
    AQUA = 0x5D1C,
    YELLOW = 0xFFE0,
    MAGENTA = 0xF81F,
    CYAN = 0x07FF,
    DARK_CYAN = 0x03EF,
    ORANGE = 0xFCA0,
    PINK = 0xF97F,
    BROWN = 0x8200,
    VIOLET = 0x9199,
    SILVER = 0xA510,
    GOLD = 0xA508,
    NAVY = 0x000F,
    MAROON = 0x7800,
    PURPLE = 0x780F,
    OLIVE = 0x7BE0
};

class Ili9341 : public SpiDevice
{
public:
    Ili9341(SerialBusId spiBusId, SerialPriority priority);
    ~Ili9341();
protected:
    void initialize(void);
};



#endif /* SRC_ILI9341_H_ */
