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
#include <unistd.h>

struct EncoderData
{
	uint32_t noOfPulses;
	uint32_t ticks;
};

GPIO* pResult;
uint32_t g_bitMask;

class Encoder
{
public:
	static void encoderSamples(const gpioSample_t* sampleArray, int noOfSamples, void* userData);
	static std::vector<EncoderData> encoderData;
};

// a pointer to encoder sampling function
void (*samplingFunc)(const gpioSample_t* sampleArray, int noOfSamples, void* userData) = nullptr;

// this function is called every 1 ms
void Encoder::encoderSamples(const gpioSample_t* sampleArray, int noOfSamples, void* userData)
{
	static unsigned int counter = 0;
	static uint32_t lastTick = 0;
	static uint32_t lastLevel = 0;
	unsigned int pulses = 0;
	uint32_t lastDeltaTicks = 0;
	std::vector<EncoderData>* pEncoderData = (std::vector<EncoderData>*)userData;

	pResult->toggle();
	counter++;
	// this condition is for printing only from time to time
	if((counter % 1000) <= 2)
	{
		// let's check all samples stored
		for(int k=0; k<noOfSamples; k++)
		{
			uint32_t currentLevel = sampleArray[k].level;
			// filter only rising edge of encoder bit
			if(((currentLevel ^ lastLevel) & g_bitMask) & currentLevel)
			{
				// if encoder bit changes 0->1
				// then print the number of ticks (us) elapsed since the previous low-to-high edge
				lastDeltaTicks = sampleArray[k].tick - lastTick;
				std::cout << std::dec << std::setw(11) << lastDeltaTicks << ",";
				// and remember the current tick state
				lastTick = sampleArray[k].tick;
				// count pulses in this period
				pulses++;
			}
			// remember the level of every sample to be able to recognize a rising edge
			lastLevel = sampleArray[k].level;
		}

		std::cout << std::endl;
		//store latest encoder data to user data structure
		(*pEncoderData)[0].noOfPulses = pulses;
		(*pEncoderData)[0].ticks = lastDeltaTicks;
	}
}

// static member variable must be globally defined
std::vector<EncoderData> Encoder::encoderData;

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

	// pointer to sampling functions gets the function address
	samplingFunc = Encoder::encoderSamples;
	// add an alement to data vector (normally it hould be done by constructor of encoder object
	EncoderData aParticularEncoderData;
	Encoder::encoderData.push_back(aParticularEncoderData);
	// using extended version of the function to pass some user data
	gpioSetGetSamplesFuncEx(samplingFunc, g_bitMask, (void*)(&Encoder::encoderData));

//	while (userKey.read())
//	{
//
//	}
	sleep(5);
	std::cout << "last encoder data:   period=" << Encoder::encoderData[0].ticks << "us,  number of pulses=" << Encoder::encoderData[0].noOfPulses << std::endl;


	delete pResult;
	Program::getInstance().terminate();
	return 0;
}

