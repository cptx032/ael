#include "ael.h"
#include <stdio.h>

#ifndef AEL_STRING
#define AEL_STRING

// strcat STR1 STR2 (DEST:optional).
// if DEST is not provided the result is stored in STR1
void _ael_strcat(aelinterpreter &ael, phrase &ph)
{
	if (ph.size() != 3 && ph.size() != 4)
	{
		_ael_error(ph, "wrong numbers of arguments");
		return;
	}
	tok dest = ph.size() == 4 ? ph[3] : ph[1];
	ael.dictionary[dest] = ael.get_value(ph[1]) + ael.get_value(ph[2]);
}

// chr HEX_CODE DEST
void _ael_chr(aelinterpreter &ael, phrase &ph)
{
	if (_ael_error_invalid_number_arguments_exact(ph, 2))
	{
		return;
	}
	char hex_chr = atoi(ael.get_value(ph[1]).c_str());
	std::string _result = "";
	_result += hex_chr;
	ael.dictionary[ph[2]] = _result;
}

// strlen STR DEST
void _ael_strlen(aelinterpreter &ael, phrase &ph)
{
	if (_ael_error_invalid_number_arguments_exact(ph, 2))
	{
		return;
	}
	ael.dictionary[ph[2]] = to_string(ael.get_value(ph[1]).length());
}

// strindex STR INDEX DEST
void _ael_strindex(aelinterpreter &ael, phrase &ph)
{
	if (_ael_error_invalid_number_arguments_exact(ph, 3))
	{
		return;
	}
	std::string _str = ael.get_value(ph[1]);
	int index = atoi(ael.get_value(ph[2]).c_str());
	if (index >= _str.length())
	{
		_ael_error(ph, "index out of string bounds");
		return;
	}
	std::string result = "";
	result += _str[index];
	ael.dictionary[ph[3]] = result;
}

void load_string_functions(aelinterpreter &ael)
{
	ael.functions["strcat"] = _ael_strcat;
	ael.functions["chr"] = _ael_chr;
	ael.functions["strlen"] = _ael_strlen;
	ael.functions["strindex"] = _ael_strindex;
}

#endif
