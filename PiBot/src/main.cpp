/*
 * main.cpp
 *
 *  Created on: 4 sty 2019
 *      Author: Marcin
 */

#include "program.h"
#include "logger.h"
#include "udp.h" //XXX test
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

	    while(!Program::getInstance().isExitRequest())
	    {
	        std::this_thread::sleep_for(std::chrono::milliseconds(12));
	    }
	    std::this_thread::sleep_for(std::chrono::milliseconds(100));

	    server.stop();
	}

	Program::getInstance().terminate();
	return 0;
}

