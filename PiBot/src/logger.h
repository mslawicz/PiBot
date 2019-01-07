/*
 * logger.h
 *
 *  Created on: 7 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_LOGGER_H_
#define SRC_LOGGER_H_

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
	void logEvent(MessageLevel level);
	// these two must be declared for prevention from copying a singleton object
	Logger(Logger const&) = delete;
	Logger& operator=(Logger const&) = delete;
private:
	// private constructor prevents from more objects creation
	Logger() {}
};

#endif /* SRC_LOGGER_H_ */
