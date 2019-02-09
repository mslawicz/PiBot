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
	// logger starts in info level
	levelThreshold = MessageLevel::INFO;
	pLoggerHandlerThread = nullptr;
	exitHandler = true;
	currentMessageLevel = MessageLevel::NONE;
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

/*
 * logger handler to be launched in a new thread
 * it sends queued messages to sinks
 */
void Logger::handler(void)
{
    logEvent(INFO, "logger handler started");
    do
    {
        std::this_thread::yield();
        std::unique_lock<std::mutex> lock(loggerHandlerMutex);
        queueEvent.wait(lock, [this]() {return (!eventQueue.empty() || exitHandler); });
        lock.unlock();
        while(!eventQueue.empty())
        {
            eventContainer event;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                event = eventQueue.front();
                eventQueue.pop();
            }
            if(std::get<0>(event) <= levelThreshold)
            {
                sendMessage(std::get<1>(event));
            }
        }


    } while (!exitHandler);
}

/*
 * starts logger
 */
void Logger::start(MessageLevel level)
{
    levelThreshold = level;
    if(level != MessageLevel::NONE)
    {
        exitHandler = false;
        pLoggerHandlerThread = new std::thread(&Logger::handler, this);
    }
}

/*
 * stops the logger
 */
void Logger::stop(void)
{
    logEvent(INFO, "logger stop request");
    exitHandler = true;
    queueEvent.notify_one();
    pLoggerHandlerThread->join();
    delete pLoggerHandlerThread;
}
