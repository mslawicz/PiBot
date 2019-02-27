/*
 * menu.h
 *
 *  Created on: 27 lut 2019
 *      Author: Marcin
 */

#ifndef SRC_MENU_H_
#define SRC_MENU_H_

#include <iostream>

class MenuItem
{
public:
    MenuItem(uint16_t itemPositionX, uint16_t ItemPositionY, unsigned itemKeyPin);
    ~MenuItem();
private:
    uint16_t positionX;
    uint16_t positionY;
    unsigned keyPin;
    std::string text;

};

#endif /* SRC_MENU_H_ */
