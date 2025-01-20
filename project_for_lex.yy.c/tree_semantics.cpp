#include "tables.h"
#include <algorithm>
#include <string>
// ---------- Keyword_argument_list_node ----------
void Keyword_argument_list_node::fillFieldRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement)
{
	Keyword_argument_node* arg = First;
	while (arg != NULL)
	{
		arg->expression->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		arg = arg->Next;
	}
}

void Keyword_argument_list_node::fillMethodRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement, bool isInstanceMethod)
{
	Keyword_argument_node* arg = First;
	while (arg != NULL)
	{
		arg->expression->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInstanceMethod);
		arg = arg->Next;
	}
}

void Keyword_argument_list_node::fillLiterals(ConstantsTable* constantTable)
{
	Keyword_argument_node* arg = First;
	while (arg != NULL)
	{
		arg->expression->fillLiterals(constantTable);
		arg = arg->Next;
	}
}

// ---------- Expression_list_node ----------

void Expression_list_node::fillFieldRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement)
{
	Expression_node* cur = First;
	while (cur != NULL)
	{
		cur->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		cur = cur->Next;
	}
}

void Expression_list_node::fillMethodRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement, bool isInInstanceMethod)
{
	Expression_node* cur = First;
	while (cur != NULL)
	{
		cur->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
		cur = cur->Next;
	}
}

void Expression_list_node::fillLiterals(ConstantsTable* constantTable)
{
	Expression_node* cur = First;
	while (cur != NULL)
	{
		cur->fillLiterals(constantTable);
		cur = cur->Next;
	}
}

// ---------- Init_declarator_list_node ----------
void Init_declarator_list_node::fillFieldRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement)
{
	Init_declarator_node* declarator = First;
	while (declarator != NULL)
	{
		declarator->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		declarator = declarator->Next;
	}
}

void Init_declarator_list_node::fillMethodRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement, bool isInInstanceMethod)
{
	Init_declarator_node* declarator = First;
	while (declarator != NULL)
	{
		declarator->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
		declarator = declarator->Next;
	}
}

void Init_declarator_list_node::fillLiterals(ConstantsTable* constantTable)
{
	Init_declarator_node* declarator = First;
	while (declarator != NULL)
	{
		declarator->fillLiterals(constantTable);
		declarator = declarator->Next;
	}
}

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

