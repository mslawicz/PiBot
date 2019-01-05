/*
 * main.cpp
 *
 *  Created on: 4 sty 2019
 *      Author: Marcin
 */

#include <iostream>
#include "gpio.h"

void* myFunction(void* arg)
{
	static int counter = 0;
	while(1)
	{
		std::cout << (char*)arg << " #" << counter++ << std::endl;
		gpioDelay(1000000);
	}
}

int main(int argc, char* argv[])
{
	GPIO::Initialize();
	std::cout << "Hello from PiBot!" << std::endl;
	std::cout << "gpio hardware revision: " << gpioHardwareRevision() << std::endl;
	GPIO GreenLED(17, PI_OUTPUT);
	GPIO UserKey(21, PI_INPUT, PI_PUD_UP);
	pthread_t* pth;
	pth = gpioStartThread(myFunction, (void*)"another thread: loop");

	while(UserKey.Read())
	{
		GreenLED.Toggle();
		gpioDelay(200000);
	}

	gpioStopThread(pth);
	std::cout << "Good bye!" << std::endl;
	GPIO::Terminate();
	return 0;
}

