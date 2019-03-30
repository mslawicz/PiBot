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
#include "gui.h"
#include "menu.h"
#include "console.h"
#include "robot.h"
#include "udp.h"
#include "config.h"
#include <vector>
#include <map>
#include <unordered_map>

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
	ButtonMenuItem& getPushbutton(GpioPin pushbuttonPin) { return pushbuttonMenu.find(pushbuttonPin)->second; }
	bool isExitRequest(void) const { return exitRequest; }
	void requestExit(void) { exitRequest = true; }
	Robot* getRobot(void) const { return pRobot; }
	UDP::Client* getUdpClient(void) const { return pUdpClient; }
	UDP::Server* getUdpServer(void) const { return pUdpServer; }
	Console* getConsole(void) const { return pConsole; }
	Config* getConfig(void) const { return pConfig; }
private:
	// private constructor prevents from creation more objects
	Program();
	void displayHelp(void);
	std::map<std::string, std::vector<std::string>> options;
	PCA9685* pDevicePCA9685;
	Display* pDisplay;
	GUI* pGui;
	std::unordered_map<GpioPin, ButtonMenuItem> pushbuttonMenu;
	volatile bool exitRequest;
	Console* pConsole;
	Robot* pRobot;
	UDP::Client* pUdpClient;
	UDP::Server* pUdpServer;
	Config* pConfig;
};

#endif /* SRC_PROGRAM_H_ */
