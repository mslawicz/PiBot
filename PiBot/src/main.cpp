/*
 * main.cpp
 *
 *  Created on: 4 sty 2019
 *      Author: Marcin
 */

#include "program.h"
#include "logger.h"
#include "drive.h"	//XXX for test
#include "display.h" //XXX test
#include "ili9341.h"
#include "gpio.h"
#include <iostream>
#include <chrono>
#include <cmath> //XXX for test
#include <iomanip>//XXX test

int main(int argc, char* argv[])
{
    Logger::getInstance().logEvent(INFO, "PiBot started");
	Program::getInstance().parseArguments(argc, argv);
	Program::getInstance().initialize();

	{
	    // scope of test objects
	    PushButton exitButton(GpioPin::SW4);

	    while(!exitButton.hasBeenPressed())
	    {

	    }

	}

	Program::getInstance().terminate();
	return 0;
}

