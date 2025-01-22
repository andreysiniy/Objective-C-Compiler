#include <fstream>
#include "tables.h"
#include "code_generation_helpers.h"
#include <iostream>
#include <string>

using namespace std;
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
