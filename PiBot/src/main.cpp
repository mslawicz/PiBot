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
#include "motor.h"  //XXX for test
#include <iostream>
#include <chrono>

int main(int argc, char* argv[])
{
	Logger::getInstance().logEvent(INFO, "PiBot started");
	Program::getInstance().parseArguments(argc, argv);
	Program::getInstance().initialize();


	std::cout << "gpio hardware revision: " << gpioHardwareRevision() << std::endl;
	GPIO terminatePin(21, PI_INPUT, PI_PUD_UP);
	GPIO loopMark(12, PI_OUTPUT);
	//GPIO GreenLED(23, PI_OUTPUT);

	gpioSetPullUpDown(2, PI_PUD_UP);	// XXX temporary for LSM9DS1 I2C purpose
	gpioSetPullUpDown(3, PI_PUD_UP);	// XXX temporary for LSM9DS1 I2C purpose

	Motor testMotor(I2cBusId::I2C1, I2cDeviceAddress::MOTOR_ADDR, I2cPriority::MOTOR_PR); // motor test object

	//	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	Drive testDrive;	//XXX test
	testDrive.start();	// XXX test
	//while(UserKey.read());
	//gpioDelay(1000);

	testMotor.test();

	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point event = start;
	std::chrono::steady_clock::time_point now;
	do
	{
	    now = std::chrono::steady_clock::now();
	    if(std::chrono::duration_cast<std::chrono::milliseconds>(now - event).count() > 250)
	    {
	        event = now;
	        if(testDrive.gyroXYZ.size()>=6)
	        {
	            std::cout << ((int16_t)(testDrive.gyroXYZ[0] | (testDrive.gyroXYZ[1] << 8))) / (double)0x7FFF << std::endl;
	        }
	        else
	        {
	            std::cout << "---" << std::endl;
	        }
	    }
	} while (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() < 1);


    testDrive.stop(); // XXX test

	Program::getInstance().terminate();
	return 0;
}

