/*
 * program.cpp
 *
 *  Created on: 7 sty 2019
 *      Author: Marcin
 */

#include "program.h"
#include "gpio.h"

Program& Program::getInstance(void)
{
	// instantiated on the first use and guaranteed to be destroyed
	static Program instance;
	return instance;
}

Program::~Program()
{
	GPIO::terminate();
}

void Program::initialize(void)
{
	GPIO::initialize();
}

void Program::terminate(void)
{

}
