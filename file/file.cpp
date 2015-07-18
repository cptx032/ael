#include "poo.cpp"
#include <fstream>

#ifndef aelfile
#define aelfile

#ifdef __cplusplus
extern "C" {
#endif

class aelfilestream : public aelobject
{
private:
	std::ofstream __file;
	void open(std::string filepath)
	{
		__file.open(filepath);
	}
public:
	void call(aelinterpreter& ael, phrase& ph)
	{
		if(ph[1] == "open")
		{
			this->open( ael.get_value(ph[2]) );
		}
		else if(ph[1] == "close")
		{
			this->__file.close();
		}
		else if(ph[1] == "<<")
		{
			if(this->__file.is_open())
				this->__file << ph[2];
		}
	}
};

void ael_create_file(aelinterpreter& ael, phrase& ph)
{
	aelfilestream* _ael_file = new aelfilestream();
	_ael_file->name = ph[1];
	store_object(ael, _ael_file);
}

void ael_lib_init(aelinterpreter& ael, phrase& ph)
{
	ael.dictionary["file"] = "loaded";
	ael.functions["file"] = ael_create_file;
}


#ifdef __cplusplus
}
#endif

#endif