#include "ael.h"
#ifndef aelpoo
#define aelpoo

#ifdef __cplusplus
extern "C" {
#endif

class aelobject
{
public:
	std::string name;
	virtual void call(aelinterpreter& ael, phrase& ph)
	{
		std::cout << "nop" << std::endl;
	}
};

std::unordered_map<tok, aelobject*> AEL_POO_OBJECTS;

void ael_poo_object_call(aelinterpreter& ael, phrase& ph)
{
	aelobject* obj = AEL_POO_OBJECTS[ph[0]];
	obj->call(ael, ph);
}

// [doc] creates an index to an object that alredy has a name
// you can pass here an inherit instance of aelobject
void store_object(aelinterpreter& ael, aelobject* obj)
{
	AEL_POO_OBJECTS[obj->name] = obj;
	// creating a function with the name of object
	ael.functions[obj->name] = ael_poo_object_call;
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
		store_object(ael, aelo);
	}
}
#ifdef AEL_LIB
void ael_lib_init(aelinterpreter& ael, phrase& ph)
{
	ael.dictionary["poo"] = "loaded";
	ael.functions["obj"] = ael_create_object;
}
#endif

#ifdef __cplusplus
}
#endif

#endif
