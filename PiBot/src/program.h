/*
 * program.h
 *
 *  Created on: 7 sty 2019
 *      Author: Marcin
 */

#ifndef SRC_PROGRAM_H_
#define SRC_PROGRAM_H_

// singleton class of this program
// it provides necessary initializations
class Program
{
public:
	static Program* instance(void);
	static Program* pointer_to_object;
	void initialize(void);
	void terminate(void);
private:
	// private constructor for getting a singleton feature
	Program(void) {}
};

#endif /* SRC_PROGRAM_H_ */
