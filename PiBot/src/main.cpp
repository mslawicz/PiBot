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
	std::cout << "Hello from PiBot!!!" << std::endl;
	return 0;
}

