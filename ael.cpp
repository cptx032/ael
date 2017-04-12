/*
Author: Willie Lawrence - cptx032@gmail.com
*/

#include "ael.h"
#include "ael_io.h"
#include "ael_string.h"

std::vector<tok> args;

//[doc] retorna true se o argumento foi
// passado nos parametros
bool args_contains(std::string key)
{
	for(uint i=0;i<args.size();i++)
	{
		if(args[i] == key)
			return true;
	}
	return false;
}

//[doc] pega o valor do parametro
std::string param_get(std::string key)
{
	for(uint i=0;i<args.size();i++)
	{
		if(args[i] == key)
		{
			if(args.size() >= i+2)
				return args[i+1];
			else
				return "";
		}
	}
	return "";
}
void ael_argv(aelinterpreter& ael, phrase& ph)
{
	if(ph.size()!=3)
	{
		std::cerr << "Error: " << ph[0] << " takes exactly 2 arguments (" << ph.size()-1 << " given)" << std::endl;
		return;
	}
	uint arg_index = atoi(ael.get_value(ph[1]).c_str());
	ael.dictionary[ph[2]] = args[arg_index];
}

aelinterpreter i;
int main(int argc, char* argv[])
{
	load_main_ael_functions(i);
	load_io_functions(i);
	load_string_functions(i);
	for(int i=0;i<argc;i++)
	{
		args.push_back(argv[i]);
	}
	i.dictionary["argc"] = to_string(argc);
	i.functions["argv"] = ael_argv;
	if (argc == 1)
	{ // [fixme] > here document with interpreter
		std::cerr << argv[0] << ": missing file operand" << std::endl;
		exit(-1);
	}
	for(int i=0;i<argc;i++)
	{
		args.push_back(argv[i]);
	}
	if(args_contains("-v"))
	{
		std::cout << "Ael Version: " << AEL_VERSION << std::endl;
		exit(0);
	}
	phrase ph;
	if(param_get("-c") != "")
	{
		i.to_tokens(param_get("-c").c_str(), ph);
	}
	else
	{
		std::string script_file = get_file_content(argv[1]);
		i.to_tokens(script_file.c_str(), ph);
	}
	i.interprets(ph);
	return 0;
}
