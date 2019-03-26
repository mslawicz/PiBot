/*
 * command.h
 *
 *  Created on: 26 mar 2019
 *      Author: Marcin
 */

#ifndef SRC_COMMAND_H_
#define SRC_COMMAND_H_

#include <string>
#include <set>
#include <tuple>
#include <vector>
#include <functional>
#include <sstream>

typedef std::set<std::string> CommandStrings;
typedef std::tuple<CommandStrings, std::string, std::function<void(void)>> CommandContainer;

class Command
{
public:
    Command();
    void interpreter(std::string input);
private:
    template <typename T> T getArgument(T min, T max, T def);
    std::string getArgument(void);
    std::vector<CommandContainer> commands;
    std::stringstream commandLine;
};

#endif /* SRC_COMMAND_H_ */
