/*
 * gpio.cpp
 *
 *  Created on: 5 sty 2019
 *      Author: Marcin
 */

#include <stdlib.h>
#include "exit_codes.h"
#include "gpio.h"

void GPIO::Initialize(void)
{
	if(gpioInitialise() < 0)
	{
		exit(ExitCode::GPIO_INITIALIZATION_ERROR);
	}
}

void GPIO::Terminate(void)
{
	gpioTerminate();
}

GPIO::GPIO(unsigned gpio_number, unsigned mode, unsigned pull)
	: m_gpio(gpio_number)
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

