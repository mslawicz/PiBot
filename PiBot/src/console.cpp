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
        parseArguments();
        bool isValid = false;

        for (auto commandItem : commands)
        {
            if (std::get<0>(commandItem).find(command) != std::get<0>(commandItem).end())
            {
                isValid = true;
                std::get<2>(commandItem)();
                break;
            }
        }

        if (!isValid)
        {
            std::cout << "unknown command\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.clear();
        }

    } while (!Program::getInstance().isExitRequest());

    Logger::getInstance().logEvent(INFO, "Console handler exit");
}

/*
 * parse remaining arguments into vector of strings
 */
void Console::parseArguments(void)
{
    std::string line;
    std::string argument;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    arguments.clear();
    while(iss >> argument)
    {
        arguments.push_back(argument);
    }
}

/*
 * sets user console commands
 */
void Console::setCommands(void)
{
    commands.emplace_back(CommandStrings {"help", "h"}, "display the list of commands", std::bind(&Console::displayHelp, this));
    commands.emplace_back(CommandStrings {"exit", "quit", "x"}, "exit from program", []() { Program::getInstance().requestExit(); });
    commands.emplace_back(CommandStrings {"start"}, "start the robot control", []() { Program::getInstance().getRobot()->start(); });
    commands.emplace_back(CommandStrings {"stop"}, "stop the robot control", []() { Program::getInstance().getRobot()->stop(); });
    commands.emplace_back(CommandStrings {"telemetry"}, "set the data telemetry: <IP> <port> | off", std::bind(&Console::setTelemetry, this));
}

/*
 * displays help - list of commands
 */
void Console::displayHelp(void)
{
    for(auto commandItem : commands)
    {
        bool notFirst = false;
        for (auto command : std::get<0>(commandItem))
        {
            if(notFirst)
            {
                std::cout << " | ";
            }
            else
            {
                notFirst = true;
            }
            std::cout << command.c_str();
        }
        std::cout << " : " << std::get<1>(commandItem).c_str() << std::endl;
    }
}

bool Console::noOfArgumentsNotLessThan(unsigned number)
{
    if(arguments.size() < number)
    {
        std::cout << "too few arguments" << std::endl;
        return false;
    }
    return true;
}

/*
 * switch telemetry on/off
 */
void Console::setTelemetry(void)
{
    if(noOfArgumentsNotLessThan(1))
    {
        if(arguments[0] == "off")
        {
            // turn off telemetry
            Program::getInstance().getRobot()->telemetryEnable(false);
            Program::getInstance().getUdpClient()->disconnect();
        }
        else
        {
            // turn on telemetry
            if(noOfArgumentsNotLessThan(2))
            {
                Program::getInstance().getUdpClient()->setConnection(arguments[0], std::stoi(arguments[1]));
                Program::getInstance().getRobot()->telemetryEnable(true);
            }
        }
    }
}
