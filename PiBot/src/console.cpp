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
    pCli = new CLI(HostProcess::CONSOLE);
    pConsoleHandlerThread = new std::thread(&Console::handler, this);
}

Console::~Console()
{
    pConsoleHandlerThread->join();
    delete pConsoleHandlerThread;
    delete pCli;
}

/*
 * Console handler to be launched in a new thread
 */
void Console::handler(void)
{
    std::string commandLine;
    Logger::getInstance().logEvent(INFO, "Console handler started");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    do
    {
        std::cout << '>';
        std::this_thread::yield();
        std::getline(std::cin, commandLine);
        pCli->process(commandLine);
    } while (!Program::getInstance().isExitRequest());

    Logger::getInstance().logEvent(INFO, "Console handler exit");
}

/*
 * switch telemetry on/off
 */
//void Console::setTelemetry(void)
//{
//    if(noOfArgumentsNotLessThan(1))
//    {
//        if(arguments[0] == "off")
//        {
//            // turn off telemetry
//            Program::getInstance().getRobot()->setTelemetry(false);
//            Program::getInstance().getUdpClient()->disconnect();
//        }
//        else
//        {
//            // turn on telemetry
//            if(noOfArgumentsNotLessThan(2))
//            {
//                Program::getInstance().getUdpClient()->setConnection(arguments[0], std::stoi(arguments[1]));
//                Program::getInstance().getRobot()->setTelemetry(true);
//            }
//        }
//    }
//}
