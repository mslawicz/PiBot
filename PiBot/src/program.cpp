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
	Logger::getInstance().logEvent(INFO, "PiBot started");
	GPIO::initialize();
}

/*
 * program termination function
 * it should be called always on exiting
 * it calls exit() when an exit code is provided
 */
void Program::terminate(ExitCode exitCode)
{
	Logger::getInstance().logEvent(INFO, "PiBot terminating");

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
                currentOptionName.clear();
                currentOptionParameters.clear();
            }
            currentOptionName = argv[argNo];
        }
        else
        {
            // if not an option than it is a parameter
            if (!currentOptionName.empty())
            {
                // this condition skips everything until the first option
                currentOptionParameters.push_back(argv[argNo]);
            }
        }
    }
    if (!currentOptionName.empty())
    {
        // store the very last option
        options[currentOptionName] = currentOptionParameters;
    }
}

