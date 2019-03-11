/*
 * menu.cpp
 *
 *  Created on: 27 lut 2019
 *      Author: Marcin
 */

#include "menu.h"
#include "program.h"
#include "display.h"

const uint8_t* const ButtonMenuItem::pFont = FontTahoma15;
const uint16_t ButtonMenuItem::Width = 60;
const uint16_t ButtonMenuItem::Height = 15;

ButtonMenuItem::ButtonMenuItem(uint16_t itemPositionX, uint16_t itemPositionY, GpioPin itemKeyPin)
    : positionX(itemPositionX)
    , positionY(itemPositionY)
    , keyPin(itemKeyPin)
{
    // set the pushbutton pin as input with pullup
    GPIO(itemKeyPin, PI_INPUT, PI_PUD_UP);
    deActivate();
    buttonPressed = false;
}

ButtonMenuItem::~ButtonMenuItem()
{
}

/*
 * activates menu item and displays the menu text
 */
void ButtonMenuItem::activate(std::string text, uint16_t foregroundColor, uint16_t backgroundColor)
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
void ButtonMenuItem::deActivate(void)
{
    action = nullptr;
    isActive = false;
    //disable pushbutton interrupts
    gpioSetAlertFuncEx(keyPin, nullptr, this);
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
    if (level == 0)
    {
        // the pushbutton has been just pressed
        buttonPressed = true;
        if (action != nullptr)
        {
            // there is an action to do upon pressing the key
            action();
        }
    }
}

/*
 * test on pressing the button since the last check
 */
bool ButtonMenuItem::hasBeenPressed(void)
{
    bool stateSinceLastCheck = buttonPressed;
    buttonPressed = false;
    return stateSinceLastCheck;
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

void TestClass::testMethod()
{
    Logger::getInstance().logEvent(INFO, "test method executed!");
}

/*
 * display items of the screen menu object
 */
void ScreenMenu::displayItems(void)
{
    Program::getInstance().getDisplay()->setTextFieldWidth(ButtonMenuItem::Width * 2);
    Program::getInstance().getDisplay()->setTextAlignment(TextAlignment::CENTER);
    Program::getInstance().getDisplay()->setColor(Ili9341Color::YELLOW);
    Program::getInstance().getDisplay()->setFont(FontTahoma15);
    Program::getInstance().getDisplay()->print(ButtonMenuItem::Width, 0, title.c_str());

    Program::getInstance().getDisplay()->setTextFieldWidth(0);
    Program::getInstance().getDisplay()->setTextAlignment(TextAlignment::LEFT);
    Program::getInstance().getDisplay()->setColor(Ili9341Color::WHITE);
    for(uint8_t row=0; row < items.size(); row++)
    {
        Program::getInstance().getDisplay()->print(20, ButtonMenuItem::Height + 10 + (Program::getInstance().getDisplay()->getCurrentFontHeight() + 2) * row, items[row].getItemText().c_str());
    }
}
