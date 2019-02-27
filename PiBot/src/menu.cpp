/*
 * menu.cpp
 *
 *  Created on: 27 lut 2019
 *      Author: Marcin
 */

#include "menu.h"
#include "program.h"
#include "fonts.h"

MenuItem::MenuItem(uint16_t itemPositionX, uint16_t itemPositionY, uint16_t foregroundColor, uint16_t backgroundColor, GpioPin itemKeyPin)
    : positionX(itemPositionX)
    , positionY(itemPositionY)
    , activeForegroundColor(foregroundColor)
    , activeBackgroundColor(backgroundColor)
    , keyPin(itemKeyPin)
{
    pKey = new PushButton(keyPin);

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
    Program::getInstance().getDisplay()->setFont(FontTahoma15);
    Program::getInstance().getDisplay()->print(positionX, positionY, text.c_str());
}
