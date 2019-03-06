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
#include "mqtt.h"
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

        Program::getInstance().getButtonMenu(0).activateItem("exit");
        Program::getInstance().getButtonMenu(3).activateItem("menu");


        while(!Program::getInstance().getButtonMenu(0).keyHasBeenPressed())
        {

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }


        std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	Program::getInstance().terminate();
	return 0;
}

