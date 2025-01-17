#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "tables.h"
using namespace std;

// -------------------- ConstantsTableElement --------------------

ConstantsTableElement::ConstantsTableElement(int id, constantType type, string utf8string)
{
	Id = id;
	Type = type;
	Utf8String = new string(utf8string);
}


ConstantsTableElement::ConstantsTableElement(int id, constantType type, int number, int firstRef, int secondRef)
{
	Id = id;
	Type = type;
	Number = number;
	FirstRef = firstRef;
	SecondRef = secondRef;
}

// -------------------- ConstantsTable --------------------

ConstantsTable::ConstantsTable()
{
	items[maxId] = new ConstantsTableElement(maxId, UTF8, "Code");
	maxId++;
}

int ConstantsTable::findOrAddConstant(constantType type, string utf8string)
{
	int res = findConstant(type, &utf8string);
	if (res == -1)
	{
		res = maxId++;
		items[res] = new ConstantsTableElement(res, type, utf8string);
	}
	return res;
}

int ConstantsTable::findOrAddConstant(constantType type, int number, int firstRef, int secondRef)
{
	int res = findConstant(type, NULL, number, firstRef, secondRef);
	if (res == -1)
	{
		res = maxId++;
		items[res] = new ConstantsTableElement(res, type, number, firstRef, secondRef);
	}
	return res;
}
