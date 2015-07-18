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
	std::ofstream __file_output;
	std::ifstream __file_input;
	void writeto(std::string filepath)
	{
		__file_output.open(filepath);
	}
	void readfrom(std::string filepath)
	{
		__file_input.open(filepath);
	}
public:
	void call(aelinterpreter& ael, phrase& ph)
	{
		if(ph[1] == "writeto")
		{
			this->writeto( ael.get_value(ph[2]) );
		}
		else if(ph[1] == "readfrom")
		{
			this->readfrom( ael.get_value(ph[2]) );
		}
		else if(ph[1] == "close")
		{
			this->__file_output.close();
			this->__file_input.close();
		}
		else if(ph[1] == "<<")
		{
			if(this->__file_output.is_open())
			{
				this->__file_output << ael.get_value(ph[2]);
			}
		}
		else if(ph[1] == ">>")
		{
			if(this->__file_input.is_open())
			{
				std::string content;
				std::string line;
				while ( getline (this->__file_input,line) )
				{
					content += line + '\n';
				}
				ael.dictionary[ph[2]] = content;
			}
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