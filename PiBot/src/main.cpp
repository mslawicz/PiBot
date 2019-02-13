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
#include <chrono>
#include <cmath> //XXX for test
#include <iomanip>//XXX test

int main(int argc, char* argv[])
{
    Logger::getInstance().logEvent(INFO, "PiBot started");
	Program::getInstance().parseArguments(argc, argv);
	Program::getInstance().initialize();


	GPIO terminatePin(GpioPin::SW5, PI_INPUT, PI_PUD_UP);
	GPIO backlightPin(GpioPin::BACKLIGHT, PI_OUTPUT);
	backlightPin.write(0);

	std::this_thread::sleep_for(std::chrono::milliseconds(250));


	Program::getInstance().terminate();
	return 0;
}

