/*
 * main.cpp
 *
 *  Created on: 4 sty 2019
 *      Author: Marcin
 */

#include "gpio.h"
#include "program.h"
#include "logger.h"
#include "drive.h"	//XXX for test
#include "display.h" //XXX test
#include <iostream>
#include <chrono>
#include <cmath> //XXX for test
#include <iomanip>//XXX test

int main(int argc, char* argv[])
{
    Logger::getInstance().logEvent(INFO, "PiBot started");
	Program::getInstance().parseArguments(argc, argv);
	Program::getInstance().initialize();

	PushButton testPB(GpioPin::SW1);
	PushButton exitButton(GpioPin::TMP_EXIT);

	GPIO backlightPin(GpioPin::BACKLIGHT, PI_OUTPUT);
	backlightPin.write(0);

	Display display(SpiChannelId::MainSPI, SpiPriority::DISPLAY);   //XXX test

	while(!exitButton.hasBeenPressed())
	{
	    if(testPB.hasBeenPressed())
	    {
	        Logger::getInstance().logEvent(INFO, "SW1 has been pressed");
	    }
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(250));


	Program::getInstance().terminate();
	return 0;
}

