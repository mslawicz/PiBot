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
	GreenLED.Write(1);
	gpioDelay(500000);
	GreenLED.Write(0);
	gpioDelay(500000);
	GreenLED.Write(1);
	std::cout << "green LED level read: " << GreenLED.Read() << std::endl;
	gpioDelay(500000);
	GreenLED.Toggle();
	std::cout << "green LED level read: " << GreenLED.Read() << std::endl;
	GPIO::Terminate();
	return 0;
}

