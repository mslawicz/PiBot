/*
 * logger.h
 *
 *  Created on: 7 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_LOGGER_H_
#define SRC_LOGGER_H_

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <chrono>
#include <iomanip>
#include <set>

// definition of program exit codes
enum ExitCode
{
	OK,
	GENERAL_ERROR,
	SHELL_ERROR,
	GPIO_INITIALIZATION_ERROR,
	BAD_GPIO_NUMBER,
	BAD_GPIO_MODE,
	MEMORY_ALLOCATION_ERROR,
	HELP_REQUEST,
	I2C_NOT_OPENED,
	WRONG_I2C_DEVICE
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
			auto now = std::chrono::system_clock::now();
			auto time = std::chrono::system_clock::to_time_t(now);
			auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) - std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
			message << std::put_time(std::localtime(&time), "%H:%M:%S.");
			message << std::setw(3) << std::setfill('0') << milliseconds.count() << "->";
			message << MessageLevelText.find(level)->second.c_str() << "->";
			takeNextArgument(args...);
			// restore default stream manipulators
			message << std::dec;
		}
	}

	// these two must be declared for prevention from copying a singleton object
	Logger(Logger const&) = delete;
	Logger& operator=(Logger const&) = delete;

	/*
	 * adds a sink to sinks set
	 */
	void addSink(char sink) { sinks.insert(sink); }

	/*
	 * sets logger level threshold
	 */
	void setLevelThreshold(MessageLevel level) { levelThreshold = level; }

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

	// sink container
	std::set<char> sinks;

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
