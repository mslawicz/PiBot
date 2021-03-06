/*
 * ili9341.cpp
 *
 *  Created on: 15 lut 2019
 *      Author: Marcin
 */

#include "ili9341.h"
#include "gpio.h"
#include <thread>
#include <chrono>
#include <stdlib.h> // test

Ili9341::Ili9341(SerialBusId spiBusId, SerialPriority priority)
    : SpiDevice(spiBusId, priority, 3900000, GpioPin::ILI9341_CD)
{
    maxX = maxY = 0;
    activeLeftX = activeTopY = activeRightX = activeBottomY = 0;
}


Ili9341::~Ili9341()
{
    // TODO Auto-generated destructor stub
}

void Ili9341::initialize(void)
{
    writeDataRequest(Ili9341Registers::SWRESET);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    writeDataRequest(Ili9341Registers::DISPOFF);
    writeDataRequest(Ili9341Registers::MADCTL, std::vector<uint8_t>{0x98});
    writeDataRequest(Ili9341Registers::PIXSET, std::vector<uint8_t>{0x55});
    writeDataRequest(Ili9341Registers::IFCTL, std::vector<uint8_t>{0x00, 0x00, 0x00});
    writeDataRequest(Ili9341Registers::PWCTR1, std::vector<uint8_t>{0x23}); //VRH[5:0]
    writeDataRequest(Ili9341Registers::PWCTR2, std::vector<uint8_t>{0x10}); //SAP[2:0];BT[3:0]
    writeDataRequest(Ili9341Registers::VMCTR1, std::vector<uint8_t>{0x3e, 0x28});
    writeDataRequest(Ili9341Registers::VMCTR2, std::vector<uint8_t>{0x86}); //--
    writeDataRequest(Ili9341Registers::FRMCTR1, std::vector<uint8_t>{0x00, 0x18});
    writeDataRequest(0xF2, std::vector<uint8_t>{0x00}); // 3Gamma Function Disable
    writeDataRequest(Ili9341Registers::GAMMASET, std::vector<uint8_t>{0x01});
    writeDataRequest(Ili9341Registers::GMCTRP1, std::vector<uint8_t>{0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00});
    writeDataRequest(Ili9341Registers::GMCTRN1, std::vector<uint8_t>{0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F});
    writeDataRequest(Ili9341Registers::SLPOUT); //Exit Sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    writeDataRequest(Ili9341Registers::DISPON); //Display on
    maxX = 240;
    maxY = 320;
}

/*
 * sets active square area for display write operations
 */
void Ili9341::setActiveArea(uint16_t leftX, uint16_t topY, uint16_t rightX, uint16_t bottomY)
{
    activeLeftX = leftX;
    activeRightX = rightX;
    writeDataRequest(Ili9341Registers::CASET, std::vector<uint16_t>{activeLeftX, activeRightX});
    activeTopY = topY;
    activeBottomY = bottomY;
    writeDataRequest(Ili9341Registers::PASET, std::vector<uint16_t>{activeTopY, activeBottomY});
}

/*
 * fills active area with solid color
 */
void Ili9341::fillActiveArea(uint16_t color)
{
    std::vector<uint16_t> rawOfPixels(activeRightX - activeLeftX + 1, color);
    writeDataRequest(Ili9341Registers::RAMWR, rawOfPixels);
    for(uint16_t raw = activeTopY; raw < activeBottomY; raw++)
    {
        writeDataRequest(Ili9341Registers::WRCONT, rawOfPixels);
    }
}




