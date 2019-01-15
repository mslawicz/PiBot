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
	/*
	 * if the 'help' option is provided, display help and terminate before any initializations
	 */
	if (isOptionProvided("-h") || isOptionProvided("--help"))
	{
		displayHelp();
		terminate(HELP_REQUEST);
	}

	if (isOptionProvided("-s"))
	{
		// logger sinks declaration
		auto params = options.find("-s")->second;
		for (auto param : params)
		{
			if (!param.empty())
			{
				Logger::getInstance().addSink(param[0]);
			}
		}
	}

	if (isOptionProvided("-l"))
	{
		// logger severity threshold declaration
		auto params = options.find("-l")->second;
		if (!params.empty())
		{
			// there's at least one parameter defined
			if (params[0] == "none")
			{
				Logger::getInstance().setLevelThreshold(MessageLevel::NONE);
			}
			else if (params[0] == "warning")
			{
				Logger::getInstance().setLevelThreshold(MessageLevel::WARNING);
			}
			else if (params[0] == "info")
			{
				Logger::getInstance().setLevelThreshold(MessageLevel::INFO);
			}
			else if (params[0] == "debug")
			{
				Logger::getInstance().setLevelThreshold(MessageLevel::DEBUG);
			}
		}

	}

	GPIO::initialize();
}

/*
 * program termination function
 * it should be called always on exiting
 * it calls exit() when an exit code is provided
 */
void Program::terminate(ExitCode exitCode)
{
	// map for printing
	const std::map<ExitCode, std::string> ExitMessages = {
		{OK, "Normal program exit"},
		{GENERAL_ERROR, "General error"},
		{SHELL_ERROR, "Shell error"},
		{GPIO_INITIALIZATION_ERROR, "GPIO initialization error"},
		{BAD_GPIO_NUMBER, "Bad GPIO number"},
		{BAD_GPIO_MODE, "Bad GPIO mode"},
		{MEMORY_ALLOCATION_ERROR, "Memory allocation error"},
		{HELP_REQUEST, "Program help requested"}
	};

	if (exitCode == ExitCode::OK)
	{
		Logger::getInstance().logEvent(INFO, "PiBot is shutting down");
	}
	else
	{
		Logger::getInstance().logEvent(ERROR, "PiBot is exiting with code ", exitCode, " (", ExitMessages.find(exitCode)->second, ")");
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

void Program::displayHelp(void)
{
	// vector of help info strings
	const std::vector<std::pair<std::string, std::string>> HelpInfoTexts = {
		{"-h | --help", "display help"},
		{"-l", "set log severity threshold: none | error | warning | info | debug"},
		{"-s", "define logger sinks: console, display"}
	};

	for (auto line : HelpInfoTexts)
	{
		std::cout << std::setw(20) << std::left << line.first.c_str() << line.second.c_str() << std::endl;
	}
}
