#include <fstream>
#include "tables.h"
#include "code_generation_helpers.h"
#include <iostream>
#include <string>

using namespace std;
vector<char> Expression_list_node::generateCode(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;
	Expression_node* cur = First;
	while (cur != NULL) {
		vector<char> expression = cur->generateCode(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, expression.data(), expression.size());
		cur = cur->Next;
	}
	return res;
}
