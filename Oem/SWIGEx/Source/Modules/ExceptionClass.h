#ifndef _EXCEPTION_CLASS_H_
#define _EXCEPTION_CLASS_H_

#include <string>
#include <list>
using namespace std;

class ExceptionClass
{
public:
	ExceptionClass()
	{
		this->parent = NULL;
	}

	void addChild(ExceptionClass* e)
	{
		e->parent = this;
		this->derivedClasses.push_back(e);
	}

	string name;
	string parentName;

	ExceptionClass* parent;
	list<ExceptionClass*> derivedClasses;
};

#endif
