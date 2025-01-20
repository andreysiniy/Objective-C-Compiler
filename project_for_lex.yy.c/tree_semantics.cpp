#include "tables.h"
#include <algorithm>
#include <string>
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

