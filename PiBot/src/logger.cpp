/*
 * logger.cpp
 *
 *  Created on: 7 sty 2019
 *      Author: Marcin
 */

#include "logger.h"

Logger& Logger::getInstance(void)
{
	// instantiated on the first use and guaranteed to be destroyed
	static Logger instance;
	return instance;
}

Logger::Logger()
{

}

Logger::~Logger()
{

}

void Logger::sendMessage(std::string message)
{
	//TODO: time stamp and sinks

	// temporary console sink
	std::cout << message << std::endl;
}
