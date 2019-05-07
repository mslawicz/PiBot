/*
 * console.cpp
 *
 *  Created on: 13 mar 2019
 *      Author: Marcin
 */

#include "console.h"
#include "logger.h"
#include "program.h"
#include "cli.h"
#include <string>
#include <chrono>

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
    std::string commandLine;
    CLI cli(HostProcess::CONSOLE);
    Logger::getInstance().logEvent(INFO, "Console handler started");
    //std::cin.ignore();
    std::cin.clear();

    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << '>';
        std::this_thread::yield();
        std::getline(std::cin, commandLine);
        cli.process(commandLine);
        //std::cin.ignore(255, '\n');
        std::cin.clear();
    } while (!Program::getInstance().isExitRequest());

    Logger::getInstance().logEvent(INFO, "Console handler exit");
}

