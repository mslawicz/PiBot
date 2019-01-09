/*
 * program.cpp
 *
 *  Created on: 7 sty 2019
 *      Author: Marcin
 */

#include "program.h"
#include "gpio.h"

/*
 * get the singleto program object
 */
Program& Program::getInstance(void)
{
	// instantiated on the first use and guaranteed to be destroyed
	static Program instance;
	return instance;
}

Program::~Program()
{
	// gpio termination should be called in the very end
	// thus it is called in the program object destructor
	GPIO::terminate();
}

/*
 * program initialization function
 * hardware initialization should be provided here
 */
void Program::initialize(void)
{
	GPIO::initialize();
}

/*
 * program termination function
 * it should be called always on exiting
 * it calls exit() when an exit code is provided
 */
void Program::terminate(ExitCode exitCode)
{
	if (exitCode == ExitCode::OK)
	{
		Logger::getInstance().logEvent(INFO, "PiBot is shutting down");
	}
	else
	{
		Logger::getInstance().logEvent(ERROR, "PiBot is exiting with code ", exitCode);
	}

	//TODO: save log to file here

	if (exitCode != ExitCode::OK)
	{
		exit(exitCode);
	}
}

/*
 * parses main program arguments
 * creates a map of argument name and vector of argument's parameters
 */
void Program::parseArguments(int argc, char* argv[])
{
    std::string currentOptionName;
    std::vector<std::string> currentOptionParameters;
    std::stringstream logStream;
    // iterate through all program arguments (skip argument #0)
    for (int argNo = 1; argNo < argc; argNo++)
    {
        if (argv[argNo][0] == '-')
        {
            // program option found
            if (!currentOptionName.empty())
            {
                // its not the first option, so store the previous one
                options[currentOptionName] = currentOptionParameters;
                Logger::getInstance().logEvent(INFO, "Program parameter parsed: ", logStream.str().c_str());
                currentOptionName.clear();
                currentOptionParameters.clear();
                logStream.str(std::string());
            }
            // the name of current option
            currentOptionName = argv[argNo];
            logStream << currentOptionName;
        }
        else
        {
            // it's not an option,  it is a parameter
            if (!currentOptionName.empty())
            {
                // this condition skips everything until the first option
                currentOptionParameters.push_back(argv[argNo]);
                logStream << ' ' << argv[argNo];
            }
        }
    }
    // store the very last option if exists
    if (!currentOptionName.empty())
    {
        options[currentOptionName] = currentOptionParameters;
        Logger::getInstance().logEvent(INFO, "Program parameter parsed: ", logStream.str().c_str());
    }
}

