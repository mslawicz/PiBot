/*
 * command.cpp
 *
 *  Created on: 26 mar 2019
 *      Author: Marcin
 */

#include "cli.h"

#include "program.h"

CLI::CLI()
{
    commands.emplace_back(CommandStrings {"help", "h"}, "display the list of commands", []() { Program::getInstance().getConsole()->displayHelp(); });
    commands.emplace_back(CommandStrings {"exit", "quit", "x"}, "exit from program", []() { Program::getInstance().requestExit(); });
    commands.emplace_back(CommandStrings {"start"}, "start the robot control", []() { Program::getInstance().getRobot()->start(); });
    commands.emplace_back(CommandStrings {"stop"}, "stop the robot control", []() { Program::getInstance().getRobot()->stop(); });
    //commands.emplace_back(CommandStrings {"telemetry"}, "set the data telemetry: <IP> <port> | off", std::bind(&Console::setTelemetry, this));
}

void CLI::process(std::string input)
{
    commandLine.str(input);
    std::string command;
    commandLine >> command;
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
    }
    commandLine.ignore();
    commandLine.clear();
}

template <typename T> T CLI::getArgument(T min, T max, T def)
{
    if (commandLine.eof())
    {
        std::cout << "no more args, default used\n";
        return def;
    }
    else
    {
        T argument;
        commandLine >> argument;
        if (commandLine.fail())
        {
            std::cout << "invalid value, default used\n";
            return def;
        }
        if ((argument < min) || (argument > max))
        {
            argument = def;
            std::cout << "value out of range, default used\n";
        }
        return argument;
    }
}

std::string CLI::getArgument(void)
{
    std::string argument;
    if (commandLine.eof())
    {
        std::cout << "no more args, default used\n";
    }
    else
    {
        commandLine >> argument;
    }
    return argument;
}

