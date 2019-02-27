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
#include <sstream>

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
    void setColor(uint16_t foregroundColor, uint16_t backgroundColor);
    void setTextFieldWidth(uint16_t width) { textFieldWidth = width; }
    template<typename... Args> void print(uint16_t positionX, uint16_t positionY, Args... args);
private:
    uint16_t getTextWidth(std::string text);
    uint16_t renderText(uint16_t positionX, uint16_t positionY, std::string text);
    template<typename First, typename... Rest> void takeNextArgument(std::stringstream& textStream, First arg0, Rest... args);
    void takeNextArgument(std::stringstream& textStream){}
    const uint8_t* pFont;     // pointer to current font definition array
    uint8_t characterSpace;     // character-to-character space in pixels
    uint16_t textFieldWidth;    // total width of text field
    TextAlignment textAlignment;    // alignment of printed text
    uint16_t currentBackgroundColor;       // pixel current background color
    uint16_t currentForegroundColor;       // pixel current foreground color
    uint8_t spaceWidth;     // space (character 0x20) width in pixels
};

#endif /* SRC_DISPLAY_H_ */
