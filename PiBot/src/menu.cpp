/*
 * menu.cpp
 *
 *  Created on: 27 lut 2019
 *      Author: Marcin
 */

#include "menu.h"
#include "program.h"

const uint8_t* const MenuItem::pFont = FontTahoma15;
const uint16_t MenuItem::Width = 60;
const uint16_t MenuItem::Height = 15;

MenuItem::MenuItem(uint16_t itemPositionX, uint16_t itemPositionY, uint16_t foregroundColor, uint16_t backgroundColor, GpioPin itemKeyPin)
    : positionX(itemPositionX)
    , positionY(itemPositionY)
    , activeForegroundColor(foregroundColor)
    , activeBackgroundColor(backgroundColor)
    , keyPin(itemKeyPin)
{
    pKey = new PushButton(keyPin);
    deActivateItem();
}

MenuItem::~MenuItem()
{
    delete pKey;
}

/*
 * activates menu item and displays the menu text
 */
void MenuItem::activateItem(std::string text)
{
    Program::getInstance().getDisplay()->setTextFieldWidth(Width);
    Program::getInstance().getDisplay()->setColor(activeForegroundColor, activeBackgroundColor);
    Program::getInstance().getDisplay()->setFont(pFont);
    Program::getInstance().getDisplay()->print(positionX, positionY, text.c_str());
}

/*
 * deactivates menu item and displays inactivate-state rectangle
 */
void MenuItem::deActivateItem(void)
{
    Program::getInstance().getDisplay()->drawRectangle(positionX, positionY, Width, Height, InactivateColor);
}
