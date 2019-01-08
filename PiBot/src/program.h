/*
 * program.h
 *
 *  Created on: 7 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_PROGRAM_H_
#define SRC_PROGRAM_H_

#include "logger.h"

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
private:
	// private constructor prevents from more objects creation
	Program() {}
};

#endif /* SRC_PROGRAM_H_ */
