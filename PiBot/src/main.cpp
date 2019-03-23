/*
 * main.cpp
 *
 *  Created on: 4 sty 2019
 *      Author: Marcin
 */

#include "program.h"
#include "logger.h"
#include "config.h" //XXX test
#include "udp.h"    //XXX test
#include <iostream>
#include <chrono>

int main(int argc, char* argv[])
{
    Logger::getInstance().logEvent(INFO, "PiBot started");
	Program::getInstance().parseArguments(argc, argv);
	Program::getInstance().initialize();

	{
	    // scope of test objects
	    Config config;
	    auto lanIfs = config.getIpAddresses();
	    for(auto lanIf : lanIfs)
	    {
	        std::cout << lanIf.first.c_str() << " : " << lanIf.second.first.c_str() << " / " << lanIf.second.second.c_str() << std::endl;
	    }

	    int val = 0;
	    while(!Program::getInstance().isExitRequest())
	    {
	        val += (rand() % 20 - 10);
	        if(val > 100) val = 100;
	        if(val<-100) val = -100;
	        //Program::getInstance().getUdpClient()->sendData(val, "\n");
	        std::this_thread::sleep_for(std::chrono::milliseconds(12));
	    }
	    std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}

	Program::getInstance().terminate();
	return 0;
}

