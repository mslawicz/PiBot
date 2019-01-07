/*
 * logger.cpp
 *
 *  Created on: 7 sty 2019
 *      Author: Marcin
 */

#include "logger.h"
#include <iostream>

Logger& Logger::getInstance(void)
{
	// instantiated on the first use and guaranteed to be destroyed
	static Logger instance;
	return instance;
}

Logger::~Logger()
{

}

void Logger::logEvent(void)
{
	std::cout << "I have logged an event" << std::endl;
}
