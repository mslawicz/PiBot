/*
 * command.cpp
 *
 *  Created on: 26 mar 2019
 *      Author: Marcin
 */

#include "cli.h"

#include "program.h"

CLI::CLI(HostProcess hostProcess)
    : host(hostProcess)
{
    if(host == HostProcess::CONSOLE)
    {
        commands.emplace_back(CommandStrings {"help", "h"}, "display the list of commands", std::bind(&CLI::displayHelp, this));
    }
    commands.emplace_back(CommandStrings {"exit", "quit", "x"}, "exit from program", []() { Program::getInstance().requestExit(); });
    commands.emplace_back(CommandStrings {"start"}, "start the robot control", []() { Program::getInstance().getRobot()->start(); });
    commands.emplace_back(CommandStrings {"stop"}, "stop the robot control", []() { Program::getInstance().getRobot()->stop(); });
    commands.emplace_back(CommandStrings {"telemetry"}, "set the data telemetry: <IP> <port> | off", std::bind(&CLI::setTelemetry, this));
}

void CLI::process(std::string input)
{
    commandLine.ignore();
    commandLine.clear();
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
    if (!isValid && (host == HostProcess::CONSOLE))
    {
        std::cout << "unknown command\n";
    }
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

std::string CLI::getArgument(std::string def)
{
    std::string argument;
    if (commandLine.eof())
    {
        std::cout << "no more args, default used\n";
        argument = def;
    }
    else
    {
        commandLine >> argument;
    }
    return argument;
}

/*
 * displays help - list of commands
 */
void CLI::displayHelp(void)
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

/*
 * switch telemetry on/off
 */
void CLI::setTelemetry(void)
{
    std::string ipAddress(getArgument("off"));
    if(ipAddress == "off")
    {
        // turn off telemetry
        Program::getInstance().getRobot()->setTelemetry(false);
        Program::getInstance().getUdpClient()->disconnect();
    }
    else
    {
        // turn on telemetry
        Program::getInstance().getUdpClient()->setConnection(ipAddress, getArgument<int>(0, 65535, 8080));
        Program::getInstance().getRobot()->setTelemetry(true);
    }
}
