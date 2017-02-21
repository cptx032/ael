/*
Author: Willie Lawrence - cptx032@gmail.com
*/

#ifndef AEL_H
#define AEL_H

#include <iostream>
#include <stdlib.h> // exit
#include <sstream> // cast double to string
#include <vector>
#include <unordered_map> // hash_map
#include <fstream> // reading file
#ifdef __unix
	#include <unistd.h> // getpid
	#include <dlfcn.h>
#endif
#ifdef _WIN32
	#include <windows.h>
#endif

#define AEL_VERSION "1.0.0.2"

// [doc] When a function is not found the function saved
// in this var is runned
#define AEL_DEFAULT_FUNCTION "__ael_default_function"

typedef std::string tok; // token
typedef std::vector<tok> phrase;
typedef unsigned int uint;
typedef std::unordered_map<tok, tok> aeldict;
typedef aeldict(*aelnamefunction)(void);

bool is_a_blank_char(char _c)
{
	if(_c==' '||_c=='\t'||_c=='\n'||_c=='\r')
		return true;
	return false;
}

// [doc] Split a string by _c char argument
phrase split(tok _entry, char _c)
{
	phrase _result;
	_result.push_back("");
	for(uint i=0;i<_entry.length();i++)
	{
		if(_entry[i] == _c)
			_result.push_back("");
		else
			_result[_result.size()-1] += _entry[i];
	}
	return _result;
}

//[doc] replaces a char by another in _entry
tok replace(tok _entry, char _c, char _r)
{
	tok _result = "";
	for(uint i=0;i<_entry.length();i++)
	{
		if(_entry[i] == _c)
			_result += _r;
		else
			_result += _entry[i];
	}
	return _result;
}

// [doc] Join two file paths
tok join(tok _dirname, tok _filename)
{
	_dirname = replace(_dirname, '\\', '/');
	_filename = replace(_filename, '\\', '/');
	if(_dirname.back() != '/')
		_dirname += '/';
	if(_filename[0] == '/')
		_filename = _filename.substr(1,_filename.length()-1); // cut the first char
	return _dirname + _filename; // path/file.ext
}

//[doc] transforms float to string
tok to_string(float _v)
{
	std::ostringstream str_concatenated;
	str_concatenated << _v;
	return str_concatenated.str();
}

//[doc] returns the content of a text file
char* get_file_content(const char* file_path)
{
	std::string contents;
	std::string line;
	std::ifstream script_file(file_path);

	if(!script_file.is_open())
	{
		char* _ael_path = NULL;
		_ael_path = getenv((char*)"AEL_PATH");
		if(_ael_path == NULL)
		{
			std::cerr << "Environment variable not found...please reinstall Ael" << std::endl;
			abort();
		}
		phrase _paths = split(_ael_path, ';');
		for(uint i=0;i<_paths.size();i++)
		{
			tok _possible_path = join(_paths[i], file_path);
			script_file.open(_possible_path.c_str(), std::ifstream::in);
			if(script_file.is_open())
				break;
		}
		if(!script_file.is_open())
		{
			std::cerr << "Script '" << file_path << "' not found!" << std::endl;
			return (char*)"";
		}
	}
	
	if(script_file.is_open())
	{
		while (getline(script_file, line))
		{
			if(line[0] != '#') // [fixme] if begins with space it will be ignored
			{
				contents += line + '\n';
			}
		}
		script_file.close();
	}
	else
	{
		std::cerr << "'" << file_path << "'" << " file not found" << std::endl;
		exit(-1);
	}
	return (char*)contents.c_str();
}

class aelinterpreter
{
public:
	// dicionário chave: string valor: string > guarda as variáveis
	aeldict dictionary;
	typedef void(*aelfunction)(aelinterpreter&,phrase&);
	// dicionário cuja chave é o nome da função e o valor é um ponteiro para função
	std::unordered_map<tok, aelfunction> functions;

	aelinterpreter(){}
	//[doc] traduz um script para tokens e adiciona
	// numa frase
	void to_tokens(const char* _s, phrase& ph)
	{
		std::string script = _s;
		ph.push_back("");
		uint block_counter = 0;
		for(uint i=0;i<script.size();i++)
		{
			char actual_char = script[i];
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
	tok get_value(tok key)
	{
		// if key starts with '"'
		if(key[0] == '{')
		{
			return key.substr(1, key.size()-2);
		}

		aeldict::const_iterator got = this->dictionary.find(key);
		if(got != this->dictionary.end())
		{
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
	//[doc] executa um único comando identificado pelo
	// primeiro token passando como argumento todos
	// os outros tokens (inclusive o primeiro argumento)
	void run_cmd(phrase& ph)
	{
		std::unordered_map<tok, aelfunction>::const_iterator got = this->functions.find(ph[0]);
		if(got != this->functions.end())
		{
			this->functions[ph[0]](*this, ph);
		}
		else
		{
			// [fixme] > evitar criar uma nova lista
			std::vector<tok> _new_phrase;
			_new_phrase.push_back(this->get_value(AEL_DEFAULT_FUNCTION));
			for (uint i=0;i<ph.size();i++)
			{
				_new_phrase.push_back(ph[i]);
			}
			// [fixme] > chamar introspectivamente a funcao run_cmd
			this->functions[_new_phrase[0]](*this, _new_phrase);
			// std::cerr << "Can't find '" << ph[0] << "' function" << std::endl;
		}
	}
	//[doc] recebe um script completo e executa cada
	// comando que estiver separado por ';'
	void interprets(phrase& ph)
	{
		phrase p;
		for(uint i=0;i<ph.size();i++)
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
typedef void(*aelfunction)(aelinterpreter&,phrase&);

//[doc] print each argument in a new line
void _ael_trace(aelinterpreter&ael,phrase&ph)
{
	for(uint i=1;i<ph.size();i++)
	{
		std::cout << ael.get_value(ph[i]) << " ";
	}
	std::cout << std::endl;
}

//[doc] creates a var in interpreter dictionary
void _ael_set(aelinterpreter&ael, phrase&ph)
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
		std::cerr << "Error: set takes 1 or 2 arguments (" << ph.size()-1 << " given)" << std::endl;
		return;
	}
}

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

//[doc] interprets a string like a script (reflexion)
void _ael_run(aelinterpreter&ael, phrase&ph)
{
	if(ph.size()!=2)
	{
		std::cerr << "Error: " << ph[0] << " takes exactly 1 argument (" << ph.size()-1 << " given)" << std::endl;
		return;
	}
	std::string content = ael.get_value(ph[1]).c_str();
	phrase p;
	ael.to_tokens(content.c_str(), p);
	ael.interprets(p);
}

//[doc] quit of application
void _ael_exit(aelinterpreter& ael, phrase& ph)
{
	if(ph.size() > 1)
		exit(atoi(ph[1].c_str()));
	else
		exit(0);
}

//[doc] print each argument one after other (without
// white spaces)
void _ael_print(aelinterpreter&ael, phrase&ph)
{
	for(uint i=1;i<ph.size();i++)
	{
		std::cout << ael.get_value(ph[i]);
	}
}

//[doc] pass first argument to operational system
void _ael_sys(aelinterpreter& ael, phrase& ph)
{
	if(ph.size()<2)
	{
		std::cerr << "Error: " << ph[0] << " takes 1 or more arguments (0 given)" << std::endl;
		return;
	}
	std::string _cmd_final = "";
	for(uint i=1;i<ph.size();i++)
	{
		_cmd_final += ael.get_value(ph[i]) + " ";
	}
	system(ael.get_value(_cmd_final).c_str());
}

//[doc] deletes a variable of interpreter dictionary
void _ael_del(aelinterpreter& ael, phrase& ph)
{
	if(ph.size()==1)
	{
		std::cerr << "Error: " << ph[0] << " takes 1 or more arguments (0 given)" << std::endl;
		return;
	}
	for(uint i=1;i<ph.size();i++)
	{
		ael.dictionary.erase(ph[i]);
	}
}

//[doc] list all variables
void _ael_ls(aelinterpreter& ael, phrase& ph)
{
	aeldict::iterator it = ael.dictionary.begin();
	while(it != ael.dictionary.end())
	{
		if(it->first[0] != '_')
			std::cout << "'" << it->first << "' ";
		else
		{
			if(it->first.length() >= 2 && it->first[1] != '_')
			{
				std::cout << "'" << it->first << "' ";
			}
		}
		it++;
	}
	std::cout << std::endl;
}

//[doc] list all functions
void _ael_lf(aelinterpreter& ael, phrase& ph)
{
	std::unordered_map<tok, void(*)(aelinterpreter&, phrase&)>::iterator it = ael.functions.begin();
	while(it != ael.functions.end())
	{
		std::cout << "'" << it->first << "' ";
		it++;
	}
	std::cout << std::endl;
}

void _ael_add_number(aelinterpreter& ael, phrase& ph)
{
	if (ph.size()!=3 && ph.size() != 4) {
		// TODO: format better this error string
		std::cout << "Error: wrong numbers of arguments" << std::endl;
		return;
	}
	double value1 = atof(ael.get_value(ph[1]).c_str());
	double value2 = atof(ael.get_value(ph[2]).c_str());
	if (ph.size() == 4) {
		ael.dictionary[ph[3]] = to_string(value1 + value2);
	}
	else {
		ael.dictionary[ph[1]] = to_string(value1 + value2);
	}
}

void _ael_if_gt(aelinterpreter& ael, phrase& ph) {
	if (ph.size() != 4) {
		std::cout << "Error" << std::endl;
		return;
	}
	double value1 = atof(ael.get_value(ph[1]).c_str());
	double value2 = atof(ael.get_value(ph[2]).c_str());
	if (value1 > value2) {
		phrase p;
		ael.to_tokens(ph[3].c_str(), p);
		ael.interprets(p);
	}
}

void _ael_if_lt(aelinterpreter& ael, phrase& ph) {
	if (ph.size() != 4) {
		std::cout << "Error" << std::endl;
		return;
	}
	double value1 = atof(ael.get_value(ph[1]).c_str());
	double value2 = atof(ael.get_value(ph[2]).c_str());
	if (value1 < value2) {
		phrase p;
		ael.to_tokens(ph[3].c_str(), p);
		ael.interprets(p);
	}
}

void _ael_if_eq(aelinterpreter& ael, phrase& ph) {
	if (ph.size() != 4) {
		std::cout << "Error" << std::endl;
		return;
	}
	double value1 = atof(ael.get_value(ph[1]).c_str());
	double value2 = atof(ael.get_value(ph[2]).c_str());
	if (value1 == value2) {
		phrase p;
		ael.to_tokens(ph[3].c_str(), p);
		ael.interprets(p);
	}
}

void _ael_if_neq(aelinterpreter& ael, phrase& ph) {
	if (ph.size() != 4) {
		std::cout << "Error" << std::endl;
		return;
	}
	double value1 = atof(ael.get_value(ph[1]).c_str());
	double value2 = atof(ael.get_value(ph[2]).c_str());
	if (value1 != value2) {
		phrase p;
		ael.to_tokens(ph[3].c_str(), p);
		ael.interprets(p);
	}
}

// [doc] get environment variable
void _ael_getenv(aelinterpreter& ael, phrase& ph)
{
	if(ph.size()!=3)
	{
		std::cerr << "Error: " << ph[0] << " takes exactly 2 arguments (" << ph.size()-1 << " given)" << std::endl;
		return;
	}
	char* env_value = NULL;
	env_value = getenv(ael.get_value(ph[1]).c_str());
	if(env_value)
		ael.dictionary[ph[2]] = env_value;
}

//[doc] read and then interprets a Ael script file
void _ael_import(aelinterpreter& ael, phrase& ph)
{
	if(ph.size()!=2)
	{
		std::cerr << "Error: " << ph[0] << " takes exactly 1 argument (" << ph.size()-1 << " given)" << std::endl;
		return;
	}
	std::string content = get_file_content(ael.get_value(ph[1]).c_str());
	phrase p;
	ael.to_tokens(content.c_str(), p);
	ael.interprets(p);
}

// [doc] does nothing
void _ael_nop(aelinterpreter& ael, phrase& ph)
{
	// [fixme] > do nothing here...
}
int randint(int max, int min)
{
	srand(static_cast<unsigned int>(time(0)));
	return (min <= max) ?
		((rand() % ((max + 1) - min)) + min) :
		((rand() % ((min + 1) - max)) + max);
}

void _ael_randint(aelinterpreter& ael, phrase& ph) {
	if (ph.size() != 4) {
		std::cout << "Error" << std::endl;
		return;
	}
	long value1 = atol(ael.get_value(ph[1]).c_str());
	long value2 = atol(ael.get_value(ph[2]).c_str());
	ael.dictionary[ph[3]] = to_string(randint(value1, value2));
}

void _ael_loop(aelinterpreter& ael, phrase& ph) {
	int loop = atoi(ael.get_value(ph[1]).c_str());
	phrase p;
	ael.to_tokens(ael.get_value(ph[2]).c_str(), p);
	while (loop > 0) {
		loop--;
		ael.interprets(p);
	}
}

void _ael_if(aelinterpreter& ael, phrase& ph) {
	// formats:
	// 1.
	// 		if <var1> <op> <var2> <code>.
	// 2.
	// 		if <var1> <op> <var2> <code> <else> <code>.
	// TODO: make elifs
	if (ph.size() != 5 and ph.size() != 7) {
		std::cerr << "ERROR" << std::endl;
		return;
	}
	std::string var1 = ael.get_value(ph[1]);
	std::string op = ael.get_value(ph[2]);
	std::string var2 = ael.get_value(ph[3]);

	bool pass_in_if = false;
	if (op == "==") {
		pass_in_if = var1 == var2;
	}
	else if (op == "!=") {
		pass_in_if = var1 != var2;
	}
	else if (op == ">") {
		double v1 = atof(var1.c_str());
		double v2 = atof(var2.c_str());
		pass_in_if = v1 > v2;
	}
	else if (op == ">=") {
		double v1 = atof(var1.c_str());
		double v2 = atof(var2.c_str());
		pass_in_if = v1 >= v2;
	}
	else if (op == "<") {
		double v1 = atof(var1.c_str());
		double v2 = atof(var2.c_str());
		pass_in_if = v1 < v2;
	}
	else if (op == "<=") {
		double v1 = atof(var1.c_str());
		double v2 = atof(var2.c_str());
		pass_in_if = v1 <= v2;
	}

	if (pass_in_if) {
		phrase p;
		ael.to_tokens(ael.get_value(ph[4]).c_str(), p);
		ael.interprets(p);
	}
	else {
		if (ph.size() == 7) { // has else
			phrase p;
			ael.to_tokens(ael.get_value(ph[6]).c_str(), p);
			ael.interprets(p);
		}
	}
}

//[doc] load main functions
void load_main_ael_functions(aelinterpreter&i)
{
	i.functions["trace"] = _ael_trace;
	i.functions["set"] = _ael_set;
	i.functions["input"] = _ael_input;
	i.functions["run"] = _ael_run;
	i.functions["exit"] = _ael_exit;
	i.functions["sys"] = _ael_sys;
	i.functions["del"] = _ael_del;
	i.functions["ls"] = _ael_ls;
	i.functions["lf"] = _ael_lf;
	i.functions["print"] = _ael_print;
	i.functions["getenv"] = _ael_getenv;
	i.functions["import"] = _ael_import;
	i.functions["#"] = _ael_nop;
	i.functions["+"] = _ael_add_number;
	i.functions[">"] = _ael_if_gt;
	i.functions["<"] = _ael_if_lt;
	i.functions["=="] = _ael_if_eq;
	i.functions["!="] = _ael_if_neq;
	i.functions["randint"] = _ael_randint;
	i.functions["loop"] = _ael_loop;
	i.functions["if"] = _ael_if;

	i.dictionary["__ael_version"] = AEL_VERSION;

	i.dictionary["\\n"] = "\n";
	i.dictionary["\\t"] = "\t";
	i.dictionary["\\b"] = "\b";
	i.dictionary["\\r"] = "\r";
	i.dictionary["\\a"] = "\a";
	i.dictionary["__ael_default_function"] = "run";
}

#endif
