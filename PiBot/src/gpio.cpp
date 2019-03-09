/*
 * gpio.cpp
 *
 *  Created on: 5 sty 2019
 *      Author: Marcin
 */

#include "logger.h"
#include "gpio.h"
#include "program.h"
#include <stdlib.h>

void GPIO::initialize(void)
{
	if(gpioInitialise() < 0)
	{
		Program::getInstance().terminate(GPIO_INITIALIZATION_ERROR);
	}
}

void GPIO::terminate(void)
{
	gpioTerminate();
}

GPIO::GPIO(GpioPin gpioNumber, unsigned mode, unsigned pull)
	: gpio_number(gpioNumber)
{
	auto result = gpioSetMode(gpioNumber, mode);
	if(result == PI_BAD_GPIO)
	{
		Program::getInstance().terminate(BAD_GPIO_NUMBER);
	}
	if(result == PI_BAD_MODE)
	{
		Program::getInstance().terminate(BAD_GPIO_MODE);
	}
	gpioSetPullUpDown(gpioNumber, pull);
}
