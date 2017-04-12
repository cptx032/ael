#include "ael.h"
#include <stdio.h>

#ifndef AEL_STRING
#define AEL_STRING

// strcat STR1 STR2 (DEST:optional).
// if DEST is not provided the result is stored in STR1
void _ael_strcat(aelinterpreter& ael, phrase& ph) {
	if (ph.size() != 3 && ph.size() != 4) {
		_ael_error(ph, "wrong numbers of arguments");
		return;
	}
	tok dest = ph.size() == 4 ? ph[3] : ph[1];
	ael.dictionary[dest] = ael.get_value(ph[1]) + ael.get_value(ph[2]);
}

// chr HEX_CODE DEST
void _ael_chr(aelinterpreter& ael, phrase& ph) {
	if (_ael_error_invalid_number_arguments(ph, 2)) {
		return;
	}
	char hex_chr = atoi(ael.get_value(ph[1]).c_str());
	std::string _result = "";
	_result += hex_chr;
	ael.dictionary[ph[2]] = _result;
}

void load_string_functions(aelinterpreter& ael) {
	ael.functions["strcat"] = _ael_strcat;
	ael.functions["chr"] = _ael_chr;
}

#endif