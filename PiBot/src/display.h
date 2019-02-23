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

enum TextAlignment
{
    CENTER = 0,
    LEFT,
    RIGHT
};

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
    uint16_t renderText(uint16_t positionX, uint16_t positionY, std::string text);
    const uint8_t* pFont;     // pointer to current font definition array
    uint8_t characterSpace;     // character-to-character space in pixels
    uint16_t textFieldWidth;    // total width of text field
    TextAlignment textAlignment;    // alignment of printed text
    uint16_t backgroundColor;       // pixel current background color
    uint16_t foregroundColor;       // pixel current foreground color
};

#endif /* SRC_DISPLAY_H_ */
