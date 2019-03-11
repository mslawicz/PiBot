/*
 * menu.h
 *
 *  Created on: 27 lut 2019
 *      Author: Marcin
 */

#ifndef SRC_MENU_H_
#define SRC_MENU_H_

#include "gpio.h"
#include "fonts.h"
#include "display.h"
#include <iostream>
#include <functional>

class TestClass
{
public:
    void testMethod(void);
};

class ButtonMenuItem
{
public:
    ButtonMenuItem(uint16_t itemPositionX, uint16_t itemPositionY, GpioPin itemKeyPin);
    ~ButtonMenuItem();
    void activate(std::string text, uint16_t foregroundColor, uint16_t backgroundColor);
    void deActivate(void);
    bool hasBeenPressed(void);
    void bindAction(std::function<void(void)> callbackFunction) { action = callbackFunction; }
    static const uint16_t Width;
    static const uint16_t Height;
    static const uint8_t* const pFont;
private:
    // callback function for pushbutton generated alerts
    static void pushbuttonAlertCallback(int gpio, int level, uint32_t tick, void* pObject);
    // pushbutton service
    void pushbuttonService(int gpio, int level, uint32_t tick);
    const uint16_t InactivateColor = Ili9341Color::DARK_GRAY;
    uint16_t positionX;
    uint16_t positionY;
    GpioPin keyPin;
    bool isActive;
    volatile bool buttonPressed;
    std::function<void(void)> action;
};

class ScreenMenuItem
{
public:
    ScreenMenuItem(std::string itemText);
    std::string getItemText(void) const { return text; }
private:
    std::string text;
};

class ScreenMenu
{
public:
    ScreenMenu(std::string menuTitle, std::vector<ScreenMenuItem> menuItems);
    void displayItems(void);
private:
    std::string title;
    std::vector<ScreenMenuItem> items;
    uint8_t cursorPosition;
};

#endif /* SRC_MENU_H_ */
