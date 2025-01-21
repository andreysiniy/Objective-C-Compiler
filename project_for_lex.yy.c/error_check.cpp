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

void Expression_node::checkDeclarated(vector<string> localNames, ClassesTableElement* classElement, vector<string> keywordNames, vector<string> parameterNames)
{
	switch (type)
	{
	case IDENTIFIER_EXPRESSION_TYPE: {
		string varName = name;
		bool isDeclarated = localNames.end() != find(localNames.begin(), localNames.end(), varName);
		isDeclarated = isDeclarated || keywordNames.end() != find(keywordNames.begin(), keywordNames.end(), varName);
		isDeclarated = isDeclarated || parameterNames.end() != find(parameterNames.begin(), parameterNames.end(), varName);
		isDeclarated = isDeclarated || classElement->isContainsField(varName);
		if (!isDeclarated) {
			string msg = "Variable '" + string(varName) + "' is undeclarated in line: " + to_string(line);
			throw new std::exception(msg.c_str());
		}
	}
		 break;
	case MESSAGE_EXPRESSION_EXPRESSION_TYPE: {
		Receiver->checkDeclarated(localNames, classElement, keywordNames, parameterNames);
		Arguments->checkDeclarated(localNames, classElement, keywordNames, parameterNames);
	}
		break;
	case UMINUS_EXPRESSION_TYPE:
	case UPLUS_EXPRESSION_TYPE:
	{
		Right->checkDeclarated(localNames, classElement, keywordNames, parameterNames);
	}
	break;
	case PLUS_EXPRESSION_TYPE:
	case MINUS_EXPRESSION_TYPE:
	case MUL_EXPRESSION_TYPE:
	case DIV_EXPRESSION_TYPE:
	case EQUAL_EXPRESSION_TYPE:
	case NOT_EQUAL_EXPRESSION_TYPE:
	case GREATER_EXPRESSION_TYPE:
	case LESS_EXPRESSION_TYPE:
	case LESS_EQUAL_EXPRESSION_TYPE:
	case GREATER_EQUAL_EXPRESSION_TYPE:
	case ASSIGNMENT_EXPRESSION_TYPE: {
		Left->checkDeclarated(localNames, classElement, keywordNames, parameterNames);
		Right->checkDeclarated(localNames, classElement, keywordNames, parameterNames);
	}
		break;
	case ARROW_EXPRESSION_TYPE: {
		Left->checkDeclarated(localNames, classElement, keywordNames, parameterNames);
	}
		break;
	case ARRAY_ELEMENT_ACCESS_EXPRESSION_TYPE: {
		Left->checkDeclarated(localNames, classElement, keywordNames, parameterNames);
		Right->checkDeclarated(localNames, classElement, keywordNames, parameterNames);
	}
		break;
	default:
		break;
	}
		
}
