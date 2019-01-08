/*
 * logger.h
 *
 *  Created on: 7 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_LOGGER_H_
#define SRC_LOGGER_H_

#include<string>
#include<iostream>
#include<map>

enum ExitCode
{
	OK,
	GENERAL_ERROR,
	SHELL_ERROR,
	GPIO_INITIALIZATION_ERROR,
	BAD_GPIO_NUMBER,
	BAD_GPIO_MODE,
	MEMORY_ALLOCATION_ERROR
};

enum MessageLevel
{
	ERROR,
	WARNING,
	INFO,
	DEBUG
};

// singleton class of logger
// it provides necessary initializations
class Logger
{
public:
	static Logger& getInstance(void);
	~Logger();

	template<typename... Args> void logEvent(MessageLevel level, Args... args)
	{
		std::cout << MessageLevelText.find(level)->second.c_str() << ":";
		takeNextArgument(args...);
	}

	// these two must be declared for prevention from copying a singleton object
	Logger(Logger const&) = delete;
	Logger& operator=(Logger const&) = delete;

private:
	const std::map<MessageLevel, std::string> MessageLevelText = {
			{ERROR, "error"},
			{WARNING, "warning"},
			{INFO, "info"},
			{DEBUG, "debug"}
	};

	// private constructor prevents from more objects creation
	Logger();

	template<typename First, typename... Rest> void takeNextArgument(First arg0, Rest... args)
	{
	    std::cout << " " << arg0;
	    takeNextArgument(args...);
	}

	void takeNextArgument()
	    {
	        std::cout << std::endl;
	    }

};

#endif /* SRC_LOGGER_H_ */
