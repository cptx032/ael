#include "ael.h"

//[doc] loads and dynamic library (c++) from a file path
void _ael_load(aelinterpreter& ael, phrase& _phrase);
#ifdef _WIN32
void _ael_load(aelinterpreter& ael, phrase& _phrase)
{
	HINSTANCE dllHandle = NULL;
	dllHandle = LoadLibrary(ael.get_value(_phrase[1]).c_str());
	if(!dllHandle)
	{
		// finding the library in Windows's AEL_PATH
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
			tok _possible_path = join(_paths[i], ael.get_value(_phrase[1]));
			dllHandle = LoadLibrary(_possible_path.c_str());
			if(dllHandle)
				break;
		}
		// not found in absolute path and not found in AEL_PATH
		if(!dllHandle)
		{
			std::cout << "Library '" << ael.get_value(_phrase[1]) << "' not found!" << std::endl;
			return;
		}
	}
	aelfunction lib_init;
	lib_init = (aelfunction)GetProcAddress(dllHandle, "ael_lib_init");
	if(!lib_init)
	{
		std::cerr << "Library not formated!" << std::endl;
		return;
	}
	lib_init(ael, _phrase);
}
#endif
#ifdef __unix
//[doc] unix implementation
void _ael_load(aelinterpreter& ael, phrase& ph)
{
	void* dllHandle = NULL;
	dllHandle = dlopen(ael.get_value(ph[1]).c_str(), RTLD_LAZY);
	if(!dllHandle)
	{
		// finding the library in Unix's AEL_PATH
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
			tok _possible_path = join(_paths[i], ael.get_value(ph[1]));
			dllHandle = dlopen(_possible_path.c_str(), RTLD_LAZY);
			if(dllHandle)
				break;
		}
		// not found in absolute path and not found in AEL_PATH
		if(!dllHandle)
		{
			std::cerr << "Library '" << ael.get_value(ph[1]) << "' not found!" << std::endl;
			return;
		}
	}
	aelfunction lib_init;
	lib_init = (aelfunction)dlsym(dllHandle, "ael_lib_init");
	if(!lib_init)
	{
		std::cerr << "Library not formated!" << std::endl;
		return;
	}
	lib_init(ael, ph);
	// dlclose(dllhandle);
}
#endif

#ifdef __unix
//[doc] returns to a var the process id of program
void _ael_getpid(aelinterpreter& ael, phrase& ph)
{
	if(ph.size()!=2)
	{
		std::cerr << "Error: " << ph[0] << " takes exactly 1 argument (" << ph.size()-1 << " given)" << std::endl;
		return;
	}
	int pid = (int)getpid();
	ael.dictionary[ph[1]] = to_string(pid);
}

//[doc] returns to a var the process id of the parent program
void _ael_getppid(aelinterpreter& ael, phrase& ph)
{
	if(ph.size()!=2)
	{
		std::cerr << "Error: " << ph[0] << " takes exactly 1 argument (" << ph.size()-1 << " given)" << std::endl;
		return;
	}
	int pid = (int)getppid();
	ael.dictionary[ph[1]] = to_string(pid);
}
#endif