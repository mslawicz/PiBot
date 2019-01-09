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
#include<sstream>
#include<map>

// definition of program exit codes
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

// definition of logged messages severity level
enum MessageLevel
{
	NONE,
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

	/*
	 * log an event
	 * any number of arguments can be provided
	 */
	template<typename... Args> void logEvent(MessageLevel level, Args... args)
	{
		if(level <= levelThreshold)
		{
			message.str(std::string());
			message << MessageLevelText.find(level)->second.c_str() << ": ";
			takeNextArgument(args...);
		}
	}

	// these two must be declared for prevention from copying a singleton object
	Logger(Logger const&) = delete;
	Logger& operator=(Logger const&) = delete;

private:
	// map for printing message levels
	const std::map<MessageLevel, std::string> MessageLevelText = {
			{NONE, "none"},
			{ERROR, "error"},
			{WARNING, "warning"},
			{INFO, "info"},
			{DEBUG, "debug"}
	};

	std::stringstream message;

	// logger message level threshold
	MessageLevel levelThreshold;

	// private constructor prevents from more objects creation
	Logger();

	/*
	 * put next argument into stream
	 */
	template<typename First, typename... Rest> void takeNextArgument(First arg0, Rest... args)
	{
		message << arg0;
	    takeNextArgument(args...);
	}

	/*
	 * conclusion of the message stream formatting; it calls logger sink handler
	 */
	void takeNextArgument()
	    {
			sendMessage(message.str());
	    }

	void sendMessage(std::string message);
};

#endif /* SRC_LOGGER_H_ */
