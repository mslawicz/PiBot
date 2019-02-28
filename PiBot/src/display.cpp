/*
 * display.cpp
 *
 *  Created on: 14 lut 2019
 *      Author: Marcin
 */

#include "display.h"
#include "logger.h"
#include "program.h"

#include "tahoma11.h"
#include "tahoma11b.h"
#include "tahoma14b.h"
#include "tahoma15.h"
#include "tahoma16b.h"

Display::Display()
    : Ili9341(SerialBusId::SPI_MAIN, SerialPriority::DISPLAY_PR)
{
    initialize();
    pFont = nullptr;
    characterSpace = 0;
    textFieldWidth = 0;
    textAlignment = TextAlignment::CENTER;
    currentBackgroundColor = Ili9341Color::BLACK;
    currentForegroundColor = Ili9341Color::WHITE;
    spaceWidth = 0;
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
    static uint16_t scroll = 15;    // it is the top index if scroll area
    writeDataRequest(Ili9341Registers::VSCRSADD, std::vector<uint16_t>{scroll++});  // scroll is incremented here
    if(scroll > 305)    // if index exceeds lower scroll index - initialize it again
    {
        scroll = 15;
    }
}

void Display::test2()
{

    setActiveArea(0,0,maxX,maxY);
    fillActiveArea(Ili9341Color::BLACK);

    setFont(FontTahoma15);
    textFieldWidth = 60;
    currentForegroundColor = Ili9341Color::BLACK;

    currentBackgroundColor = Ili9341Color::ORANGE;
    print(0, maxY-15, "exit");

    currentBackgroundColor = Ili9341Color::CYAN;
    print(60, maxY-15, "surprise");

    currentBackgroundColor = Ili9341Color::YELLOW;
    print(120, maxY-15, "scroll");

    currentBackgroundColor = Ili9341Color::GREEN;
    print(180, maxY-15, "invert");

    drawRectangle(0,0,240,15, Ili9341Color::MAROON);

    writeDataRequest(Ili9341Registers::VSCRDEF, std::vector<uint16_t>{15, 290, 15});    // 20 pixels in the top and bottom fixed

    currentBackgroundColor = Ili9341Color::BLACK;
    currentForegroundColor = Ili9341Color::WHITE;

    setFont(FontTahoma11);
    print(0, 30, "FontTahoma11 2345#78*");

    setFont(FontTahoma11b);
    print(0, 45, "FontTahoma11b 2345#78*");

    setFont(FontTahoma14b);
    print(0, 60, "FontTahoma14b 2345#78*");

    setFont(FontTahoma15);
    print(0, 80, "FontTahoma15 2345#78*");

    setFont(FontTahoma16b);
    print(0, 100, "FontTahoma16b 2345");
}

void Display::test3()
{
    drawRectangle(220,30,20,20, rand() & 0xFFFF);

}

/*
 * returns the rendered text width in pixels
 */
uint16_t Display::getTextWidth(std::string text)
{
    uint16_t textWidth = 0;
    if(pFont != nullptr)
    {
        for (unsigned i = 0; i < text.size(); i++)
        {
            if((text[i] >= pFont[4]) && (text[i] < pFont[4]+pFont[5]))
            {
                // character is in allowed range
                if(text[i] == 0x20)
                {
                    // it is space
                    textWidth += spaceWidth;
                }
                else
                {
                    // not a space
                    textWidth += pFont[6 + text[i] - pFont[4]];
                }
            }

            if(i > 0)
            {
                // add character-to-character space for every next character
                textWidth += characterSpace;
            }
        }
    }
    return textWidth;
}

/*
 * sets a new font for printing
 */
void Display::setFont(const uint8_t* pNewFont)
{
    pFont = pNewFont;
    // character space id calculated from the font height
    characterSpace = 1 + pFont[3] / 8;
    spaceWidth = 1 + pFont[3] / 3;
}

/*
 * sets current colors for drawing and printing
 */
void Display::setColor(uint16_t foregroundColor, uint16_t backgroundColor)
{
    currentForegroundColor = foregroundColor;
    currentBackgroundColor = backgroundColor;
}

/*
 * render the given text to display
 */
uint16_t Display::renderText(uint16_t positionX, uint16_t positionY, std::string text)
{
    if(pFont == nullptr)
    {
        return 0;
    }
    uint16_t textWidth = getTextWidth(text);
    uint16_t leftSpace = 0;
    uint16_t rightSpace = 0;

    // if text field is wider than text width - calculate left and right spaces
    if(textWidth <= textFieldWidth + characterSpace)
    {
        switch(textAlignment)
        {
        default:
        case TextAlignment::CENTER:
            leftSpace = (textFieldWidth - textWidth) / 2;
            rightSpace = textFieldWidth - textWidth - leftSpace;
            break;
        case TextAlignment::LEFT:
            leftSpace = characterSpace;
            rightSpace = textFieldWidth - textWidth - leftSpace;
            break;
        case TextAlignment::RIGHT:
            rightSpace = characterSpace;
            leftSpace = textFieldWidth - textWidth - rightSpace;
            break;
        }
    }

    uint8_t height = pFont[3];

    // set the display area for this rendered text
    setActiveArea(positionX, positionY, positionX + textWidth + leftSpace + rightSpace - 1, positionY + height -1);

    // for every raw of pixels
    for (uint8_t pixelRaw = 0; pixelRaw < height; pixelRaw++)
    {
        // create the vector of pixels
        std::vector<uint16_t> pixels;

        // insert left padding pixels
        pixels.insert(pixels.end(), leftSpace, currentBackgroundColor);

        // for every character in the text
        for (unsigned charIndex = 0; charIndex < text.size(); charIndex++)
        {
            // add char-to-char space if it is not the first character
            if(charIndex > 0)
            {
                for (uint8_t i = 0; i < characterSpace; i++)
                {
                    pixels.push_back(currentBackgroundColor);
                }
            }

            // character in the text
            char character = text[charIndex];

            // check if character is in the allowed range
            if((character < pFont[4]) || (character >= pFont[4]+pFont[5]))
            {
                // character not in the coded range
                continue;
            }

            // index of this character in the font definitions
            uint8_t charTabIndex = character - pFont[4];
            // width of this character in pixels
            uint8_t charWidth = pFont[6 + charTabIndex];
            // add width of characters up to the current one
            unsigned cumulativeWidth = 0;
            for (uint8_t chI = 0; chI < charTabIndex; chI++)
            {
                cumulativeWidth += pFont[6 + chI];
            }
            // calculate index of the character definition in the font file
            unsigned charDefinitionIndex = cumulativeWidth * ((height - 1) / 8 + 1) + 6 + pFont[5];
            // calculate the page of pixel raws (page has up to 8 raws)
            uint8_t charPageIndex = pixelRaw / 8;
            // calculate extra shift of pixel byte (required for the last page)
            int extraShift = (charPageIndex + 1) * 8 - height;
            if (extraShift < 0)
            {
                extraShift = 0;
            }

            // if it is space - ignore the its coded width
            if(character == 0x20)
            {
                charWidth = spaceWidth;
            }

            // for every column of pixels in the char
            for (uint8_t charColumnIndex = 0; charColumnIndex < charWidth; charColumnIndex++)
            {
                // get the byte of pixels
                uint8_t pixelPattern = pFont[charDefinitionIndex + charPageIndex * charWidth + charColumnIndex];
                // if character is space - all bits are background
                if(character == 0x20)
                {
                    pixelPattern = 0;
                }

                if ((pixelPattern >> (pixelRaw % 8 + extraShift)) & 0x01)
                {
                    // the pixel is foreground
                    pixels.push_back(currentForegroundColor);
                }
                else
                {
                    // the pixel is background
                    pixels.push_back(currentBackgroundColor);
                }
            }

        }

        // insert right padding pixels
        pixels.insert(pixels.end(), rightSpace, currentBackgroundColor);

        // send raw of pixels to display
        writeDataRequest(pixelRaw == 0 ? Ili9341Registers::RAMWR : Ili9341Registers::WRCONT, pixels);
    }

    // return the width of rendered text
    return textWidth + leftSpace + rightSpace;
}

/*
 * prints text formatted with variable arguments
 */
template<typename... Args> void Display::print(uint16_t positionX, uint16_t positionY, Args... args)
    {
        std::stringstream textStream;
        takeNextArgument(textStream, args...);
        renderText(positionX, positionY, textStream.str());
    }

/*
 * adds next argument to the text stream
 */
template<typename First, typename... Rest> void Display::takeNextArgument(std::stringstream& textStream, First arg0, Rest... args)
{
    textStream << arg0;
    takeNextArgument(textStream, args...);
}

/*
 * clears screen - height raws from positionY
 */
void Display::clearScreen(uint16_t positionY, uint16_t height)
{
    drawRectangle(0, positionY, maxX, height, Ili9341Color::BLACK);
}

/*
 * clears the whole screen
 */
void Display::clearScreen(void)
{
    clearScreen(0, maxY);
}
