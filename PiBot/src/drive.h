/*
 * Drive.h
 *
 *  Created on: 16 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_DRIVE_H_
#define SRC_DRIVE_H_

#include "gpio.h"

class Drive
{
public:
	Drive();
	~Drive();
private:
	static void gyroInterruptCallback(int gpio, int level, uint32_t tick, void* pObject);
	GPIO* pGyroInterruptPort;
	GPIO* pGreenLED;	// for test only
};

#endif /* SRC_DRIVE_H_ */
