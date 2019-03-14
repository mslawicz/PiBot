/*
 * console.h
 *
 *  Created on: 13 mar 2019
 *      Author: Marcin
 */

#ifndef SRC_CONSOLE_H_
#define SRC_CONSOLE_H_

#include <thread>
#include <map>
#include <tuple>

typedef std::tuple<std::string, std::function<void(void)>> CommandContainer;

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
    std::map<std::string, CommandContainer> commands;
};

#endif /* SRC_CONSOLE_H_ */
