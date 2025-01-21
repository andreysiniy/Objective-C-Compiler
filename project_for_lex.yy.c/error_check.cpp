#include "tables.h"
#include <string>

void Expression_node::checkLvalueError()
{
	if (type == ASSIGNMENT_EXPRESSION_TYPE) {
		bool isIncorrectLvalue = Left->type != IDENTIFIER_EXPRESSION_TYPE;
		isIncorrectLvalue = isIncorrectLvalue && Left->type != SELF_EXPRESSION_TYPE;
		isIncorrectLvalue = isIncorrectLvalue && Left->type != ASSIGNMENT_EXPRESSION_TYPE;
		isIncorrectLvalue = isIncorrectLvalue && Left->type != ARROW_EXPRESSION_TYPE;
		isIncorrectLvalue = isIncorrectLvalue && Left->type != ARRAY_ELEMENT_ACCESS_EXPRESSION_TYPE;

		if (isIncorrectLvalue) {
			string msg = "Incorrect lvalue. Lvalue have '" + Left->getTypeName() + "' type in line: " + to_string(line);
			throw new std::exception(msg.c_str());
		}
	}
}
