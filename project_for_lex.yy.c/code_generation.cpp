#include <fstream>
#include "tables.h"
#include "code_generation_helpers.h"
#include <iostream>
#include <string>

using namespace std;
vector<char> Expression_node::generateCodeForEqual(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	if (Left->type != SUPER_EXPRESSION_TYPE && Left->type != SELF_EXPRESSION_TYPE && Right->type != SUPER_EXPRESSION_TYPE && Right->type != SELF_EXPRESSION_TYPE) {
		vector<char> leftOperand = Left->generateCode(isInsideClassMethod, constantsTable); //левый операнд	
		CodeGenerationHelpers::appendArrayToByteVector(&res, leftOperand.data(), leftOperand.size());

		vector<char> rightOperand = Right->generateCode(isInsideClassMethod, constantsTable); //правый операнд
		CodeGenerationHelpers::appendArrayToByteVector(&res, rightOperand.data(), rightOperand.size());

		vector<char> trueBytes = CodeGenerationCommands::iconstBipushSipush(1); //Ветка, если равны
		vector<char> falseBytes = CodeGenerationCommands::iconstBipushSipush(0); //Ветка, если не равны
		vector<char> gotoBytes = CodeGenerationCommands::goto_(falseBytes.size()); //Безусловный переход в случае положительной ветки

		int offset = trueBytes.size() + gotoBytes.size(); //Смещение, с которого начинается альтернативная ветка
		vector<char> ifBytes; //Условный переход
		if (Left->DataType->isPrimitive() && Right->DataType->isPrimitive())
			ifBytes = CodeGenerationCommands::if_icmp(CodeGenerationCommands::NE, offset);
		else
			ifBytes = CodeGenerationCommands::if_acmp(CodeGenerationCommands::NE, offset);

		// Формирование кода
		CodeGenerationHelpers::appendArrayToByteVector(&res, ifBytes.data(), ifBytes.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, trueBytes.data(), trueBytes.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, gotoBytes.data(), gotoBytes.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, falseBytes.data(), falseBytes.size());
	}
	else {
		if (Left->type == Right->type)
			res = CodeGenerationCommands::iconstBipushSipush(1);
		else
			res = CodeGenerationCommands::iconstBipushSipush(0);
	}

	return res;
}

vector<char> Expression_node::generateCodeForNotEqual(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	if (Left->type != SUPER_EXPRESSION_TYPE && Left->type != SELF_EXPRESSION_TYPE && Right->type != SUPER_EXPRESSION_TYPE && Right->type != SELF_EXPRESSION_TYPE) {
		vector<char> leftOperand = Left->generateCode(isInsideClassMethod, constantsTable); //левый операнд	
		CodeGenerationHelpers::appendArrayToByteVector(&res, leftOperand.data(), leftOperand.size());

		vector<char> rightOperand = Right->generateCode(isInsideClassMethod, constantsTable); //правый операнд
		CodeGenerationHelpers::appendArrayToByteVector(&res, rightOperand.data(), rightOperand.size());

		vector<char> trueBytes = CodeGenerationCommands::iconstBipushSipush(1); //Ветка, если не равны
		vector<char> falseBytes = CodeGenerationCommands::iconstBipushSipush(0); //Ветка, если равны
		vector<char> gotoBytes = CodeGenerationCommands::goto_(falseBytes.size()); //Безусловный переход в случае положительной ветки

		int offset = trueBytes.size() + gotoBytes.size(); //Смещение, с которого начинается альтернативная ветка
		vector<char> ifBytes; //Условный переход
		if (Left->DataType->isPrimitive() && Right->DataType->isPrimitive())
			ifBytes = CodeGenerationCommands::if_icmp(CodeGenerationCommands::EQ, offset);
		else
			ifBytes = CodeGenerationCommands::if_acmp(CodeGenerationCommands::EQ, offset);

		// Формирование кода
		CodeGenerationHelpers::appendArrayToByteVector(&res, ifBytes.data(), ifBytes.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, trueBytes.data(), trueBytes.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, gotoBytes.data(), gotoBytes.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, falseBytes.data(), falseBytes.size());
	}
	else {
		if (Left->type != Right->type)
			res = CodeGenerationCommands::iconstBipushSipush(1);
		else
			res = CodeGenerationCommands::iconstBipushSipush(0);
	}

	return res;
}

vector<char> Expression_node::generateCodeForGreater(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	vector<char> leftOperand = Left->generateCode(isInsideClassMethod, constantsTable); //левый операнд	
	CodeGenerationHelpers::appendArrayToByteVector(&res, leftOperand.data(), leftOperand.size());

	vector<char> rightOperand = Right->generateCode(isInsideClassMethod, constantsTable); //правый операнд
	CodeGenerationHelpers::appendArrayToByteVector(&res, rightOperand.data(), rightOperand.size());

	vector<char> trueBytes = CodeGenerationCommands::iconstBipushSipush(1); //Ветка, если больше
	vector<char> falseBytes = CodeGenerationCommands::iconstBipushSipush(0); //Ветка, если меньше или равно
	vector<char> gotoBytes = CodeGenerationCommands::goto_(falseBytes.size()); //Безусловный переход в случае положительной ветки

	int offset = trueBytes.size() + gotoBytes.size(); //Смещение, с которого начинается альтернативная ветка
	vector<char> ifBytes = CodeGenerationCommands::if_icmp(CodeGenerationCommands::LE, offset); //Условный переход

	// Формирование кода
	CodeGenerationHelpers::appendArrayToByteVector(&res, ifBytes.data(), ifBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, trueBytes.data(), trueBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, gotoBytes.data(), gotoBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, falseBytes.data(), falseBytes.size());

	return res;
}

vector<char> Expression_node::generateCodeForLess(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	vector<char> leftOperand = Left->generateCode(isInsideClassMethod, constantsTable); //левый операнд	
	CodeGenerationHelpers::appendArrayToByteVector(&res, leftOperand.data(), leftOperand.size());

	vector<char> rightOperand = Right->generateCode(isInsideClassMethod, constantsTable); //правый операнд
	CodeGenerationHelpers::appendArrayToByteVector(&res, rightOperand.data(), rightOperand.size());

	vector<char> trueBytes = CodeGenerationCommands::iconstBipushSipush(1); //Ветка, если меньше
	vector<char> falseBytes = CodeGenerationCommands::iconstBipushSipush(0); //Ветка, если больше или равно
	vector<char> gotoBytes = CodeGenerationCommands::goto_(falseBytes.size()); //Безусловный переход в случае положительной ветки

	int offset = trueBytes.size() + gotoBytes.size(); //Смещение, с которого начинается альтернативная ветка
	vector<char> ifBytes = CodeGenerationCommands::if_icmp(CodeGenerationCommands::GE, offset); //Условный переход

	// Формирование кода
	CodeGenerationHelpers::appendArrayToByteVector(&res, ifBytes.data(), ifBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, trueBytes.data(), trueBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, gotoBytes.data(), gotoBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, falseBytes.data(), falseBytes.size());

	return res;
}

vector<char> Expression_node::generateCodeForLessEqual(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	vector<char> leftOperand = Left->generateCode(isInsideClassMethod, constantsTable); //левый операнд	
	CodeGenerationHelpers::appendArrayToByteVector(&res, leftOperand.data(), leftOperand.size());

	vector<char> rightOperand = Right->generateCode(isInsideClassMethod, constantsTable); //правый операнд
	CodeGenerationHelpers::appendArrayToByteVector(&res, rightOperand.data(), rightOperand.size());

	vector<char> trueBytes = CodeGenerationCommands::iconstBipushSipush(1); //Ветка, если меньше или равно
	vector<char> falseBytes = CodeGenerationCommands::iconstBipushSipush(0); //Ветка, если больше
	vector<char> gotoBytes = CodeGenerationCommands::goto_(falseBytes.size()); //Безусловный переход в случае положительной ветки

	int offset = trueBytes.size() + gotoBytes.size(); //Смещение, с которого начинается альтернативная ветка
	vector<char> ifBytes = CodeGenerationCommands::if_icmp(CodeGenerationCommands::GT, offset); //Условный переход

	// Формирование кода
	CodeGenerationHelpers::appendArrayToByteVector(&res, ifBytes.data(), ifBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, trueBytes.data(), trueBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, gotoBytes.data(), gotoBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, falseBytes.data(), falseBytes.size());

	return res;
}

vector<char> Expression_node::generateCodeForGreaterEqual(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	vector<char> leftOperand = Left->generateCode(isInsideClassMethod, constantsTable); //левый операнд	
	CodeGenerationHelpers::appendArrayToByteVector(&res, leftOperand.data(), leftOperand.size());

	vector<char> rightOperand = Right->generateCode(isInsideClassMethod, constantsTable); //правый операнд
	CodeGenerationHelpers::appendArrayToByteVector(&res, rightOperand.data(), rightOperand.size());

	vector<char> trueBytes = CodeGenerationCommands::iconstBipushSipush(1); //Ветка, если больше или равно
	vector<char> falseBytes = CodeGenerationCommands::iconstBipushSipush(0); //Ветка, если меньше
	vector<char> gotoBytes = CodeGenerationCommands::goto_(falseBytes.size()); //Безусловный переход в случае положительной ветки

	int offset = trueBytes.size() + gotoBytes.size(); //Смещение, с которого начинается альтернативная ветка
	vector<char> ifBytes = CodeGenerationCommands::if_icmp(CodeGenerationCommands::LT, offset); //Условный переход

	// Формирование кода
	CodeGenerationHelpers::appendArrayToByteVector(&res, ifBytes.data(), ifBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, trueBytes.data(), trueBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, gotoBytes.data(), gotoBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, falseBytes.data(), falseBytes.size());

	return res;
}
vector<char> Expression_node::generateCodeForIdentifier(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;
	if (LocalVariable != NULL) { //Локальная переменная
		vector<char> bytes;
		if (DataType->ClassName != "" || DataType->ArrSize != NULL)
			bytes = CodeGenerationCommands::aload(LocalVariable->Id - isInsideClassMethod); //Команда
		else
			bytes = CodeGenerationCommands::iload(LocalVariable->Id - isInsideClassMethod); //Команда
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
	else if (Field != NULL) {
		vector<char> obj = CodeGenerationCommands::aload(0); //Объект
		CodeGenerationHelpers::appendArrayToByteVector(&res, obj.data(), obj.size());

		if (constantsTable->items.count(Constant) == 0) {
			string msg = "Class doesn't have constant " + to_string(Constant);
			throw new std::exception(msg.c_str());
		}

		vector<char> field = CodeGenerationCommands::getfield(Constant); //Поле
		CodeGenerationHelpers::appendArrayToByteVector(&res, field.data(), field.size());
	}
	else {
		string msg = "Unknown identifier '" + string(name);
		throw new std::exception(msg.c_str());
	}
	return res;
}

vector<char> Expression_node::generateCodeForSelf()
{
	vector<char> res = CodeGenerationCommands::aload(LocalVariable->Id);
	return res;
}

vector<char> Expression_node::generateCodeForSuper()
{
	vector<char> res = CodeGenerationCommands::aload(LocalVariable->Id);
	return res;
}
vector<char> Expression_node::generateCodeForArrow(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	if (Field != NULL) {
		vector<char> obj = Left->generateCode(isInsideClassMethod, constantsTable); //Объект
		CodeGenerationHelpers::appendArrayToByteVector(&res, obj.data(), obj.size());

		if (constantsTable->items.count(Constant) == 0) {
			string msg = "Class doesn't have constant " + to_string(Constant);
			throw new std::exception(msg.c_str());
		}
		else if (constantsTable->items[Constant]->Type != FieldRef) {
			string msg = "Constant " + to_string(Constant) + " is not fieldRef";
			throw new std::exception(msg.c_str());
		}

		vector<char> field = CodeGenerationCommands::getfield(Constant); //Поле
		CodeGenerationHelpers::appendArrayToByteVector(&res, field.data(), field.size());
	}
	else {
		string msg = "Unknown identifier '" + string(name);
		throw new std::exception(msg.c_str());
	}

	return res;
}

vector<char> Expression_node::generateCodeForArrayElementAccess(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	vector<char> arr = Left->generateCode(isInsideClassMethod, constantsTable); //ссылка на Массив
	CodeGenerationHelpers::appendArrayToByteVector(&res, arr.data(), arr.size());
	
	vector<char> index = Right->generateCode(isInsideClassMethod, constantsTable); //индекс
	CodeGenerationHelpers::appendArrayToByteVector(&res, index.data(), index.size());

	if (DataType->DataType == INT_TYPE) {
		vector<char> bytes = CodeGenerationCommands::iaload(); //Команда
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
	else if (DataType->DataType == CHAR_TYPE) {
		vector<char> bytes = CodeGenerationCommands::caload(); //Команда
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
	else if (DataType->DataType == CLASS_NAME_TYPE || DataType->DataType == ID_TYPE) {
		vector<char> bytes = CodeGenerationCommands::aaload(); //Команда
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}


	return res;
}

vector<char> Expression_node::generateCodeForArrayAssignment(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	vector<char> arr = Left->generateCode(isInsideClassMethod, constantsTable); //ссылка на Массив
	CodeGenerationHelpers::appendArrayToByteVector(&res, arr.data(), arr.size());

	vector<char> index = Child->generateCode(isInsideClassMethod, constantsTable); //индекс
	CodeGenerationHelpers::appendArrayToByteVector(&res, index.data(), index.size());

	vector<char> value = Right->generateCode(isInsideClassMethod, constantsTable); //значение
	CodeGenerationHelpers::appendArrayToByteVector(&res, value.data(), value.size());

	if (DataType->DataType == INT_TYPE) {
		vector<char> bytes = CodeGenerationCommands::iastore(); //Команда
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
	else if (DataType->DataType == CHAR_TYPE) {
		vector<char> bytes = CodeGenerationCommands::castore(); //Команда
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
	else if (DataType->DataType == CLASS_NAME_TYPE || DataType->DataType == ID_TYPE) {
		vector<char> bytes = CodeGenerationCommands::aastore(); //Команда
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}

	return res;
}

vector<char> Expression_node::generateCodeForMemberAccessAssignment(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	if (Field != NULL) {
		vector<char> obj = Left->generateCode(isInsideClassMethod, constantsTable); //Ссылка на Объект
		CodeGenerationHelpers::appendArrayToByteVector(&res, obj.data(), obj.size());

		vector<char> value = Right->generateCode(isInsideClassMethod, constantsTable); //Значение
		CodeGenerationHelpers::appendArrayToByteVector(&res, value.data(), value.size());

		if (constantsTable->items.count(Constant) == 0) {
			string msg = "Class doesn't have constant " + to_string(Constant);
			throw new std::exception(msg.c_str());
		}
		else if (constantsTable->items[Constant]->Type != FieldRef) {
			string msg = "Constant " + to_string(Constant) + " is not fieldRef";
			throw new std::exception(msg.c_str());
		}

		vector<char> bytes = CodeGenerationCommands::putfield(Constant); //Команда
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}

	return res;
}

vector<char> Expression_node::generateCodeForCharCast(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;


	if (Right->DataType->isCastableTo(DataType)) {
		vector<char> value = Right->generateCode(isInsideClassMethod, constantsTable); //Значение
		CodeGenerationHelpers::appendArrayToByteVector(&res, value.data(), value.size());
	}
	else {
		string msg = "Cannot cast " + Right->DataType->ClassName + " to " + DataType->ClassName;
		throw new std::exception(msg.c_str());
	}

	return res;
}

vector<char> Expression_node::generateCodeForIntCast(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	if (Right->DataType->isCastableTo(DataType)) {
		vector<char> value = Right->generateCode(isInsideClassMethod, constantsTable); //Значение
		CodeGenerationHelpers::appendArrayToByteVector(&res, value.data(), value.size());
	}
	else {
		string msg = "Cannot cast " + Right->DataType->ClassName + " to " + DataType->ClassName;
		throw new std::exception(msg.c_str());
	}

	return res;
}

vector<char> Expression_node::generateCodeForClassCast(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	if (Right->DataType->isCastableTo(DataType)) {
		vector<char> value = Right->generateCode(isInsideClassMethod, constantsTable); //Значение
		CodeGenerationHelpers::appendArrayToByteVector(&res, value.data(), value.size());
	}
	else {
		string msg = "Cannot cast " + Right->DataType->ClassName + " to " + DataType->ClassName;
		throw new std::exception(msg.c_str());
	}

	return res;
}
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
