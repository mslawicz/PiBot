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
        PushButton testPB(GpioPin::SW1);
        PushButton exitButton(GpioPin::SW4);

        Display display;   //XXX test
        display.setBackLight(0.2);


        Drive myDrive;  //XXX test
        myDrive.start();

        display.test2();
        while(!exitButton.hasBeenPressed())
        {
            //display.test2();
            if(testPB.hasBeenPressed())
            {
                Logger::getInstance().logEvent(INFO, "SW1 has been pressed");
                display.writeDataRequest(Ili9341Registers::INVON, std::vector<uint8_t>{});
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                display.writeDataRequest(Ili9341Registers::INVOFF, std::vector<uint8_t>{});
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        myDrive.stop();
        display.setBackLight(0.0);
	}

	Program::getInstance().terminate();
	return 0;
}

