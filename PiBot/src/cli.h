/*
 * command.h
 *
 * Command Line Interpreter
 *
 *  Created on: 26 mar 2019
 *      Author: Marcin
 */

#ifndef SRC_CLI_H_
#define SRC_CLI_H_

#include <string>
#include <set>
#include <tuple>
#include <vector>
#include <functional>
#include <sstream>

enum HostProcess
{
    CONSOLE,
    UDP_SERVER
};

typedef std::set<std::string> CommandStrings;
typedef std::tuple<CommandStrings, std::string, std::function<void(void)>> CommandContainer;

class CLI
{
public:
    CLI(HostProcess hostProcess);
    void process(std::string input);
private:
    template <typename T> T getArgument(T min, T max, T def);
    std::string getArgument(std::string def);
    void displayHelp(void);
    void setTelemetry(void);
    void displayLanAddresses(void);
    void serverUDP(void);
    void setPPID(void);
    void setSPID(void);
    std::vector<CommandContainer> commands;
    std::stringstream commandLine;
    HostProcess host;
};

#endif /* SRC_CLI_H_ */
