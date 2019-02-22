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
//    writeDataRequest(0xEF, std::vector<uint8_t>{0x03, 0x80, 0x02});
//    writeDataRequest(0xCF, std::vector<uint8_t>{0x00, 0XC1, 0X30});
//    writeDataRequest(0xED, std::vector<uint8_t>{0x64, 0x03, 0X12, 0X81});
//    writeDataRequest(0xE8, std::vector<uint8_t>{0x85, 0x00, 0x78});
//    writeDataRequest(0xCB, std::vector<uint8_t>{0x39, 0x2C, 0x00, 0x34, 0x02});
//    writeDataRequest(0xF7, std::vector<uint8_t>{0x20});
//    writeDataRequest(0xEA, std::vector<uint8_t>{0x00, 0x00});
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

void Ili9341::test1()
{
    static uint16_t scroll = 0;
    writeDataRequest(Ili9341Registers::VSCRSADD, std::vector<uint16_t>{scroll++});
    if(scroll > 300)
    {
        scroll = 0;
    }
}

void Ili9341::test2()
{

    setActiveArea(0,0,maxX,maxY);
    fillActiveArea(Ili9341Color::BLACK);

    rectangle(0,300,60,20, Ili9341Color::ORANGE);
    rectangle(60,300,60,20, Ili9341Color::MAGENTA);
    rectangle(120,300,60,20, Ili9341Color::YELLOW);
    rectangle(180,300,60,20, Ili9341Color::CYAN);

    writeDataRequest(Ili9341Registers::VSCRDEF, std::vector<uint16_t>{0, 300, 20});

    rectangle(10,10,100,50, Ili9341Color::RED);
    rectangle(40,60,100,50, Ili9341Color::GREEN);
    rectangle(20,110,150,50, Ili9341Color::BLUE);

}

void Ili9341::test3()
{
    rectangle(220,30,20,20, rand() & 0xFFFF);
}

/*
 * display solid color rectangle
 */
void Ili9341::rectangle(uint16_t positionX, uint16_t positionY, uint16_t width, uint16_t height, uint16_t color)
{
    setActiveArea(positionX, positionY, positionX + width -1, positionY + height -1);
    fillActiveArea(color);
}
