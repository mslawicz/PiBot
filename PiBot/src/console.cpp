/*
 * console.cpp
 *
 *  Created on: 13 mar 2019
 *      Author: Marcin
 */

#include "console.h"
#include "logger.h"
#include <string>

Console::Console()
{
    pConsoleHandlerThread = new std::thread(&Console::handler, this);
}

Console::~Console()
{
    pConsoleHandlerThread->join();
    delete pConsoleHandlerThread;
}

/*
 * Console handler to be launched in a new thread
 */
void Console::handler(void)
{
    std::string line;
    Logger::getInstance().logEvent(INFO, "Console handler started");
    do
    {
        std::cout << '>';
        //std::this_thread::yield();
        std::getline(std::cin, line);
    } while (line != "exit");
    Logger::getInstance().logEvent(INFO, "Console handler exit");
}
