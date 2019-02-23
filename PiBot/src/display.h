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
    void setBackLight(float level);
    void drawRectangle(uint16_t positionX, uint16_t positionY, uint16_t width, uint16_t height, uint16_t color);
    void test1(void);
    void test2(void);
    void test3(void);
    void setFont(const uint8_t* pNewFont);
private:
    uint16_t getTextWidth(std::string text);
    const uint8_t* pFont;     // pointer to current font definition array
    uint8_t characterSpace;     // character-to-character space in pixels
};

#endif /* SRC_DISPLAY_H_ */
