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
#include "fonts.h"
#include "display.h"

class ButtonMenuItem
{
public:
    ButtonMenuItem(uint16_t itemPositionX, uint16_t itemPositionY, uint16_t foregroundColor, uint16_t backgroundColor, GpioPin itemKeyPin);
    ~ButtonMenuItem();
    void activateItem(std::string text);
    void deActivateItem(void);
    static const uint16_t Width;
    static const uint16_t Height;
    static const uint8_t* const pFont;
private:
    const uint16_t InactivateColor = Ili9341Color::GRAY;
    uint16_t positionX;
    uint16_t positionY;
    uint16_t activeForegroundColor;
    uint16_t activeBackgroundColor;
    GpioPin keyPin;
    PushButton* pKey;
};

#endif /* SRC_MENU_H_ */
