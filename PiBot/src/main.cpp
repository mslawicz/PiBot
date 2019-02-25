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
#include "ili9341.h"
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
        PushButton scroll(GpioPin::SW2);
        PushButton exitButton(GpioPin::SW4);

        Display display;   //XXX test
        display.setBackLight(0.2);


        Drive myDrive;  //XXX test
        myDrive.start();

        display.test2();
        while(!exitButton.hasBeenPressed())
        {
            if(scroll.isPressed())
            {
                display.test1();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        myDrive.stop();
        display.setBackLight(0.0);
	}

	Program::getInstance().terminate();
	return 0;
}

