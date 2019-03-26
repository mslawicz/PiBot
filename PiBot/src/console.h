/*
 * console.h
 *
 *  Created on: 13 mar 2019
 *      Author: Marcin
 */

#ifndef SRC_CONSOLE_H_
#define SRC_CONSOLE_H_

#include <thread>
#include <vector>
#include <set>
#include <tuple>
#include <string>

typedef std::set<std::string> CommandStrings;
typedef std::tuple<CommandStrings, std::string, std::function<void(void)>> CommandContainer;

class Console
{
public:
    Console();
    ~Console();
    void displayHelp(void);
private:
    void handler(void);
    void setCommands(void);
    void parseArguments(void);
    bool noOfArgumentsNotLessThan(unsigned number);
    void setTelemetry(void);
    std::thread* pConsoleHandlerThread;
    std::vector<CommandContainer> commands;
    std::vector<std::string> arguments;
};

#endif /* SRC_CONSOLE_H_ */
