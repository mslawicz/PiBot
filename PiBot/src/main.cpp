/*
 * main.cpp
 *
 *  Created on: 4 sty 2019
 *      Author: Marcin
 */

#include <iostream>
#include "gpio.h"
#include "i2c.h"

int main(int argc, char* argv[])
{
	GPIO::Initialize();
	std::cout << "Hello from PiBot!" << std::endl;
	std::cout << "gpio hardware revision: " << gpioHardwareRevision() << std::endl;
	GPIO GreenLED(17, PI_OUTPUT);
	GPIO UserKey(21, PI_INPUT, PI_PUD_UP);

	gpioSetPullUpDown(2, PI_PUD_UP);
	gpioSetPullUpDown(3, PI_PUD_UP);
	I2C Gyroscope(I2C1, 0x6B);		// LSM9DS1 - Accelerometer and gyroscope
	//I2C Magnetometer(I2C1, 0x1E);	// LSM9DS1 - Magnetic sensor

	auto Data = Gyroscope.Read(0x0F, 1);

	std::cout << "the length of Data vector: " << Data.size() << std::endl;
	for(auto Byte : Data)
	{
		std::cout << Byte << ",";
	}
	std::cout << std::endl;
	gpioDelay(1000);

	std::cout << "Good bye!" << std::endl;



	GPIO::Terminate();
	return 0;
}

