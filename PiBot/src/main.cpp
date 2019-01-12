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
#include <iostream>

GPIO* pResult;
uint32_t g_bitMask;

void samplingFunc(const gpioSample_t* sampleArray, int noOfSamples)
{
	static unsigned int counter = 0;
	static int lastTick = 0;
	static uint32_t lastLevel = 0;

	pResult->toggle();
	counter++;
	if((counter % 1000) <= 1)
	{
		std::cout << noOfSamples << ": ";
		for(int k=0; k<noOfSamples; k++)
		{
			std::cout << std::dec << std::setw(11) << sampleArray[k].tick - lastTick << ",";
			std::cout << "0x" << std::setw(8) << std::hex << std::setfill(' ') << (sampleArray[k].level ^ lastLevel) << "\t";
			lastTick = sampleArray[k].tick;
			lastLevel = sampleArray[k].level;
		}

		std::cout << std::endl;
	}
}

int main(int argc, char* argv[])
{
	Logger::getInstance().logEvent(INFO, "PiBot started");
	Program::getInstance().parseArguments(argc, argv);
	Program::getInstance().initialize();


	std::cout << "gpio hardware revision: " << gpioHardwareRevision() << std::endl;
	GPIO greenLED(17, PI_OUTPUT);
	GPIO userKey(21, PI_INPUT, PI_PUD_UP);

	gpioSetPullUpDown(2, PI_PUD_UP);
	gpioSetPullUpDown(3, PI_PUD_UP);
	I2C Gyroscope(I2C1, 0x6B);		// LSM9DS1 - Accelerometer and gyroscope
	I2C Magnetometer(I2C1, 0x1E);	// LSM9DS1 - Magnetic sensor

	pResult = new GPIO(5, PI_INPUT);

	GPIO testPWM(6, PI_OUTPUT);
	gpioSetPWMfrequency(6, 1600);
	gpioPWM(6, 64);
	GPIO samplePort(12, PI_INPUT);
	g_bitMask = 1 << 12;
	gpioSetGetSamplesFunc(samplingFunc, g_bitMask);

	while (userKey.read())
	{

	}



	delete pResult;
	Program::getInstance().terminate();
	return 0;
}

