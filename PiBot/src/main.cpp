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
        uint16_t value = 0x0000;
        while(terminatePin.read())
        {
            //testMotor.test();
            //testMotor.setSpeed(speed);
            //testMotor.readDataRequest(0x26, 12);
            uint8_t valH = (value >> 8) & 0x0F;
            uint8_t valL = value & 0xFF;
            testMotor.writeData(PCA9685Registers::LED0_ON_L+8*4, std::vector<uint8_t>{0x00, 0x00, valL, valH});
            testMotor.readDataRequest(PCA9685Registers::LED0_ON_L+8*4, 4);
            while(testMotor.receiveQueueEmpty());
            auto receivedData = testMotor.getData();
            if(std::get<2>(receivedData)[2] != valL)
            {
                Logger::getInstance().logEvent(ERROR, "valL");
            }
            if(std::get<2>(receivedData)[3] != valH)
            {
                Logger::getInstance().logEvent(ERROR, "valH");
            }
            value += 11;
            value = value & 0xFFF;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            //speed += delta;
            speed *= -1.0;
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

