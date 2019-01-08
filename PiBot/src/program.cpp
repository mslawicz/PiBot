/*
 * program.cpp
 *
 *  Created on: 7 sty 2019
 *      Author: Marcin
 */

#include "program.h"
#include "logger.h"
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
 * it should be called just before return or exit()
 */
void Program::terminate(void)
{
	Logger::getInstance().logEvent(INFO, "PiBot terminating");
}
