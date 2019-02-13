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

PushButton::PushButton(GpioPin gpioNumber, uint32_t debounceTime)
    : GPIO(gpioNumber, PI_INPUT, PI_PUD_UP)
    , debounce_time(debounceTime)
{
    state = 0;
    eventTime = 0;
    keyIsPressed = false;
    keyHasBeenPressed = false;
}

void PushButton::stateMachine(void)
{
    switch(state)
    {
    case 0:
        // idle state
        if(read() == 0)
        {
            // pushbutton pressed
            eventTime = gpioTick();
            state = 1;
        }
        break;
    case 1:
        // wait for stabilization after press
        if(read() == 1)
        {
            // bouncing or released
            state = 0;
        }
        else if(gpioTick() - eventTime >= debounce_time)
        {
            // key is pressed long enough
            keyIsPressed = keyHasBeenPressed = true;
            state = 5;
        }
        break;
    case 5:
        // key is pressed - wait for release
        if(read() == 1)
        {
            // key has been released
            keyIsPressed = false;
            state = 0;
        }
        break;
    }
}
