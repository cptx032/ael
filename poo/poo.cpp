#include "ael.h"
#ifndef aelpoo
#define aelpoo

#ifdef __cplusplus
extern "C" {
#endif

class aelobject
{
public:
	uint index;
	std::string name;
};

std::vector<aelobject*> AEL_POO_OBJECTS;

void ael_poo_object_call(aelinterpreter& ael, phrase& ph)
{
	std::cout << "object name: " << ph[0] << " call: " << ph[1] << std::endl;
}

// [syntax] obj name.
void ael_create_object(aelinterpreter& ael, phrase& ph)
{
	if(ph.size() != 2)
	{
		std::cerr << "Error: " << ph[0] << " takes exactly 1 argument (" << ph.size()-1 << " given)" << std::endl;
		return;
	}
	else
	{
		// saving the object in list
		aelobject* aelo = new aelobject();
		aelo->name = ph[1];
		aelo->index = AEL_POO_OBJECTS.size();
		AEL_POO_OBJECTS.push_back(aelo);
		// creating a dictionary with your unique index:
		std::string index = "";
		index += (char)aelo->index;
		ael.dictionary["__" + aelo->name] = index;
		// creating a function with the name of object
		ael.functions[aelo->name] = ael_poo_object_call;
	}
}

void ael_lib_init(aelinterpreter& ael, phrase& ph)
{
	ael.dictionary["poo"] = "loaded";
	ael.functions["obj"] = ael_create_object;
}

#ifdef __cplusplus
}
#endif

#endif