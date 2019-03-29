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
#include <iostream>
#include <chrono>

int main(int argc, char* argv[])
{
    Logger::getInstance().logEvent(INFO, "PiBot started");
	Program::getInstance().parseArguments(argc, argv);
	Program::getInstance().initialize();

	{
	    // scope of test objects
	    UDP::Server server;
	    server.start(5001);

	    Servo s0(SerialBusId::I2C1, SerialPriority::SERVO_PR, I2cDeviceAddress::SERVO_ADDR, 0);
	    Servo s1(SerialBusId::I2C1, SerialPriority::SERVO_PR, I2cDeviceAddress::SERVO_ADDR, 1);
	    Servo s2(SerialBusId::I2C1, SerialPriority::SERVO_PR, I2cDeviceAddress::SERVO_ADDR, 2);
	    Servo s3(SerialBusId::I2C1, SerialPriority::SERVO_PR, I2cDeviceAddress::SERVO_ADDR, 3);

	    float val = 0.0f;
	    while(!Program::getInstance().isExitRequest())
	    {
	        s0.setValue(val);
	        s1.setValue(val);
	        s2.setValue(val);
	        s3.setValue(val);
	        val += 0.2;
	        if(val>1.0) val = 0.0f;
	        std::this_thread::sleep_for(std::chrono::milliseconds(500));
	    }
	    std::this_thread::sleep_for(std::chrono::milliseconds(100));

	    server.stop();
	}

	Program::getInstance().terminate();
	return 0;
}

