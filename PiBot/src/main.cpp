/*
 * main.cpp
 *
 *  Created on: 4 sty 2019
 *      Author: Marcin
 */

#include <iostream>
#include "gpio.h"


int main(int argc, char* argv[])
{
	GPIO::Initialize();
	std::cout << "Hello from PiBot!" << std::endl;
	std::cout << "gpio hardware revision: " << gpioHardwareRevision() << std::endl;
	GPIO GreenLED(17, PI_OUTPUT);
	GPIO UserKey(21, PI_INPUT, PI_PUD_UP);

	while(UserKey.Read())
	{
		GreenLED.Toggle();
		gpioDelay(200000);
	}

	std::cout << "Good bye!" << std::endl;
	GPIO::Terminate();
	return 0;
}

