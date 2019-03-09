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
    // enable pushbutton interrupts
    gpioGlitchFilter(keyPin, 20000);
    gpioSetAlertFuncEx(keyPin, ButtonMenuItem::pushbuttonAlertCallback, this);
    isActive = true;
}

/*
 * deactivates menu item and displays inactivate-state rectangle
 */
void ButtonMenuItem::deActivateItem(void)
{
    isActive = false;
    //disable pushbutton interrupts
    gpioSetAlertFuncEx(GpioPin::SW1, nullptr, this);
    Program::getInstance().getDisplay()->drawRectangle(positionX, positionY, Width, Height, InactivateColor);
}

/*
 * callback function for pushbutton generated interrupts
 * it must be a static method, but the pointer to drive object is passed as an argument
 * Parameter   Value    Meaning
 *
 * gpio        0-53     The GPIO which has changed state
 *
 * level       0-2      0 = change to low (a falling edge)
 *                      1 = change to high (a rising edge)
 *                      2 = no level change (interrupt timeout)
 *
 * tick        32 bit   The number of microseconds since boot
 *                      WARNING: this wraps around from
 *                      4294967295 to 0 roughly every 72 minutes
 *
 * pDriveObject pointer Pointer to an arbitrary object
 *
 */
void ButtonMenuItem::pushbuttonAlertCallback(int gpio, int level, uint32_t tick, void* pObject)
{
    static_cast<ButtonMenuItem*>(pObject)->pushbuttonService(gpio, level, tick);
}

/*
 * pushbutton service
*/
void ButtonMenuItem::pushbuttonService(int gpio, int level, uint32_t tick)
{
    Logger::getInstance().logEvent(INFO, "pushbutton state change: ", gpio, ", level=", level);
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
