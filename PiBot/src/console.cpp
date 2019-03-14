/*
 * console.cpp
 *
 *  Created on: 13 mar 2019
 *      Author: Marcin
 */

#include "console.h"
#include "logger.h"
#include "program.h"
#include <string>
#include <chrono>

Console::Console()
{
    setCommands();
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
    std::string command;
    Logger::getInstance().logEvent(INFO, "Console handler started");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    do
    {
        std::cout << '>';
        std::this_thread::yield();
        std::cin >> command;
        auto iCommand = commands.find(command);
        if (iCommand != commands.end())
        {
            std::get<1>(iCommand->second)();
        }
        else
        {
            std::cout << "unknown command " << command << std::endl;
        }
    } while (!Program::getInstance().isExitRequest());

    Logger::getInstance().logEvent(INFO, "Console handler exit");
}

/*
 * sets user console commands
 */
void Console::setCommands(void)
{
    commands.emplace("help", CommandContainer{ "display the list of commands", std::bind(&Console::displayHelp, this) });
    commands.emplace("hello", CommandContainer{ "just a hello", []() {std::cout << "hi" << std::endl; } });
    commands.emplace("exit", CommandContainer{ "exit from program", []() { Program::getInstance().requestExit(); } });
}

/*
 * displays help - list of commands
 */
void Console::displayHelp(void)
{
    for(auto element : commands)
    {
        std::cout << element.first.c_str() << " : " << std::get<0>(element.second).c_str() << std::endl;
    }
}
