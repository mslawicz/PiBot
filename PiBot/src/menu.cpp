/*
 * menu.cpp
 *
 *  Created on: 27 lut 2019
 *      Author: Marcin
 */

#include "menu.h"
#include "program.h"

const uint8_t* const ButtonMenuItem::pFont = FontTahoma15;
const uint16_t ButtonMenuItem::Width = 60;
const uint16_t ButtonMenuItem::Height = 15;

ButtonMenuItem::ButtonMenuItem(uint16_t itemPositionX, uint16_t itemPositionY, GpioPin itemKeyPin)
    : positionX(itemPositionX)
    , positionY(itemPositionY)
    , keyPin(itemKeyPin)
{
    deActivateItem();
}

ButtonMenuItem::~ButtonMenuItem()
{
}

/*
 * activates menu item and displays the menu text
 */
void ButtonMenuItem::activateItem(std::string text, uint16_t foregroundColor, uint16_t backgroundColor)
{
    Program::getInstance().getDisplay()->setTextFieldWidth(Width);
    Program::getInstance().getDisplay()->setColor(foregroundColor, backgroundColor);
    Program::getInstance().getDisplay()->setFont(pFont);
    Program::getInstance().getDisplay()->print(positionX, positionY, text.c_str());
    isActive = true;
}

/*
 * deactivates menu item and displays inactivate-state rectangle
 */
void ButtonMenuItem::deActivateItem(void)
{
    Program::getInstance().getDisplay()->drawRectangle(positionX, positionY, Width, Height, InactivateColor);
    isActive = false;
}


/*
 * screen menu item constructor
 */
ScreenMenuItem::ScreenMenuItem(std::string itemText)
    : text(itemText)
{

}

/*
 * screen menu constructor
 */
ScreenMenu::ScreenMenu(std::string menuTitle, std::vector<ScreenMenuItem> menuItems)
    : title(menuTitle)
    , items(menuItems)
{
    cursorPosition = 0;
}
