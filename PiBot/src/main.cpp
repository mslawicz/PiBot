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
#include "menu.h"   //XXX test
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
	    Program::getInstance().getPushbutton(GpioPin::SW4).activate("return", Ili9341Color::WHITE, Ili9341Color::RED);
	    Program::getInstance().getPushbutton(GpioPin::SW2).activate("test", Ili9341Color::WHITE, Ili9341Color::BLUE);

	    // create a test object on which an action should be executed
	    TestClass myTest;
	    // bind the test object method as the action of the pushbutton
	    Program::getInstance().getPushbutton(GpioPin::SW2).bindAction(std::bind(&TestClass::testMethod, myTest));

	    while(!Program::getInstance().isExitRequest())
	    {

	    }
	    std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}

	Program::getInstance().terminate();
	return 0;
}

