/*
 * main.cpp
 *
 *  Created on: 4 sty 2019
 *      Author: Marcin
 */

#include "program.h"
#include "logger.h"
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <chrono>

int main(int argc, char* argv[])
{
    Logger::getInstance().logEvent(INFO, "PiBot started");
	Program::getInstance().parseArguments(argc, argv);
	Program::getInstance().initialize();

	{
	    // scope of test objects
	    const int MaxHost = 20;
	    char host[MaxHost];
	    struct ifaddrs* pIfAddr = nullptr;
	    getifaddrs(&pIfAddr);
	    struct ifaddrs* pIfAddrCp = pIfAddr;
	    while(pIfAddr != nullptr)
	    {
	        if(pIfAddr->ifa_addr == nullptr)
	        {
	            continue;
	        }
	        std::cout << pIfAddr->ifa_name << ",";
	        auto family = pIfAddr->ifa_addr->sa_family;
	        std::cout << family << ",";
	        if(family == AF_INET)
	        {
	            int s = getnameinfo(pIfAddr->ifa_addr, sizeof(struct sockaddr_in), host, MaxHost, NULL, 0, NI_NUMERICHOST);
	            if(s)
	            {
	                std::cout << "  getnameinfo error\n";
	                continue;
	            }
	            std::cout << host;
	        }
	        std::cout << std::endl;
	        pIfAddr = pIfAddr->ifa_next;
	    }
	    freeifaddrs(pIfAddrCp);


	    while(!Program::getInstance().isExitRequest())
	    {
	        std::this_thread::sleep_for(std::chrono::milliseconds(10));
	    }
	    std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}

	Program::getInstance().terminate();
	return 0;
}

