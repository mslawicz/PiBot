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
#include <thread>
#include <mutex>
#include <condition_variable>
#include <tuple>
#include <queue>

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
	WRONG_I2C_DEVICE,
	WRONG_SERIAL_BUS,
	I2C_BUFFER_SIZE,
	BAD_MOTOR_NO,
	SPI_BUFFER_SIZE,
	WRONG_SPI_DEVICE,
	WRONG_SPI_CHANNEL,
	SPI_NOT_OPENED,
};

// definition of logged messages severity level
enum MessageLevel
{
	ERROR,
	WARNING,
	INFO,
	DEBUG,
	NONE
};

typedef std::tuple<MessageLevel, std::string> eventContainer;

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
		    // this mutex prevents from writing to messageStream by more than one thread at a time
		    std::lock_guard<std::mutex> lock(messageMutex);
		    currentMessageLevel = level;
		    messageStream.str(std::string());
			auto now = std::chrono::system_clock::now();
			auto time = std::chrono::system_clock::to_time_t(now);
			auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) - std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
			messageStream << std::put_time(std::localtime(&time), "%H:%M:%S.");
			messageStream << std::setw(3) << std::setfill('0') << milliseconds.count() << "->";
			messageStream << MessageLevelText.find(level)->second.c_str() << "->";
			takeNextArgument(args...);
			// restore default stream manipulators
			messageStream << std::dec;
		}
	}

	// these two must be declared for prevention from copying a singleton object
	Logger(Logger const&) = delete;
	Logger& operator=(Logger const&) = delete;

	/*
	 * adds a sink to sinks set
	 */
	void addSink(char sink) { sinks.insert(sink); }

	void start(MessageLevel level);
	void stop(void);

private:
	// map for printing message levels
	const std::map<MessageLevel, std::string> MessageLevelText = {
			{ERROR, "error"},
			{WARNING, "warning"},
			{INFO, "info"},
			{DEBUG, "debug"},
			{NONE, "none"}
	};

    // private constructor prevents from more objects creation
    Logger();

    // logger handler to be launched in a new thread
    void handler(void);

	/*
	 * put next argument into stream
	 */
	template<typename First, typename... Rest> void takeNextArgument(First arg0, Rest... args)
	{
		messageStream << arg0;
	    takeNextArgument(args...);
	}

	/*
	 * conclusion of the message stream formatting; it calls logger sink handler
	 */
	void takeNextArgument()
	    {
			{
                std::lock_guard<std::mutex> lock(queueMutex);
                eventQueue.push(eventContainer{currentMessageLevel, messageStream.str()});
			}
			queueEvent.notify_one();
	    }

	void sendMessage(std::string message);

    std::stringstream messageStream;

    // logger message level threshold
    MessageLevel levelThreshold;

    // sink container
    std::set<char> sinks;

    std::thread* pLoggerHandlerThread;
    bool exitHandler;
    std::mutex loggerHandlerMutex;
    std::condition_variable queueEvent;
    std::mutex messageMutex;
    MessageLevel currentMessageLevel;
    std::queue<eventContainer> eventQueue;
    std::mutex queueMutex;
};

#endif /* SRC_LOGGER_H_ */
