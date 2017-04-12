#include "ael.h"

#ifndef AEL_IO
#define AEL_IO

//[doc] get user entry from terminal
void _ael_input(aelinterpreter&ael, phrase&ph)
{
	if(ph.size() != 2)
	{
		std::cerr << "Error: " << ph[0] << " takes exactly 1 argument (" << ph.size()-1 << " given)" << std::endl;
		return;
	}
	tok I;
	getline(std::cin, I);
	ael.dictionary[ph[1]] = I;
}

void load_io_functions(aelinterpreter& ael) {
	ael.functions["input"] = _ael_input;
}

#endif