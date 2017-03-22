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

#define AEL_VERSION "1.0.0.2"

// [doc] When a function is not found the function saved
// in this var is runned
#define AEL_DEFAULT_FUNCTION "__ael_default_function"

typedef std::string tok; // token
typedef std::vector<tok> phrase;
typedef unsigned int uint;
typedef std::unordered_map<tok, tok> aeldict;
typedef aeldict(*aelnamefunction)(void);

typedef std::vector<tok> string_stack;

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
tok to_string(double _v)
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
	// ael lists
	std::unordered_map<tok, string_stack> stack;

	aelinterpreter(){}
	//[doc] traduz um script para tokens e adiciona
	// numa frase
	void to_tokens(const char* _s, phrase& ph)
	{
		std::string script = _s;
		ph.push_back("");
		int block_counter = 0;
		char actual_char;
		for(uint i=0; i < script.size(); i++)
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
	tok get_value(tok key)
	{
		// if key starts with '"'
		if(key[0] == '{')
		{
			return key.substr(1, key.size()-2);
		}

		if(this->has_var(key))
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

	bool has_var(tok var_identifier) {
		return this->dictionary.find(var_identifier) != this->dictionary.end();
	}

	bool has_stack(tok var_identifier) {
		return this->stack.find(var_identifier) != this->stack.end();
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
	// comando que estiver separado por '.'
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

// * a b <dest> (if dest is not provided the result is stored in a)
void _ael_mult_number(aelinterpreter& ael, phrase& ph)
{
	if (ph.size()!=3 && ph.size() != 4) {
		// TODO: format better this error string
		std::cout << "Error: wrong numbers of arguments" << std::endl;
		return;
	}
	double value1 = atof(ael.get_value(ph[1]).c_str());
	double value2 = atof(ael.get_value(ph[2]).c_str());
	if (ph.size() == 4) {
		ael.dictionary[ph[3]] = to_string(value1 * value2);
	}
	else {
		ael.dictionary[ph[1]] = to_string(value1 * value2);
	}
}

// / a b <dest> (if dest is not provided the result is stored in a)
void _ael_div_number(aelinterpreter& ael, phrase& ph)
{
	if (ph.size()!=3 && ph.size() != 4) {
		// TODO: format better this error string
		std::cout << "Error: wrong numbers of arguments" << std::endl;
		return;
	}
	double value1 = atof(ael.get_value(ph[1]).c_str());
	double value2 = atof(ael.get_value(ph[2]).c_str());
	if (ph.size() == 4) {
		ael.dictionary[ph[3]] = to_string(value1 / value2);
	}
	else {
		ael.dictionary[ph[1]] = to_string(value1 / value2);
	}
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
	// does nothing
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
	if (ph.size() != 5 && ph.size() != 7) {
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
	else {
		std::cerr << "ERROR: ael_if: Wrong operator: " << op << std::endl;
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

void _ael_log(phrase& ph) {
	for (int i=0; i < ph.size(); i++) {
		std::cout <<  "{" << ph[i] << "} ";
	}
	std::cout << std::endl;
}

bool _ael_error_invalid_number_arguments(phrase& ph, int expected) {
	if (ph.size()-1 != expected) {
		std::cerr << "Error: " << ph[0] << " takes exactly " << expected << " arguments (" << ph.size()-1 << " given)" << std::endl;
		std::cout << "Args:\n\t";
		_ael_log(ph);
		return true;
	}
	return false;
}

void _ael_error(std::string msg) {
	std::cerr << "ERROR: " << msg << std::endl;
}

const bool AEL_VERBOSE = true;
void ael_trace(std::string m) {
	if (AEL_VERBOSE) {
		std::cout << m << std::endl;
	}
}

void _ael_stack(aelinterpreter& i, phrase& ph) {
	// stack loop <var> <code>. (acessible by `stack_value`)
	// stack destroy <var>.
	// stack push <var> <value:get_value>.
	// stack pop <var>.
	// stack length <var> <destvar>.
	// stack set <var> <index> <value>.
	// stack get <var> <index> <destvar>.
	// stack erase <var> <index>.
	tok command = ph[1];
	if (command == "push") {
		if (_ael_error_invalid_number_arguments(ph, 3)) {
			return;
		}
		i.stack[ph[2]].push_back(i.get_value(ph[3]));
	}
	else if (command == "pop") {
		if (_ael_error_invalid_number_arguments(ph, 2)) {
			return;
		}
		i.stack[ph[2]].pop_back();
	}
	else if (command == "destroy") {
		if (_ael_error_invalid_number_arguments(ph, 2)) {
			return;
		}
		i.stack.erase(ph[2]);
	}
	else if (command == "loop") {
		if (_ael_error_invalid_number_arguments(ph, 3)) {
			return;
		}
		phrase p;
		i.to_tokens(i.get_value(ph[3]).c_str(), p);
		for (int stack_i=0; stack_i < i.stack[ph[2]].size(); stack_i++) {
			i.dictionary["STACK_VALUE"] = i.stack[ph[2]][stack_i];
			i.interprets(p);
		}
	}
	else if (command == "length") {
		if (_ael_error_invalid_number_arguments(ph, 3)) {
			return;
		}
		i.dictionary[ph[3]] = to_string(i.stack[ph[2]].size());
	}
	else if (command == "set") {
		if (_ael_error_invalid_number_arguments(ph, 4)) {
			return;
		}
		int index = atoi(i.get_value(ph[3]).c_str());
		if (index < i.stack[ph[2]].size()) {
			i.stack[ph[2]][index] = i.get_value(ph[4]);
		} else {
			_ael_error("ael set: index out of stack bounds");
		}
	}
	else if (command == "get") {
		if (_ael_error_invalid_number_arguments(ph, 4)) {
			return;
		}
		int index = atoi(i.get_value(ph[3]).c_str());
		if (index < i.stack[ph[2]].size()) {
			i.dictionary[ph[4]] = i.stack[ph[2]][index];
		} else {
			_ael_error("ael get: index out of stack bounds");
		}
	}
	else if (command == "erase") {
		if (_ael_error_invalid_number_arguments(ph, 3)) {
			return;
		}
		int index = atoi(i.get_value(ph[3]).c_str());
		if (index < i.stack[ph[2]].size()) {
			i.stack[ph[2]].erase(i.stack[ph[2]].begin() + index);
		} else {
			_ael_error("ael stack get: index out of stack bounds");
		}
	}
	else {
		_ael_error("ael stack: invalid command");
	}
}

// strcat STR1 STR2 (DEST:optional).
// if DEST is not provided the result is stored in STR1
void _ael_strcat(aelinterpreter& ael, phrase& ph) {
	if (ph.size() != 3 && ph.size() != 4) {
		_ael_error("strcat: wrong numbers of arguments");
		return;
	}
	tok dest = ph.size() == 4 ? ph[3] : ph[1];
	ael.dictionary[dest] = ael.get_value(ph[1]) + ael.get_value(ph[2]);
}

//[doc] load main functions
void load_main_ael_functions(aelinterpreter&i)
{
	i.functions["trace"] = _ael_trace;
	i.functions["set"] = _ael_set;
	i.functions["input"] = _ael_input;
	i.functions["run"] = _ael_run;
	i.functions["exit"] = _ael_exit;
	i.functions["del"] = _ael_del;
	i.functions["ls"] = _ael_ls;
	i.functions["lf"] = _ael_lf;
	i.functions["print"] = _ael_print;
	i.functions["import"] = _ael_import;
	i.functions["#"] = _ael_nop;
	i.functions["+"] = _ael_add_number;
	i.functions["*"] = _ael_mult_number;
	i.functions["/"] = _ael_div_number;
	i.functions["loop"] = _ael_loop;
	i.functions["if"] = _ael_if;
	i.functions["stack"] = _ael_stack;
	i.functions["strcat"] = _ael_strcat;

	i.dictionary["__ael_version"] = AEL_VERSION;

	i.dictionary["\\n"] = "\n";
	i.dictionary["\\t"] = "\t";
	i.dictionary["\\b"] = "\b";
	i.dictionary["\\r"] = "\r";
	i.dictionary["\\a"] = "\a";
	i.dictionary[AEL_DEFAULT_FUNCTION] = "run";
}

#endif
