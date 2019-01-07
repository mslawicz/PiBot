/*
 * gpio.cpp
 *
 *  Created on: 5 sty 2019
 *      Author: Marcin
 */

#include <stdlib.h>
#include "logger.h"
#include "gpio.h"

void GPIO::initialize(void)
{
	if(gpioInitialise() < 0)
	{
		exit(ExitCode::GPIO_INITIALIZATION_ERROR);
	}
}

void GPIO::terminate(void)
{
	gpioTerminate();
}

GPIO::GPIO(unsigned gpio_number, unsigned mode, unsigned pull)
	: gpio_number(gpio_number)
{
	auto result = gpioSetMode(gpio_number, mode);
	if(result == PI_BAD_GPIO)
	{
		exit(ExitCode::BAD_GPIO_NUMBER);
	}
	if(result == PI_BAD_MODE)
	{
		exit(ExitCode::BAD_GPIO_MODE);
	}
	gpioSetPullUpDown(gpio_number, pull);
}

