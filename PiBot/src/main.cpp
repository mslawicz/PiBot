/*
 * main.cpp
 *
 *  Created on: 4 sty 2019
 *      Author: Marcin
 */

#include <iostream>
#include "gpio.h"
#include "i2c.h"
#include "program.h"
#include "logger.h"

int main(int argc, char* argv[])
{
	Program::getInstance().initialize();

	Logger::getInstance().logEvent();
	std::cout << "Hello from PiBot!" << std::endl;
	std::cout << "gpio hardware revision: " << gpioHardwareRevision() << std::endl;
	GPIO GreenLED(17, PI_OUTPUT);
	GPIO UserKey(21, PI_INPUT, PI_PUD_UP);

	gpioSetPullUpDown(2, PI_PUD_UP);
	gpioSetPullUpDown(3, PI_PUD_UP);
	I2C Gyroscope(I2C1, 0x6B);		// LSM9DS1 - Accelerometer and gyroscope
	I2C Magnetometer(I2C1, 0x1E);	// LSM9DS1 - Magnetic sensor

	auto Data = Gyroscope.read(0x15, 9);
	std::cout << "the length of Data vector: " << Data.size() << std::endl;
	for(auto Byte : Data)
	{
		std::cout << std::hex << (int)Byte << ",";
	}
	std::cout << std::endl;

	// the time between two devices readout is 0.14 ms
	Data.clear();
	Data.push_back(0xBA);
	Data.push_back(0x40);
	Magnetometer.write(0x20, Data);
	Data = Magnetometer.read(0x20, 5);
	std::cout << "the length of Data vector: " << Data.size() << std::endl;
	for(auto Byte : Data)
	{
		std::cout << std::hex << (int)Byte << ",";
	}
	std::cout << std::endl;
	gpioDelay(1000);

	std::cout << "Good bye!" << std::endl;



	Program::getInstance().terminate();
	return 0;
}

