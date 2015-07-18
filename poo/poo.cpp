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
	virtual void call(aelinterpreter& ael, phrase& ph)
	{
		std::cout << "nop" << std::endl;
	}
};

std::vector<aelobject*> AEL_POO_OBJECTS;

void ael_poo_object_call(aelinterpreter& ael, phrase& ph)
{
	// [fixme] > aqui estou pegando so o primeiro caractere
	// isso vai ocasionar um erro quando forem criados 0xff objetos
	// mas por enquanto eh suficiente

	// getting the object index
	uint index = ael.dictionary["__" + ph[0]][0];
	aelobject* obj = AEL_POO_OBJECTS[index];
	// calling the call method
	obj->call(ael, ph);
}

// [doc] creates an index to an object that alredy has a name
// you can pass here an inherit instance of aelobject
void store_object(aelinterpreter& ael, aelobject* obj)
{
	// saving the object in list
	obj->index = AEL_POO_OBJECTS.size();
	AEL_POO_OBJECTS.push_back(obj);
	// creating a dictionary with your unique index:
	std::string index = "";
	// [fixme] > quando o indice for maior que 0xff vai
	// causar um overflow, mas 255 objetos, por enquanto,
	// eh um numero interessante
	index += (char)obj->index;
	ael.dictionary["__" + obj->name] = index;
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