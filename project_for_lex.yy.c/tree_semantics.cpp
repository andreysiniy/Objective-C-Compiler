#include "tables.h"
#include <algorithm>
#include <string>
// ---------- Init_declarator_node ---------- 
void Init_declarator_node::fillFieldRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement)
{
	if (expression != NULL)
		expression->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	if (ArraySize != NULL)
		ArraySize->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	if (InitializerList != NULL)
		InitializerList->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
}

void Init_declarator_node::fillMethodRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement, bool isInInstanceMethod)
{
	if (expression != NULL)
		expression->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	if (ArraySize != NULL)
		ArraySize->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	if (InitializerList != NULL)
		InitializerList->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);

}

void Init_declarator_node::fillLiterals(ConstantsTable* constantTable)
{
	if (expression != NULL)
		expression->fillLiterals(constantTable);
	if (ArraySize != NULL)
		ArraySize->fillLiterals(constantTable);
	if (InitializerList != NULL)
		InitializerList->fillLiterals(constantTable);
}



// ---------- Numeric_constant ----------
void Numeric_constant_node::fillLiterals(ConstantsTable* constantTable)
{
	if (Int > 32767 || Int < -32768) {
		constantTable->findOrAddConstant(Integer, Int);
	}
}

// ---------- literal ----------
void Literal_node::fillLiterals(ConstantsTable* constantTable)
{
	if (type == STRING_CONSTANT_TYPE || type == NSSTRING_CONSTANT_TYPE) {
		int utfIndex = constantTable->findOrAddConstant(UTF8, value);
		constantTable->findOrAddConstant(String, 0, utfIndex);
	}
}

