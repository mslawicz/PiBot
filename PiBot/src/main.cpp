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
#include "LSM9DS1.h"  //XXX for test
#include <iostream>

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

	Gyroscope gyroscope(I2cBusId::I2C1, I2cDeviceAddress::GYROSCOPE_ADDR, I2cPriority::GYROSCOPE_PR);  // example of an i2c object
	Gyroscope magnetometer(I2cBusId::I2C1, I2cDeviceAddress::MAGNETOMETER_ADDR, I2cPriority::MAGNETOMETER_PR);  // example of an i2c object


//	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	//Drive testDrive;	//XXX test
	//testDrive.start();	// XXX test
	//while(UserKey.read());
	//gpioDelay(1000);
	//testDrive.stop();	// XXX test

	std::chrono::steady_clock::time_point event = std::chrono::steady_clock::now();

	while(terminatePin.read())
	{
	    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	    if(std::chrono::duration_cast<std::chrono::milliseconds>(now - event).count() >= 10)
	    {
	        loopMark.write(1);
	        event = now;
	        loopMark.pulse(100, 1);
	        gyroscope.clearReceiveQueue();
	        magnetometer.clearReceiveQueue();
	        gyroscope.writeData(ImuRegisters::CTRL_REG1_G, std::vector<char>{0x82});
	        magnetometer.readDataRequest(MagnetometerRegisters::WHO_AM_I_M, 1);
	        gyroscope.readDataRequest(ImuRegisters::WHO_AM_I, 1);
	        loopMark.write(0);
	    }

	}


	Program::getInstance().terminate();
	return 0;
}

