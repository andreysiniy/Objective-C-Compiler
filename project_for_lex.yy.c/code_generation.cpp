#include <fstream>
#include "tables.h"
#include "code_generation_helpers.h"
#include <iostream>
#include <string>

using namespace std;
vector<char> Statement_node::generateCodeForIfStatement(bool isInsideClassMethod, ConstantsTable* constantsTable, LocalVariablesTable* locals)
{
	vector<char> res;
	
	If_statement_node* if_stmt = (If_statement_node*)this;

	vector<char> condition = if_stmt->Condition->generateCode(isInsideClassMethod, constantsTable); //Условие
	CodeGenerationHelpers::appendArrayToByteVector(&res, condition.data(), condition.size());

	vector<char> trueBytes;
	vector<char> falseBytes;
	vector<char> gotoBytes;

	if (if_stmt->TrueBranch != NULL)
		trueBytes = if_stmt->TrueBranch->generateCode(isInsideClassMethod, constantsTable, locals);

	if (if_stmt->FalseBranch != NULL) {
		falseBytes = if_stmt->FalseBranch->generateCode(isInsideClassMethod, constantsTable, locals);
		gotoBytes = CodeGenerationCommands::goto_(falseBytes.size());
	}

	int offset = trueBytes.size() + gotoBytes.size();
	vector<char> ifBytes;
	if (if_stmt->Condition->DataType->isPrimitive()) {
		ifBytes = CodeGenerationCommands::if_(CodeGenerationCommands::EQ, offset);
	}
	else {
		vector<char> aconst_null = CodeGenerationCommands::aconst_null(); //Загрузка null для сравнения объекта
		CodeGenerationHelpers::appendArrayToByteVector(&res, aconst_null.data(), aconst_null.size());
		ifBytes = CodeGenerationCommands::if_acmp(CodeGenerationCommands::EQ, offset);
	}


	// Формирование кода
	CodeGenerationHelpers::appendArrayToByteVector(&res, ifBytes.data(), ifBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, trueBytes.data(), trueBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, gotoBytes.data(), gotoBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, falseBytes.data(), falseBytes.size());

	return res;
}
vector<char> Statement_node::generateCodeForWhileStatement(bool isInsideClassMethod, ConstantsTable* constantsTable, LocalVariablesTable* locals)
{
	vector<char> res;

	While_statement_node* while_stmt = (While_statement_node*)this;

	vector<char> bodyBytes = while_stmt->LoopBody->generateCode(isInsideClassMethod, constantsTable, locals);
	vector<char> gotoBytes = CodeGenerationCommands::goto_(bodyBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, gotoBytes.data(), gotoBytes.size());
	CodeGenerationHelpers::appendArrayToByteVector(&res, bodyBytes.data(), bodyBytes.size());

	vector<char> conditionBytes = while_stmt->LoopCondition->generateCode(isInsideClassMethod, constantsTable); //Условие
	CodeGenerationHelpers::appendArrayToByteVector(&res, conditionBytes.data(), conditionBytes.size());

	int offset = bodyBytes.size() + conditionBytes.size();
	offset = -offset;
	vector<char> ifBytes;
	if (while_stmt->LoopCondition->DataType->isPrimitive()) {
		ifBytes = CodeGenerationCommands::if_(CodeGenerationCommands::NE, offset);
	}
	else {
		vector<char> aconst_null = CodeGenerationCommands::aconst_null(); //Загрузка null для сравнения объекта
		CodeGenerationHelpers::appendArrayToByteVector(&res, aconst_null.data(), aconst_null.size());
		ifBytes = CodeGenerationCommands::if_acmp(CodeGenerationCommands::NE, offset);
	}

	CodeGenerationHelpers::appendArrayToByteVector(&res, ifBytes.data(), ifBytes.size());
	

	return res;
}

vector<char> Statement_node::generateCodeForDoWhileStatement(bool isInsideClassMethod, ConstantsTable* constantsTable, LocalVariablesTable* locals)
{
	vector<char> res;

	Do_while_statement_node* do_while_stmt = (Do_while_statement_node*)this;

	vector<char> bodyBytes = do_while_stmt->LoopBody->generateCode(isInsideClassMethod, constantsTable, locals);
	CodeGenerationHelpers::appendArrayToByteVector(&res, bodyBytes.data(), bodyBytes.size());

	vector<char> conditionBytes = do_while_stmt->LoopCondition->generateCode(isInsideClassMethod, constantsTable); //Условие
	CodeGenerationHelpers::appendArrayToByteVector(&res, conditionBytes.data(), conditionBytes.size());

	int offset = bodyBytes.size() + conditionBytes.size();
	offset = -offset;
	vector<char> ifBytes;
	if (do_while_stmt->LoopCondition->DataType->isPrimitive()) {
		ifBytes = CodeGenerationCommands::if_(CodeGenerationCommands::NE, offset);
	}
	else {
		vector<char> aconst_null = CodeGenerationCommands::aconst_null(); //Загрузка null для сравнения объекта
		CodeGenerationHelpers::appendArrayToByteVector(&res, aconst_null.data(), aconst_null.size());
		ifBytes = CodeGenerationCommands::if_acmp(CodeGenerationCommands::NE, offset);
	}

	CodeGenerationHelpers::appendArrayToByteVector(&res, ifBytes.data(), ifBytes.size());

	return res;
}
// ---------- Expression ----------
vector<char> Expression_node::generateCodeForPlus(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	vector<char> leftOperand = Left->generateCode(isInsideClassMethod, constantsTable); //1-ое слагаемое
	CodeGenerationHelpers::appendArrayToByteVector(&res, leftOperand.data(), leftOperand.size());

	vector<char> rightOperand = Right->generateCode(isInsideClassMethod, constantsTable); //2-ое слагаемое
	CodeGenerationHelpers::appendArrayToByteVector(&res, rightOperand.data(), rightOperand.size());

	vector<char> bytes = CodeGenerationCommands::iadd(); //Команда
	CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());

	return res;
}

vector<char> Expression_node::generateCodeForMinus(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	vector<char> leftOperand = Left->generateCode(isInsideClassMethod, constantsTable); //Уменьшаемое
	CodeGenerationHelpers::appendArrayToByteVector(&res, leftOperand.data(), leftOperand.size());

	vector<char> rightOperand = Right->generateCode(isInsideClassMethod, constantsTable); //Вычитаемое
	CodeGenerationHelpers::appendArrayToByteVector(&res, rightOperand.data(), rightOperand.size());

	vector<char> bytes = CodeGenerationCommands::isub(); //Команда
	CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());

	return res;
}

vector<char> Expression_node::generateCodeForMul(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	vector<char> leftOperand = Left->generateCode(isInsideClassMethod, constantsTable); //1-ый множитель
	CodeGenerationHelpers::appendArrayToByteVector(&res, leftOperand.data(), leftOperand.size());

	vector<char> rightOperand = Right->generateCode(isInsideClassMethod, constantsTable); //2-ой множитель
	CodeGenerationHelpers::appendArrayToByteVector(&res, rightOperand.data(), rightOperand.size());

	vector<char> bytes = CodeGenerationCommands::imul(); //Команда
	CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());

	return res;
}

vector<char> Expression_node::generateCodeForDiv(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	vector<char> leftOperand = Left->generateCode(isInsideClassMethod, constantsTable); //Делимое
	CodeGenerationHelpers::appendArrayToByteVector(&res, leftOperand.data(), leftOperand.size());

	vector<char> rightOperand = Right->generateCode(isInsideClassMethod, constantsTable); //Делитель
	CodeGenerationHelpers::appendArrayToByteVector(&res, rightOperand.data(), rightOperand.size());

	vector<char> bytes = CodeGenerationCommands::idiv(); //Команда
	CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());

	return res;
}

vector<char> Expression_node::generateCodeForUminus(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	vector<char> rightOperand = Right->generateCode(isInsideClassMethod, constantsTable); //Число
	CodeGenerationHelpers::appendArrayToByteVector(&res, rightOperand.data(), rightOperand.size());

	vector<char> bytes = CodeGenerationCommands::ineg(); //Команда
	CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());

	return res;
}

vector<char> Expression_node::generateCodeForUplus(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res = Right->generateCode(isInsideClassMethod, constantsTable); //Число
	return res;
}

vector<char> Expression_node::generateCodeForLiteral()
{
	vector<char> res;

	if (literal->type == CHAR_CONSTANT_TYPE) {
		vector<char> bytes = CodeGenerationCommands::iconstBipushSipush(*literal->value); //Команда
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
	else if (literal->type == STRING_CONSTANT_TYPE) {
		vector<char> size = CodeGenerationCommands::iconstBipushSipush(strlen(literal->value)); //Команда
		CodeGenerationHelpers::appendArrayToByteVector(&res, size.data(), size.size());
		vector<char> bytes = CodeGenerationCommands::newarray(CodeGenerationCommands::T_CHAR); //Команда
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
		for (int i = 0; i < strlen(literal->value); i++) {
			vector<char> dup = CodeGenerationCommands::dup(); //Дублирование ссылки на массив
			CodeGenerationHelpers::appendArrayToByteVector(&res, dup.data(), dup.size());
			vector<char> index = CodeGenerationCommands::iconstBipushSipush(i); //Индекс
			CodeGenerationHelpers::appendArrayToByteVector(&res, index.data(), index.size());
			vector<char> charValue = CodeGenerationCommands::iconstBipushSipush(literal->value[i]); //Значение
			CodeGenerationHelpers::appendArrayToByteVector(&res, charValue.data(), charValue.size());
			vector<char> aastore = CodeGenerationCommands::castore(); //Команда
			CodeGenerationHelpers::appendArrayToByteVector(&res, aastore.data(), aastore.size());
		}
	}
	else {
		vector<char> bytes = CodeGenerationCommands::ldc(literal->constant->Id); //Команда
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}

	return res;
}

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
