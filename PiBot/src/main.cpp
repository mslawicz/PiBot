/*
 * main.cpp
 *
 *  Created on: 4 sty 2019
 *      Author: Marcin
 */

#include "gpio.h"
#include "i2c.h"
#include "program.h"
#include "logger.h"
#include "drive.h"	//XXX for test
#include <iostream>

int main(int argc, char* argv[])
{
	Logger::getInstance().logEvent(INFO, "PiBot started");
	Program::getInstance().parseArguments(argc, argv);
	Program::getInstance().initialize();


	std::cout << "gpio hardware revision: " << gpioHardwareRevision() << std::endl;
	GPIO UserKey(21, PI_INPUT, PI_PUD_UP);
	//GPIO GreenLED(23, PI_OUTPUT);

	gpioSetPullUpDown(2, PI_PUD_UP);	// XXX temporary for LSM9DS1 I2C purpose
	gpioSetPullUpDown(3, PI_PUD_UP);	// XXX temporary for LSM9DS1 I2C purpose

	//I2C Magnetometer(I2C1, 0x1E);	// LSM9DS1 - Magnetic sensor

	//auto Data = Magnetometer.read(0x20, 5);
	//std::cout << "the length of Data vector: " << Data.size() << std::endl;
//	for(auto Byte : Data)
//	{
//		std::cout << std::hex << (int)Byte << ",";
//	}
//	std::cout << std::endl;


	Drive testDrive;	//XXX test
	testDrive.start();	// XXX test
	while(UserKey.read());
	//gpioDelay(1000);
	testDrive.stop();	// XXX test




	Program::getInstance().terminate();
	return 0;
}

