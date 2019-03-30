/*
 * main.cpp
 *
 *  Created on: 4 sty 2019
 *      Author: Marcin
 */

#include "program.h"
#include "logger.h"
#include "udp.h" //XXX test
#include "servo.h" //XXX test
#include <pigpio.h> //XXX test
#include <iostream>
#include <chrono>

int main(int argc, char* argv[])
{
    Logger::getInstance().logEvent(INFO, "PiBot started");
	Program::getInstance().parseArguments(argc, argv);
	Program::getInstance().initialize();

	{
	    // scope of test objects

	    gpioSetMode(14, PI_OUTPUT);
	    gpioServo(14, 0);
	    unsigned position = 1500;
	    std::this_thread::sleep_for(std::chrono::milliseconds(100));
	    while(!Program::getInstance().isExitRequest())
	    {
	        gpioServo(14, position);
	        position += 100;
	        if(position > 2500) position = 500;
	        std::this_thread::sleep_for(std::chrono::milliseconds(100));
	    }
	    std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}

	Program::getInstance().terminate();
	return 0;
}

