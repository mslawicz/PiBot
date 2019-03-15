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

typedef std::set<std::string> CommandStrings;
typedef std::tuple<CommandStrings, std::string, std::function<void(void)>> CommandContainer;

class Console
{
public:
    Console();
    ~Console();
private:
    void handler(void);
    void setCommands(void);
    void displayHelp(void);
    std::thread* pConsoleHandlerThread;
    std::vector<CommandContainer> commands;
};

#endif /* SRC_CONSOLE_H_ */
