/*
 * menu.cpp
 *
 *  Created on: 27 lut 2019
 *      Author: Marcin
 */

#include "menu.h"

MenuItem::MenuItem(uint16_t itemPositionX, uint16_t ItemPositionY, unsigned itemKeyPin)
    : positionX(itemPositionX)
    , positionY(ItemPositionY)
    , keyPin(itemKeyPin)
{


}

MenuItem::~MenuItem()
{
    // TODO Auto-generated destructor stub
}

