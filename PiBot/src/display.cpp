/*
 * display.cpp
 *
 *  Created on: 14 lut 2019
 *      Author: Marcin
 */

#include "display.h"
#include "logger.h"
#include "program.h"
#include "tahoma11b.h"

Display::Display()
    : Ili9341(SerialBusId::SPI_MAIN, SerialPriority::DISPLAY_PR)
{
    initialize();
    pFont = nullptr;
    characterSpace = 0;
    textFieldWidth = 0;
    textAlignment = TextAlignment::CENTER;
    backgroundColor = Ili9341Color::BLACK;
    foregroundColor = Ili9341Color::WHITE;
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
    setFont(FontTahoma11b);
    Logger::getInstance().logEvent(INFO, "width of text 'abc'=", getTextWidth("abc"));
}

/*
 * returns the rendered text width in pixels
 */
uint16_t Display::getTextWidth(std::string text)
{
    uint16_t textWidth = 0;
    if(pFont != nullptr)
    {
        for (char& ch : text)
        {
            textWidth += pFont[6 + ch - pFont[4]] + characterSpace;
        }
        textWidth -= characterSpace;
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
}

/*
 * render the given text to display
 */
void Display::renderText(uint16_t positionX, uint16_t positionY, std::string text)
{
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
    setActiveArea(positionX, positionY, textWidth + leftSpace + rightSpace - 1, height -1);

    // for every raw of pixels
    for (uint8_t pixelRaw = 0; pixelRaw < height; pixelRaw++)
    {
        // create the vector of pixels
        std::vector<uint16_t> pixels;

        // insert left padding pixels
        pixels.insert(pixels.end(), leftSpace, backgroundColor);

        // for every character in the text
        for (unsigned charIndex = 0; charIndex < text.size(); charIndex++)
        {
            // add char-to-char space if it is not the first character
            if(charIndex > 0)
            {
                for (uint8_t i = 0; i < characterSpace; i++)
                {
                    pixels.push_back(backgroundColor);
                }
            }

            // character in the text
            char character = text[charIndex];
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

            // for every column of pixels in the char
            for (uint8_t charColumnIndex = 0; charColumnIndex < charWidth; charColumnIndex++)
            {
                // get the byte of pixels
                uint8_t pixelPattern = pFont[charDefinitionIndex + charPageIndex * charWidth + charColumnIndex];

                if ((pixelPattern >> (pixelRaw % 8 + extraShift)) & 0x01)
                {
                    // the pixel is foreground
                    pixels.push_back(foregroundColor);
                }
                else
                {
                    // the pixel is background
                    pixels.push_back(backgroundColor);
                }
            }

        }

        // insert right padding pixels
        pixels.insert(pixels.end(), rightSpace, backgroundColor);

        // send raw of pixels to display
        writeDataRequest(pixelRaw == 0 ? Ili9341Registers::RAMWR : Ili9341Registers::WRCONT, pixels);
    }
}
