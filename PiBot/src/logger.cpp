/*
 * logger.cpp
 *
 *  Created on: 7 sty 2019
 *      Author: Marcin
 */

#include "logger.h"

/*
 * get instance of the logger singleton object
 */
Logger& Logger::getInstance(void)
{
	// instantiated on the first use and guaranteed to be destroyed
	static Logger instance;
	return instance;
}

Logger::Logger()
{
	// default logger level threshold
	levelThreshold = MessageLevel::ERROR;
}

Logger::~Logger()
{

}

/*
 * logger output sink handler
 */
void Logger::sendMessage(std::string message)
{
	if(sinks.find('c') != sinks.end())
	{
		std::cout << message << std::endl;
	}
}
