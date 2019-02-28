/*
 * program.h
 *
 *  Created on: 7 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_PROGRAM_H_
#define SRC_PROGRAM_H_

#include "logger.h"
#include "i2c.h"
#include "PCA9685.h"
#include "display.h"
#include "menu.h"
#include <vector>
#include <map>

// singleton class of this program
// it provides necessary initializations
class Program
{
public:
	static Program& getInstance(void);
	void initialize(void);
	void terminate(ExitCode exitCode = ExitCode::OK);
	~Program();
	// these two must be declared for prevention from copying a singleton object
	Program(Program const&) = delete;
	Program& operator=(Program const&) = delete;
	void parseArguments(int argc, char* argv[]);
	// checks if this option is provided in program arguments
	bool isOptionProvided(std::string option) const {return options.find(option) != options.end();}
	// gets pointer to display object
	Display* getDisplay(void) const { return pDisplay; }
private:
	// private constructor prevents from creation more objects
	Program();
	void displayHelp(void);
	std::map<std::string, std::vector<std::string>> options;
	PCA9685* pDevicePCA9685;
	Display* pDisplay;
	std::vector<MenuItem> menu;
};

#endif /* SRC_PROGRAM_H_ */
