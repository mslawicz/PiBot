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

	gpioSetPullUpDown(2, PI_PUD_UP);
	gpioSetPullUpDown(3, PI_PUD_UP);
	int I2C_handle = i2cOpen(1, 0x6B, 0);	//  LSM9DS1 - Accelerometer and gyroscope
	std::cout << "I2C handle: " << I2C_handle << std::endl;

	char data[10];
	//while(UserKey.Read())
	{
		int res = i2cReadI2CBlockData(I2C_handle, 0x0F, data, 1);
		std::cout << "I2C wrire result: " << res << std::endl;
		GreenLED.Toggle();
		gpioDelay(100000);
	}

	std::cout << "Good bye!" << std::endl;



	GPIO::Terminate();
	return 0;
}

