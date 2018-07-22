/*
Author: Willie Lawrence - cptx032@gmail.com
*/

#ifndef AEL_H
#define AEL_H

#include <iostream>
#include <sstream> // cast double to string
#include <vector>
#include <unordered_map> // hash_map
#include <fstream> // reading file
#include <thread>

#define AEL_VERSION "0.0.3"

// when a function is not found the function saved
// in this var is runned
#define AEL_DEFAULT_FUNCTION "__ael_default_function"

// tok = token, in this case "token" is meant to be a "word"
typedef std::string tok;
typedef std::vector<tok> phrase;
typedef std::unordered_map<tok, tok> aeldict;
typedef aeldict(*aelnamefunction)(void);

typedef std::vector<tok> string_stack;


// used to print a phrase in terminal
void _ael_log(phrase &ph)
{
	for (int i=0; i < ph.size(); i++)
	{
		std::cout <<  "{" << ph[i] << "} ";
	}
	std::cout << std::endl;
}

// function to show formated "syntax/semantic" errors
void _ael_error(phrase &ph, std::string error)
{
	std::cerr << "Error: " << ph[0] << " " << error << "\nArgs:\n\t";
	_ael_log(ph);
}

bool _ael_error_invalid_number_arguments_exact(phrase &ph, int expected)
{
	if (ph.size() - 1 != expected)
	{
		// fixme: use _ael_error
		std::cerr << "Error: " << ph[0] << " takes exactly " << expected << " arguments (" << ph.size()-1 << " given)" << std::endl;
		std::cout << "Args:\n\t";
		_ael_log(ph);
		return true;
	}
	return false;
}

bool _ael_error_invalid_number_arguments_minimum(phrase &ph, int min_expected)
{
	if ((ph.size() - 1) < min_expected)
	{
		// fixme: use _ael_error
		std::cerr << "Error: " << ph[0] << " takes at least " << min_expected << " arguments (" << ph.size()-1 << " given)" << std::endl;
		std::cout << "Args:\n\t";
		_ael_log(ph);
		return true;
	}
	return false;
}

bool is_a_blank_char(char _c)
{
	if(_c==' '||_c=='\t'||_c=='\n'||_c=='\r')
		return true;
	return false;
}

// transforms float to string
tok to_string(double _v)
{
	std::ostringstream str_concatenated;
	str_concatenated << _v;
	return str_concatenated.str();
}

// returns the content of a text file
std::string get_file_content(const char* file_path)
{
	std::string contents;
	std::string line;
	std::ifstream script_file(file_path);
	
	if(script_file.is_open())
	{
		while (getline(script_file, line))
		{
			if (line[0] != '#')
			{
				contents += line + '\n';
			}
		}
		script_file.close();
	}
	else
	{
		std::cerr << "'" << file_path << "'" << " file not found" << std::endl;
	}
	return contents;
}


class aelinterpreter
{
public:
	// stores the variables
	aeldict dictionary;

	// defining "aelfunction": a function-type to ael-functions
	// each ael-function must return void and receive the interpreter
	// followed by a phrase
	typedef void(*aelfunction)(aelinterpreter&,phrase&);

	// stores the ael functions
	std::unordered_map<tok, aelfunction> functions;

	// stores the ael lists, called here by "stack"
	std::unordered_map<tok, string_stack> stack;

	aelinterpreter(){}

	// gets a string and parses it. the result of this process is a phrase
	// the phrase must be passed to to_tokens function
	void to_tokens(const char* _s, phrase &ph)
	{
		std::string script = _s;
		ph.push_back("");
		int block_counter = 0;
		char actual_char;
		for(int i=0; i < script.size(); i++)
		{
			actual_char = script[i];
			// se estiver dentro de um bloco
			if (block_counter > 0)
			{
				// e abrir outro bloco
				if (actual_char == '{')
				{
					// aumenta o contador de blocos
					// para saber quando realmente
					// o caractere '}' fecha o bloco
					block_counter++;
					ph[ph.size()-1] += actual_char;
				}
				// se ele fechar um bloco
				else if (actual_char == '}')
				{
					// decrementa o contador de blocos
					block_counter--;
					// se for maior que zero é porque
					// se fechou um bloco dentro de outro
					if(block_counter != 0)
					{
						// inclui o caractere '}'
						// porque faz parte da string
						ph[ph.size()-1] += actual_char;
					}
				}
				// se ele digitar qualquer outro caractere
				// dentro de um bloco
				// simplesmente adiciona o caractere
				else
				{
					ph[ph.size()-1] += actual_char;
				}
			}
			// se não houver blocos abertos
			else
			{
				// e ele abrir um bloco
				if (actual_char == '{')
				{
					// incrementa o contador de blocos
					// e não salva o caractere '{'
					// no próximo loop já não entra nesse
					// 'else'
					block_counter++;
				}
				// se é um caractere em branco e
				// não estiver dentro de um bloco
				else if (is_a_blank_char(actual_char)) // [fixme] >  and last is not blank
				{
					// adiciona outro token
					if(ph[ph.size()-1] != "")
						ph.push_back("");
				}
				// se o caractere é um ';' adiciona
				// outro token
				else if(actual_char == '.')
				{
					ph.push_back(".");
					ph.push_back("");
				}
				else
				{
					ph[ph.size()-1] += actual_char;
				}
			}
		}
		// se sobrar algum token vazio
		if(ph[ph.size()-1] == "")
		{
			ph.erase(ph.end());
		}
	}

	// return the value in variables dictionary
	// if value is not found the own key is returned
	tok get_value(tok key)
	{
		// if key starts with '"'
		if(key[0] == '{')
		{
			return key.substr(1, key.size()-2);
		}

		if(this->has_var(key))
		{
			// fixme: verificar se ainda há necessidade disso
			if(this->dictionary[key][0] == '{')
			{
				return this->get_value(this->dictionary[key]);
			}
			else
			{
				return this->dictionary[key];
			}
		}

		return key;
	}

	// returns true if a variable exists in variables dictionary
	bool has_var(tok var_identifier)
	{
		return this->dictionary.find(var_identifier) != this->dictionary.end();
	}

	// returns true if a variable exists in stack dictionary
	bool has_stack(tok var_identifier)
	{
		return this->stack.find(var_identifier) != this->stack.end();
	}

	// returns true if a variable exists in function dictionary
	bool has_function(tok function_identifier)
	{
		return this->functions.find(function_identifier) != this->functions.end();
	}

	// runs a ael-function identified by the first token passing
	// all phrase as argument
	void run_cmd(phrase &ph)
	{
		if(has_function(ph[0]))
		{
			this->functions[ph[0]](*this, ph);
		}
		else
		{
			std::string function_name = this->get_value(AEL_DEFAULT_FUNCTION);
			if (this->has_function(function_name))
			{
				// try: avoid create a new list
				std::vector<tok> _new_phrase;
				_new_phrase.push_back(this->get_value(AEL_DEFAULT_FUNCTION));
				for (int i=0; i < ph.size(); i++)
				{
					_new_phrase.push_back(ph[i]);
				}

				// here i'm trying not call "run_cmd" recursively to avoid
				// any type of infinity loop
				this->functions[_new_phrase[0]](*this, _new_phrase);
			}
			else
			{
				_ael_error(ph, "can't find that function");
			}
		}
	}

	// receives a complete script and runs each command splitted by "." char
	void interprets(phrase &ph)
	{
		phrase p;
		for(int i=0; i < ph.size(); i++)
		{
			if(ph[i] == ".")
			{
				this->run_cmd(p);
				p.clear();
			}
			else
			{
				p.push_back(ph[i]);
			}
		}
	}
};

typedef void(*aelfunction)(aelinterpreter&, phrase&);

// trace arg1 arg2 argN.
// no new lines
void _ael_trace(aelinterpreter &ael, phrase &ph)
{
	for(int i=1; i < ph.size(); i++)
	{
		std::cout << ael.get_value(ph[i]) << " ";
	}
}

//[doc] creates a var in interpreter dictionary
void _ael_set(aelinterpreter &ael, phrase &ph)
{
	if(ph.size() == 3)
	{
		ael.dictionary[ph[1]] = ael.get_value(ph[2]);
	}
	else if(ph.size() == 2)
	{
		ael.dictionary[ph[1]] = "";
	}
	else
	{
		// fixme: better error formatting
		std::cerr << "Error: set takes 1 or 2 arguments (" << ph.size()-1 << " given)" << std::endl;
		return;
	}
}

// interprets a string like a script (reflexion)
void _ael_run(aelinterpreter &ael, phrase &ph)
{
	if (_ael_error_invalid_number_arguments_exact(ph, 1))
	{
		return;
	}
	std::string content = ael.get_value(ph[1]).c_str();
	phrase p;
	ael.to_tokens(content.c_str(), p);
	ael.interprets(p);
}

// print each argument one after other (without white spaces)
void _ael_print(aelinterpreter &ael, phrase &ph)
{
	for(int i=1; i < ph.size(); i++)
	{
		std::cout << ael.get_value(ph[i]);
	}
}

// deletes one or more variables of interpreter dictionary
void _ael_del(aelinterpreter &ael, phrase &ph)
{
	if(_ael_error_invalid_number_arguments_minimum(ph, 1))
	{
		return;
	}
	for(int i=1;i < ph.size(); i++)
	{
		ael.dictionary.erase(ph[i]);
	}
}

// trace in terminal all functions and variables in scope
void _ael_dump(aelinterpreter &ael, phrase &ph)
{
	std::cout << "Functions:\n";
	std::unordered_map<tok, void(*)(aelinterpreter&, phrase&)>::iterator it = ael.functions.begin();
	while(it != ael.functions.end())
	{
		std::cout << "\t" << it->first << std::endl;
		it++;
	}
	std::cout << "\nVariables:\n";
	aeldict::iterator itv = ael.dictionary.begin();
	while(itv != ael.dictionary.end())
	{
		std::cout << "\t" << itv->first << std::endl;
		itv++;
	}
}

void _ael_add_number(aelinterpreter &ael, phrase &ph)
{
	if (ph.size() != 3 && ph.size() != 4)
	{
		// fixme: format better this error string
		std::cerr << "Error: wrong numbers of arguments" << std::endl;
		return;
	}
	double value1 = atof(ael.get_value(ph[1]).c_str());
	double value2 = atof(ael.get_value(ph[2]).c_str());
	if (ph.size() == 4)
	{
		ael.dictionary[ph[3]] = to_string(value1 + value2);
	}
	else
	{
		ael.dictionary[ph[1]] = to_string(value1 + value2);
	}
}

// * a b <dest> (if dest is not provided the result is stored in a)
void _ael_mult_number(aelinterpreter &ael, phrase &ph)
{
	if (ph.size() != 3 && ph.size() != 4)
	{
		// fixme: format better this error string
		std::cerr << "Error: wrong numbers of arguments" << std::endl;
		return;
	}
	double value1 = atof(ael.get_value(ph[1]).c_str());
	double value2 = atof(ael.get_value(ph[2]).c_str());
	if (ph.size() == 4)
	{
		ael.dictionary[ph[3]] = to_string(value1 * value2);
	}
	else
	{
		ael.dictionary[ph[1]] = to_string(value1 * value2);
	}
}

// / a b <dest> (if dest is not provided the result is stored in a)
void _ael_div_number(aelinterpreter &ael, phrase &ph)
{
	if (ph.size() != 3 && ph.size() != 4)
	{
		// fixme: format better this error string
		std::cerr << "Error: wrong numbers of arguments" << std::endl;
		return;
	}
	double value1 = atof(ael.get_value(ph[1]).c_str());
	double value2 = atof(ael.get_value(ph[2]).c_str());
	if (ph.size() == 4)
	{
		ael.dictionary[ph[3]] = to_string(value1 / value2);
	}
	else
	{
		ael.dictionary[ph[1]] = to_string(value1 / value2);
	}
}

// read and then interprets a Ael script file
void _ael_import(aelinterpreter &ael, phrase &ph)
{
	if (_ael_error_invalid_number_arguments_exact(ph, 1))
	{
		return;
	}
	std::string content = get_file_content(ael.get_value(ph[1]).c_str());
	phrase p;
	ael.to_tokens(content.c_str(), p);
	ael.interprets(p);
}

// does nothing
void _ael_nop(aelinterpreter &ael, phrase &ph)
{
}

// loop NUMBER CODE.
void _ael_loop(aelinterpreter &ael, phrase &ph)
{
	if (_ael_error_invalid_number_arguments_exact(ph, 2))
	{
		return;
	}
	int loop = atoi(ael.get_value(ph[1]).c_str());
	phrase p;
	ael.to_tokens(ael.get_value(ph[2]).c_str(), p);
	while (loop > 0)
	{
		loop--;
		ael.interprets(p);
	}
}

void _ael_if(aelinterpreter &ael, phrase &ph)
{
	// formats:
	// 1.
	// 		if <var1> <op> <var2> <code>.
	// 2.
	// 		if <var1> <op> <var2> <code> <else> <code>.
	// TODO: make elifs
	if (ph.size() != 5 && ph.size() != 7)
	{
		std::cerr << "ERROR" << std::endl;
		return;
	}
	std::string var1 = ael.get_value(ph[1]);
	std::string op = ph[2];
	std::string var2 = ael.get_value(ph[3]);

	bool pass_in_if = false;
	if (op == "==")
	{
		pass_in_if = var1 == var2;
	}
	else if (op == "!=")
	{
		pass_in_if = var1 != var2;
	}
	else if (op == ">")
	{
		double v1 = atof(var1.c_str());
		double v2 = atof(var2.c_str());
		pass_in_if = v1 > v2;
	}
	else if (op == ">=")
	{
		double v1 = atof(var1.c_str());
		double v2 = atof(var2.c_str());
		pass_in_if = v1 >= v2;
	}
	else if (op == "<")
	{
		double v1 = atof(var1.c_str());
		double v2 = atof(var2.c_str());
		pass_in_if = v1 < v2;
	}
	else if (op == "<=")
	{
		double v1 = atof(var1.c_str());
		double v2 = atof(var2.c_str());
		pass_in_if = v1 <= v2;
	}
	else
	{
		_ael_error(ph, "wrong operator");
		return;
	}

	if (pass_in_if)
	{
		phrase p;
		ael.to_tokens(ael.get_value(ph[4]).c_str(), p);
		ael.interprets(p);
	}
	else
	{
		// has else
		if (ph.size() == 7)
		{
			phrase p;
			ael.to_tokens(ael.get_value(ph[6]).c_str(), p);
			ael.interprets(p);
		}
	}
}

const bool AEL_VERBOSE = true;
void ael_trace(std::string m)
{
	if (AEL_VERBOSE)
	{
		std::cout << m << std::endl;
	}
}

// this function is called by 'instance' of stack
void _ael_stack_variable(aelinterpreter &i, phrase &ph)
{
	// stack loop <code>. (acessible by `@item`)
	// stack clear.
	// stack push <value:get_value>.
	// stack pop.
	// stack length <destvar>.
	// stack set <index> <value>.
	// stack get <index> <destvar>.
	// stack erase <index>.
	if(_ael_error_invalid_number_arguments_minimum(ph, 1))
	{
		return;
	}
	tok stack_name = ph[0];
	tok command = ph[1];
	if (command == "push")
	{
		if (_ael_error_invalid_number_arguments_exact(ph, 2))
		{
			return;
		}
		i.stack[stack_name].push_back(i.get_value(ph[2]));
	}
	else if (command == "pop")
	{
		if (_ael_error_invalid_number_arguments_exact(ph, 1))
		{
			return;
		}
		if (i.stack[stack_name].size() > 0)
		{
			i.stack[stack_name].pop_back();
		}
	}
	else if (command == "clear")
	{
		if (_ael_error_invalid_number_arguments_exact(ph, 1))
		{
			return;
		}
		i.stack.erase(stack_name);
	}
	else if (command == "loop")
	{
		if (_ael_error_invalid_number_arguments_exact(ph, 2))
		{
			return;
		}
		phrase p;
		i.to_tokens(i.get_value(ph[2]).c_str(), p);
		for (int stack_i=0; stack_i < i.stack[stack_name].size(); stack_i++)
		{
			i.dictionary["@item"] = i.stack[stack_name][stack_i];
			i.interprets(p);
		}
	}
	else if (command == "length")
	{
		if (_ael_error_invalid_number_arguments_exact(ph, 2))
		{
			return;
		}
		i.dictionary[ph[2]] = to_string(i.stack[stack_name].size());
	}
	else if (command == "set")
	{
		if (_ael_error_invalid_number_arguments_exact(ph, 3))
		{
			return;
		}
		int index = atoi(i.get_value(ph[2]).c_str());
		if (index < i.stack[stack_name].size())
		{
			i.stack[stack_name][index] = i.get_value(ph[3]);
		}
		else
		{
			_ael_error(ph, "index out of stack bounds");
		}
	}
	else if (command == "get")
	{
		if (_ael_error_invalid_number_arguments_exact(ph, 3))
		{
			return;
		}
		int index = atoi(i.get_value(ph[2]).c_str());
		if (index < i.stack[stack_name].size())
		{
			i.dictionary[ph[3]] = i.stack[stack_name][index];
		}
		else
		{
			_ael_error(ph, "index out of stack bounds");
		}
	}
	else if (command == "erase")
	{
		if (_ael_error_invalid_number_arguments_exact(ph, 2))
		{
			return;
		}
		int index = atoi(i.get_value(ph[2]).c_str());
		if (index < i.stack[stack_name].size())
		{
			i.stack[stack_name].erase(i.stack[stack_name].begin() + index);
		}
		else
		{
			_ael_error(ph, "index out of stack bounds");
		}
	}
	else
	{
		_ael_error(ph, "invalid command");
	}
}

void _ael_stack(aelinterpreter &i, phrase &ph)
{
	if (_ael_error_invalid_number_arguments_exact(ph, 1))
	{
		return;
	}
	i.functions[ph[1]] = _ael_stack_variable;

}

// load FILE_NAME DEST_VAR FILE_NAME2 DEST_VAR2 ...
// load a file and put the content inside 'DEST_VAR'
void _ael_load(aelinterpreter &ael, phrase &ph)
{
	if (ph.size() < 3)
	{
		_ael_error(ph, "_ael_load: Invalid number of arguments less than 2");
		return;
	}
	if (((ph.size() - 1) % 2) != 0)
	{
		_ael_error(ph, "_ael_load: Invalid number of arguments. not divisible by 2");
		return;
	}
	for (int i=1; i < ph.size() - 1; i+=2)
	{
		ael.dictionary[ph[i+1]] = get_file_content(ph[i].c_str());
	}
}

// function that receives a phrase and run it
void async_interprets(aelinterpreter ael, phrase ph) {
	ael.interprets(ph);
}
// async <CODE>
void _ael_async(aelinterpreter& ael, phrase& ph) {
	if (_ael_error_invalid_number_arguments_exact(ph, 1)) {
		return;
	}
	phrase code;
	ael.to_tokens(ael.get_value(ph[1]).c_str(), code);
	std::thread(async_interprets, ael, code).detach();
}

//[doc] load main functions
void load_main_ael_functions(aelinterpreter &i)
{
	i.functions["trace"] = _ael_trace;
	i.functions["set"] = _ael_set;
	i.functions["run"] = _ael_run;
	i.functions["del"] = _ael_del;
	i.functions["dump"] = _ael_dump;
	i.functions["print"] = _ael_print;
	i.functions["import"] = _ael_import;
	i.functions["#"] = _ael_nop;
	i.functions["+"] = _ael_add_number;
	i.functions["*"] = _ael_mult_number;
	i.functions["/"] = _ael_div_number;
	i.functions["loop"] = _ael_loop;
	i.functions["if"] = _ael_if;
	i.functions["stack"] = _ael_stack;
	i.functions["load"] = _ael_load;
	i.functions["async"] = _ael_async;
	i.dictionary["__ael_version"] = AEL_VERSION;
	i.dictionary[AEL_DEFAULT_FUNCTION] = "run";
}

#endif