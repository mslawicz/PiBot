/*
 * main.cpp
 *
 *  Created on: 4 sty 2019
 *      Author: Marcin
 */

#include <iostream>
#include <pigpio.h>

int main(int argc, char* argv[])
{
	if (gpioInitialise() < 0)
	   {
	      fprintf(stderr, "pigpio initialisation failed\n");
	      return 1;
	   }
	std::cout << "Hello from PiBot!" << std::endl;
	auto res = gpioSetMode(17, PI_OUTPUT);
	std::cout << "set pin 17: " << res << std::endl;
	std::cout << "gpio hardware revision: " << gpioHardwareRevision() << std::endl;
	gpioWrite(17, 1);
	gpioDelay(200000);
	gpioWrite(17, 0);
	gpioDelay(200000);
	gpioWrite(17, 1);
	gpioDelay(200000);
	gpioWrite(17, 0);
	gpioTerminate();
	return 0;
}

