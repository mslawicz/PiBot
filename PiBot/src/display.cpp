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

/*
 * display solid color rectangle
 */
void Display::drawRectangle(uint16_t positionX, uint16_t positionY, uint16_t width, uint16_t height, uint16_t color)
{
    setActiveArea(positionX, positionY, positionX + width -1, positionY + height -1);
    fillActiveArea(color);
}

void Display::test1()
{
    static uint16_t scroll = 20;    // it is the top index if scroll area
    writeDataRequest(Ili9341Registers::VSCRSADD, std::vector<uint16_t>{scroll++});  // scroll is incremented here
    if(scroll > 300)    // if index exceeds lower scroll index - initialize it again
    {
        scroll = 20;
    }
}

void Display::test2()
{

    setActiveArea(0,0,maxX,maxY);
    fillActiveArea(Ili9341Color::BLACK);

    drawRectangle(0,300,60,20, Ili9341Color::ORANGE);
    drawRectangle(60,300,60,20, Ili9341Color::MAGENTA);
    drawRectangle(120,300,60,20, Ili9341Color::YELLOW);
    drawRectangle(180,300,60,20, Ili9341Color::CYAN);

    drawRectangle(0,0,240,20, Ili9341Color::MAROON);

    writeDataRequest(Ili9341Registers::VSCRDEF, std::vector<uint16_t>{20, 280, 20});    // 20 pixels in the top and bottom fixed

    drawRectangle(10,30,100,50, Ili9341Color::RED);
    drawRectangle(40,100,100,50, Ili9341Color::GREEN);
    drawRectangle(20,160,150,50, Ili9341Color::BLUE);

}

void Display::test3()
{
    drawRectangle(220,30,20,20, rand() & 0xFFFF);
}
