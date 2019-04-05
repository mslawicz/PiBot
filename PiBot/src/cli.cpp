/*
 * command.cpp
 *
 *  Created on: 26 mar 2019
 *      Author: Marcin
 */

#include "cli.h"

#include "program.h"
#include "logger.h"
#include <set>

CLI::CLI(HostProcess hostProcess)
    : host(hostProcess)
{
    if(host == HostProcess::CONSOLE)
    {
        // console only commands
        commands.emplace_back(CommandStrings {"help", "h"}, "display the list of commands", std::bind(&CLI::displayHelp, this));
        commands.emplace_back(CommandStrings {"ip"}, "display lan interface addresses", std::bind(&CLI::displayLanAddresses, this));
        commands.emplace_back(CommandStrings {"udp"}, "start/stop udp remote control: <port> | 0", std::bind(&CLI::serverUDP, this));
    }
    else
    {
        // not-console commands
        commands.emplace_back(CommandStrings {"yaw"}, "set yaw speed", [this]() { Program::getInstance().getRobot()->getDrive()->setYawSpeed(getArgument<float>(-1.0f, 1.0f, 0.0f)); });
        commands.emplace_back(CommandStrings {"pitch"}, "set target pitch value", [this]() { Program::getInstance().getRobot()->getDrive()->setTargetPitchAngularRate(getArgument<float>(-1.0f, 1.0f, 0.0f)); });
    }
    commands.emplace_back(CommandStrings {"exit", "quit", "x"}, "exit from program", []() { Program::getInstance().requestExit(); });
    commands.emplace_back(CommandStrings {"start"}, "start the robot control", []() { Program::getInstance().getRobot()->start(); });
    commands.emplace_back(CommandStrings {"stop"}, "stop the robot control", []() { Program::getInstance().getRobot()->stop(); });
    commands.emplace_back(CommandStrings {"telemetry"}, "connect to telemetry server: <IP> <port> | off", std::bind(&CLI::setTelemetry, this));
    commands.emplace_back(CommandStrings {"pid"}, "set motor PID value: p | i | d <value>", std::bind(&CLI::setPID, this));
}

void CLI::process(std::string input)
{
    if(host == HostProcess::UDP_SERVER)
    {
        Logger::getInstance().logEvent(MessageLevel::DEBUG, "Remote command: ", input.c_str());
    }
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

/*
 * get and display lan addresses
 */
void CLI::displayLanAddresses(void)
{
    auto lanIfs = Program::getInstance().getConfig()->getIpAddresses();
    for(auto lanIf : lanIfs)
    {
        std::cout << lanIf.first.c_str() << " : " << lanIf.second.first.c_str() << " / " << lanIf.second.second.c_str() << std::endl;
    }
}

/*
 * start / stop server UDP
 */
void CLI::serverUDP(void)
{
    int port = getArgument<int>(0, 0xFFFF, 5001);
    if(port)
    {
        // start server UDP
        Program::getInstance().getUdpServer()->start(port);
    }
    else
    {
        // stop server UDP
        Program::getInstance().getUdpServer()->stop();
    }
}

/*
 * set PID coefficient value
 */
void CLI::setPID(void)
{
    std::string term = getArgument("");
    if(term == "p" || term == "P")
    {
        Program::getInstance().getRobot()->getDrive()->getPitchPID()->setKp(getArgument<float>(0.0f, 10.0f, 0.5f));
    }
    else     if(term == "i" || term == "I")
    {
        Program::getInstance().getRobot()->getDrive()->getPitchPID()->setKi(getArgument<float>(0.0f, 1.0f, 0.1f));
    }
    else     if(term == "d" || term == "D")
    {
        Program::getInstance().getRobot()->getDrive()->getPitchPID()->setKd(getArgument<float>(0.0f, 2.0f, 0.2f));
    }
    else
    {
        // unknown PID term
        Logger::getInstance().logEvent(MessageLevel::WARNING, "Unknown PID term: ", term);
    }
}
