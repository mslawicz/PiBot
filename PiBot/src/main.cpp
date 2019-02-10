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
#include <cmath> //XXX for test

int main(int argc, char* argv[])
{
    Logger::getInstance().logEvent(INFO, "PiBot started");
	Program::getInstance().parseArguments(argc, argv);
	Program::getInstance().initialize();


	GPIO terminatePin(21, PI_INPUT, PI_PUD_UP);
	//GPIO loopMark(12, PI_OUTPUT);
	//GPIO GreenLED(23, PI_OUTPUT);

	gpioSetPullUpDown(2, PI_PUD_UP);	// XXX temporary for LSM9DS1 I2C purpose
	gpioSetPullUpDown(3, PI_PUD_UP);	// XXX temporary for LSM9DS1 I2C purpose



	{
	    //scope of motor and drive objects
	    //Drive testDrive;    //XXX test
	    //testDrive.start();  // XXX test

        Motor testMotor(I2cBusId::I2C1, I2cDeviceAddress::MOTOR_ADDR, I2cPriority::MOTOR_PR, 0); // motor test object
        float speed = 0.8;
        float delta = 0.03;
        testMotor.test();
        while(0*terminatePin.read())
        {
            testMotor.setSpeed(speed);
            //testMotor.readDataRequest(0x26, 12);
            std::this_thread::sleep_for(std::chrono::milliseconds(7));
            speed += delta;
            if(fabs(speed)>1.0)
            {
                delta *= -1.0;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //testDrive.stop(); // XXX test
	}

	Program::getInstance().terminate();
	return 0;
}

