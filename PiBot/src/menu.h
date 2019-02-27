/*
 * menu.h
 *
 *  Created on: 27 lut 2019
 *      Author: Marcin
 */

#ifndef SRC_MENU_H_
#define SRC_MENU_H_

#include <iostream>
#include "gpio.h"

class MenuItem
{
public:
    MenuItem(uint16_t itemPositionX, uint16_t itemPositionY, uint16_t foregroundColor, uint16_t backgroundColor, GpioPin itemKeyPin);
    ~MenuItem();
    void activateItem(std::string text);
private:
    uint16_t positionX;
    uint16_t positionY;
    uint16_t activeForegroundColor;
    uint16_t activeBackgroundColor;
    GpioPin keyPin;
    PushButton* pKey;
};

#endif /* SRC_MENU_H_ */
