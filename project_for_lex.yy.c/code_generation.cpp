#include <fstream>
#include "tables.h"
#include "code_generation_helpers.h"
#include <iostream>
#include <string>

using namespace std;
// -------------------- Генерация байт-кода метода для атрибута Code --------------------

// ---------- Statement ----------
vector<char> Statement_node::generateCode(bool isInsideClassMethod, ConstantsTable* constantsTable, LocalVariablesTable *locals)
{
	vector<char> res;

	switch (type)
	{
	case EMPTY_STATEMENT_TYPE:
		break;
	case SIMPLE_STATEMENT_TYPE: {
		vector<char> bytes = generateCodeForSimpleStatement(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case RETURN_STATEMENT_TYPE: {
		vector<char> bytes = generateCodeForReturnStatement(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case IF_STATEMENT_TYPE: {
		vector<char> bytes = generateCodeForIfStatement(isInsideClassMethod, constantsTable, locals);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case WHILE_STATEMENT_TYPE: {
		vector<char> bytes = generateCodeForWhileStatement(isInsideClassMethod, constantsTable, locals);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case DO_WHILE_STATEMENT_TYPE: {
		vector<char> bytes = generateCodeForDoWhileStatement(isInsideClassMethod, constantsTable, locals);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case FOR_STATEMENT_TYPE: {
		vector<char> bytes = generateCodeForForStatement(isInsideClassMethod, constantsTable, locals);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case COMPOUND_STATEMENT_TYPE: {
		vector<char> bytes = generateCodeForCompoundStatement(isInsideClassMethod, constantsTable, locals);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case DECLARATION_STATEMENT_TYPE: {
		vector<char> bytes = generateCodeForDeclarationStatement(isInsideClassMethod, constantsTable, locals);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	default:
		break;
	}

	return res;
}

vector<char> Statement_node::generateCodeForSimpleStatement(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	vector<char> expr = Expression->generateCode(isInsideClassMethod, constantsTable);
	CodeGenerationHelpers::appendArrayToByteVector(&res, expr.data(), expr.size());

	return res;
}

vector<char> Statement_node::generateCodeForReturnStatement(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	if (Expression != NULL) {
		vector<char> expr = Expression->generateCode(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, expr.data(), expr.size()); //Загрузить expression (значение)

		if (Expression->DataType->DataType == CLASS_NAME_TYPE || Expression->DataType->DataType == ID_TYPE) { //Возвращается ссылка
			vector<char> returnBytes = CodeGenerationCommands::areturn();
			CodeGenerationHelpers::appendArrayToByteVector(&res, returnBytes.data(), returnBytes.size());
		}
		else { //Возвращается значение
			vector<char> returnBytes = CodeGenerationCommands::ireturn();
			CodeGenerationHelpers::appendArrayToByteVector(&res, returnBytes.data(), returnBytes.size());
		}
	}
	else { //void return
		vector<char> returnBytes = CodeGenerationCommands::return_();
		CodeGenerationHelpers::appendArrayToByteVector(&res, returnBytes.data(), returnBytes.size());
	}

	return res;
}

vector<char> Statement_node::generateCodeForDeclarationStatement(bool isInsideClassMethod, ConstantsTable* constantsTable, LocalVariablesTable *locals)
{
	vector<char> res;

	Declaration_node* declaration = (Declaration_node*)this;
	Init_declarator_node* initDeclarator = declaration->init_declarator_list->First;

	while (initDeclarator != NULL) {
		
		vector<char> bytes = initDeclarator->generateCodeForInitDeclarator(isInsideClassMethod, constantsTable, locals);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());

		initDeclarator = initDeclarator->Next;
	}

	return res;
}

vector<char> Init_declarator_node::generateCodeForInitDeclarator(bool isInsideClassMethod, ConstantsTable* constantsTable, LocalVariablesTable* locals)
{
	vector <char> res;

	LocalVariablesTableElement* local = locals->items[Declarator]; //Локальная переменная
	Type* dataType = local->type; //Тип локальной переменной
	if (dataType->DataType == INT_TYPE) { //Тип int
		if (dataType->ArrSize == NULL) { //Не является массивом
			vector<char> expr;
			if (expression != NULL) { //Указано начальное значение
				expr = expression->generateCode(isInsideClassMethod, constantsTable);
				CodeGenerationHelpers::appendArrayToByteVector(&res, expr.data(), expr.size()); //Загрузить expression (значение)
			}
			else { //Не указано начальное значение
				expr = CodeGenerationCommands::iconstBipushSipush(0);
				CodeGenerationHelpers::appendArrayToByteVector(&res, expr.data(), expr.size());
			}
			vector<char> istore = CodeGenerationCommands::istore(local->Id - isInsideClassMethod);
			CodeGenerationHelpers::appendArrayToByteVector(&res, istore.data(), istore.size());
		}
		else { //Является массивом
			vector<char> arrSize = dataType->ArrSize->generateCode(isInsideClassMethod, constantsTable);
			CodeGenerationHelpers::appendArrayToByteVector(&res, arrSize.data(), arrSize.size());
			vector<char> array = CodeGenerationCommands::newarray(CodeGenerationCommands::T_INT);
			CodeGenerationHelpers::appendArrayToByteVector(&res, array.data(), array.size());
			vector<char> astore = CodeGenerationCommands::astore(local->Id - isInsideClassMethod);
			CodeGenerationHelpers::appendArrayToByteVector(&res, astore.data(), astore.size());
			if (InitializerList != NULL) {
				vector<char> aload = CodeGenerationCommands::aload(local->Id - isInsideClassMethod);
				CodeGenerationHelpers::appendArrayToByteVector(&res, aload.data(), aload.size());

				Expression_node* cur = InitializerList->First;
				int i = 0;
				while (cur != NULL) {
					vector<char> dup = CodeGenerationCommands::dup(); //Дублирование ссылки на массив
					CodeGenerationHelpers::appendArrayToByteVector(&res, dup.data(), dup.size());
					vector<char> index = CodeGenerationCommands::iconstBipushSipush(i); //Индекс
					CodeGenerationHelpers::appendArrayToByteVector(&res, index.data(), index.size());
					vector<char> bytes = cur->generateCode(isInsideClassMethod, constantsTable); //Значение
					CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
					vector<char> iastore = CodeGenerationCommands::iastore(); //Команда
					CodeGenerationHelpers::appendArrayToByteVector(&res, iastore.data(), iastore.size());

					i++;
					cur = cur->Next;
				}
			}

		}
	}
	else if (dataType->DataType == CHAR_TYPE) { //Тип char
		if (dataType->ArrSize == NULL) { //Не является массивом
			vector<char> expr;
			if (expression != NULL) { //Указано начальное значение
				expr = expression->generateCode(isInsideClassMethod, constantsTable);
				CodeGenerationHelpers::appendArrayToByteVector(&res, expr.data(), expr.size()); //Загрузить expression (значение)
			}
			else { //Не указано начальное значение
				expr = CodeGenerationCommands::iconstBipushSipush(0);
				CodeGenerationHelpers::appendArrayToByteVector(&res, expr.data(), expr.size());
			}
			vector<char> istore = CodeGenerationCommands::istore(local->Id - isInsideClassMethod);
			CodeGenerationHelpers::appendArrayToByteVector(&res, istore.data(), istore.size());
		}
		else { //Является массивом
			vector<char> arrSize = dataType->ArrSize->generateCode(isInsideClassMethod, constantsTable);
			CodeGenerationHelpers::appendArrayToByteVector(&res, arrSize.data(), arrSize.size());
			vector<char> array = CodeGenerationCommands::newarray(CodeGenerationCommands::T_CHAR);
			CodeGenerationHelpers::appendArrayToByteVector(&res, array.data(), array.size());
			vector<char> astore = CodeGenerationCommands::astore(local->Id - isInsideClassMethod);
			CodeGenerationHelpers::appendArrayToByteVector(&res, astore.data(), astore.size());
			if (expression != NULL) {
				vector<char> aload = CodeGenerationCommands::aload(local->Id - isInsideClassMethod);
				CodeGenerationHelpers::appendArrayToByteVector(&res, aload.data(), aload.size());

				Literal_node* literal = expression->literal;
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
			else if (InitializerList != NULL) {
				vector<char> aload = CodeGenerationCommands::aload(local->Id - isInsideClassMethod);
				CodeGenerationHelpers::appendArrayToByteVector(&res, aload.data(), aload.size());

				Expression_node* cur = InitializerList->First;
				int i = 0;
				while (cur != NULL) {
					vector<char> dup = CodeGenerationCommands::dup(); //Дублирование ссылки на массив
					CodeGenerationHelpers::appendArrayToByteVector(&res, dup.data(), dup.size());
					vector<char> index = CodeGenerationCommands::iconstBipushSipush(i); //Индекс
					CodeGenerationHelpers::appendArrayToByteVector(&res, index.data(), index.size());
					vector<char> bytes = cur->generateCode(isInsideClassMethod, constantsTable); //Значение
					CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
					vector<char> castore = CodeGenerationCommands::castore(); //Команда
					CodeGenerationHelpers::appendArrayToByteVector(&res, castore.data(), castore.size());

					i++;
					cur = cur->Next;
				}

			}
		}
	}
	else if (dataType->DataType == CLASS_NAME_TYPE || dataType->DataType == ID_TYPE) { //Тип класса
		if (dataType->ArrSize == NULL) { //Не является массивом

			

			if (expression != NULL) { //Указано начальное значение
				vector<char> expr = expression->generateCode(isInsideClassMethod, constantsTable);
				CodeGenerationHelpers::appendArrayToByteVector(&res, expr.data(), expr.size()); //Загрузить expression (значение)
			}

			else { //Не указано начальное значение
				// Поиск константы с создаваемым классом
				int constUtf = constantsTable->findConstant(UTF8, &dataType->ClassName);
				int constClass = constantsTable->findConstant(Class, NULL, NULL, constUtf);

				vector<char> bytes = CodeGenerationCommands::new_(constClass); // Создание объекта
				CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
				vector<char> dup = CodeGenerationCommands::dup();
				CodeGenerationHelpers::appendArrayToByteVector(&res, dup.data(), dup.size());
				vector<char> aconstNull = CodeGenerationCommands::aconst_null();
				CodeGenerationHelpers::appendArrayToByteVector(&res, aconstNull.data(), aconstNull.size());
			}

			vector<char> astore = CodeGenerationCommands::astore(local->Id - isInsideClassMethod);
			CodeGenerationHelpers::appendArrayToByteVector(&res, astore.data(), astore.size());
		}
		else { //Является массивом
			// Поиск константы с создаваемым классом
			int constUtf = constantsTable->findConstant(UTF8, &dataType->ClassName);
			int constClass = constantsTable->findConstant(Class, NULL, NULL, constUtf);

			vector<char> arrSize = dataType->ArrSize->generateCode(isInsideClassMethod, constantsTable);
			CodeGenerationHelpers::appendArrayToByteVector(&res, arrSize.data(), arrSize.size());
			vector<char> array = CodeGenerationCommands::anewarray(constClass);
			CodeGenerationHelpers::appendArrayToByteVector(&res, array.data(), array.size());
			vector<char> astore = CodeGenerationCommands::astore(local->Id - isInsideClassMethod);
			CodeGenerationHelpers::appendArrayToByteVector(&res, astore.data(), astore.size());

			if (InitializerList != NULL) {
				vector<char> aload = CodeGenerationCommands::aload(local->Id - isInsideClassMethod);
				CodeGenerationHelpers::appendArrayToByteVector(&res, aload.data(), aload.size());

				Expression_node* cur = InitializerList->First;
				int i = 0;
				while (cur != NULL) {
					vector<char> dup = CodeGenerationCommands::dup(); //Дублирование ссылки на массив
					CodeGenerationHelpers::appendArrayToByteVector(&res, dup.data(), dup.size());
					vector<char> index = CodeGenerationCommands::iconstBipushSipush(i); //Индекс
					CodeGenerationHelpers::appendArrayToByteVector(&res, index.data(), index.size());
					vector<char> bytes = cur->generateCode(isInsideClassMethod, constantsTable); //Значение
					CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
					vector<char> aastore = CodeGenerationCommands::aastore(); //Команда
					CodeGenerationHelpers::appendArrayToByteVector(&res, aastore.data(), aastore.size());

					i++;
					cur = cur->Next;
				}

			}

		}
	}

	return res;
}

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

vector<char> Statement_node::generateCodeForCompoundStatement(bool isInsideClassMethod, ConstantsTable* constantsTable, LocalVariablesTable* locals)
{
	vector<char> res;
	
	Statement_list_node* statements = (Statement_list_node*)this;
	
	Statement_node* cur = statements->First;
	while (cur != NULL) {
		vector<char> bytes = cur->generateCode(isInsideClassMethod, constantsTable, locals);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
		cur = cur->Next;
	}

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

vector<char> Statement_node::generateCodeForForStatement(bool isInsideClassMethod, ConstantsTable* constantsTable, LocalVariablesTable* locals)
{
	vector<char> res;
	For_statement_node* for_stmt = (For_statement_node*)this;
	int num;
	int arrNum;

	// Инициализация
	if (for_stmt->ForType == FOR_FOR_TYPE) {
		vector<char> initBytes = for_stmt->InitExpression->generateCode(isInsideClassMethod, constantsTable); // Инициализация
		CodeGenerationHelpers::appendArrayToByteVector(&res, initBytes.data(), initBytes.size());
	}
	else if (for_stmt->ForType == FOR_WITH_DECLARATION_FOR_TYPE) {
		Init_declarator_node* initDeclarator = for_stmt->InitList->First;
		while (initDeclarator != NULL) {
			vector<char> bytes = initDeclarator->generateCodeForInitDeclarator(isInsideClassMethod, constantsTable, locals);
			CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
			initDeclarator = initDeclarator->Next;
		}
	}
	else if (for_stmt->ForType == FOREACH_FOR_TYPE) {
		num = locals->items["<iterator" + to_string(for_stmt->id) + ">"]->Id - isInsideClassMethod;
		arrNum = locals->items["<array" + to_string(for_stmt->id) + ">"]->Id - isInsideClassMethod;
		vector<char> initValue = CodeGenerationCommands::iconstBipushSipush(0); // Инициализация
		CodeGenerationHelpers::appendArrayToByteVector(&res, initValue.data(), initValue.size());
		vector<char> initBytes = CodeGenerationCommands::istore(num); // Инициализация
		CodeGenerationHelpers::appendArrayToByteVector(&res, initBytes.data(), initBytes.size());
	}
	else if (for_stmt->ForType == FOREACH_WITH_DECLARATION_FOR_TYPE) {
		LocalVariablesTableElement* local = locals->items[for_stmt->name];
		num = locals->items["<iterator" + to_string(for_stmt->id) + ">"]->Id - isInsideClassMethod;
		arrNum = locals->items["<array" + to_string(for_stmt->id) + ">"]->Id - isInsideClassMethod;
		vector<char> initValue = CodeGenerationCommands::iconstBipushSipush(0); // Инициализация
		CodeGenerationHelpers::appendArrayToByteVector(&res, initValue.data(), initValue.size());
		vector<char> initBytes = CodeGenerationCommands::istore(num); // Инициализация
		CodeGenerationHelpers::appendArrayToByteVector(&res, initBytes.data(), initBytes.size());
		
		// Поиск константы с создаваемым классом
		string className = for_stmt->NameType->ClassName;
		int constUtf = constantsTable->findConstant(UTF8, &className);
		int constClass = constantsTable->findConstant(Class, NULL, NULL, constUtf);

		vector<char> bytes = CodeGenerationCommands::new_(constClass); // Создание объекта
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
		vector<char> dup = CodeGenerationCommands::dup();
		CodeGenerationHelpers::appendArrayToByteVector(&res, dup.data(), dup.size());
		vector<char> aconstNull = CodeGenerationCommands::aconst_null();
		CodeGenerationHelpers::appendArrayToByteVector(&res, aconstNull.data(), aconstNull.size());

		vector<char> astore = CodeGenerationCommands::astore(local->Id - isInsideClassMethod);
		CodeGenerationHelpers::appendArrayToByteVector(&res, astore.data(), astore.size());
	}

	if (for_stmt->ForType == FOR_FOR_TYPE || for_stmt->ForType == FOR_WITH_DECLARATION_FOR_TYPE) {
		vector<char> bodyBytes = for_stmt->LoopBody->generateCode(isInsideClassMethod, constantsTable, locals); // Тело цикла
		vector<char> loopExpression = for_stmt->LoopExpression->generateCode(isInsideClassMethod, constantsTable); // Выражение цикла
		vector<char> gotoBytes = CodeGenerationCommands::goto_(bodyBytes.size() + loopExpression.size()); // Переход к условию цикла
		CodeGenerationHelpers::appendArrayToByteVector(&res, gotoBytes.data(), gotoBytes.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, bodyBytes.data(), bodyBytes.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, loopExpression.data(), loopExpression.size());

		vector<char> conditionBytes = for_stmt->ConditionExpression->generateCode(isInsideClassMethod, constantsTable); //Условие
		CodeGenerationHelpers::appendArrayToByteVector(&res, conditionBytes.data(), conditionBytes.size());

		int offset = bodyBytes.size() + conditionBytes.size() + loopExpression.size();
		offset = -offset;
		vector<char> ifBytes;
		if (for_stmt->ConditionExpression->DataType->isPrimitive()) {
			ifBytes = CodeGenerationCommands::if_(CodeGenerationCommands::NE, offset);
		}
		else {
			vector<char> aconst_null = CodeGenerationCommands::aconst_null(); //Загрузка null для сравнения объекта
			CodeGenerationHelpers::appendArrayToByteVector(&res, aconst_null.data(), aconst_null.size());
			ifBytes = CodeGenerationCommands::if_acmp(CodeGenerationCommands::NE, offset);
		}

		CodeGenerationHelpers::appendArrayToByteVector(&res, ifBytes.data(), ifBytes.size());
	}
	else {
		vector<char> arr = for_stmt->ConditionExpression->generateCode(isInsideClassMethod, constantsTable); //Объект
		int cloneConstant = constantsTable->findOrAddMethodRefConstant("rtl/NSArray", "arrayWithArrayStatic", "(Lrtl/NSArray;)Lrtl/NSArray;");
		vector<char> cloneInvoke = CodeGenerationCommands::invokestatic(cloneConstant); //Клонирование
		vector<char> cloneStore = CodeGenerationCommands::astore(arrNum); //Сохранение
		vector<char> cloneLoad = CodeGenerationCommands::aload(arrNum); //Загрузка
		
		vector<char> iload = CodeGenerationCommands::iload(num); //Индекс

		int constant = constantsTable->findOrAddMethodRefConstant("rtl/NSArray", "objectAtIndexDynamic", "(I)Lrtl/NSObject;");
		vector<char> value = CodeGenerationCommands::invokevirtual(constant); //Значение

		int valLocal = locals->items[for_stmt->name]->Id - isInsideClassMethod;
		vector<char> astore = CodeGenerationCommands::astore(valLocal); //Сохранение
		
		vector<char> bodyBytes;
		if (for_stmt->LoopBody != NULL)
			bodyBytes = for_stmt->LoopBody->generateCode(isInsideClassMethod, constantsTable, locals); // Тело цикла
		vector<char> incIter = CodeGenerationCommands::iinc(num, 1); // Инкремент
		int gotoOffset = cloneLoad.size() + iload.size() + value.size() + astore.size() + bodyBytes.size() + incIter.size();
		vector<char> gotoBytes = CodeGenerationCommands::goto_(gotoOffset); // Переход к условию цикла
		CodeGenerationHelpers::appendArrayToByteVector(&res, arr.data(), arr.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, cloneInvoke.data(), cloneInvoke.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, cloneStore.data(), cloneStore.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, gotoBytes.data(), gotoBytes.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, cloneLoad.data(), cloneLoad.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, iload.data(), iload.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, value.data(), value.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, astore.data(), astore.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, bodyBytes.data(), bodyBytes.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, incIter.data(), incIter.size());

		
		CodeGenerationHelpers::appendArrayToByteVector(&res, iload.data(), iload.size()); //Текущее значение итератора
		int sizeConst = constantsTable->findOrAddMethodRefConstant("rtl/NSArray", "countDynamic", "()I");
		vector<char> invokeVirtual = CodeGenerationCommands::invokevirtual(sizeConst); //Размер массива
		CodeGenerationHelpers::appendArrayToByteVector(&res, cloneLoad.data(), cloneLoad.size());
		CodeGenerationHelpers::appendArrayToByteVector(&res, invokeVirtual.data(), invokeVirtual.size());
		
		int offset = cloneLoad.size() + iload.size() + value.size() + astore.size() + bodyBytes.size() + incIter.size() + iload.size() + cloneLoad.size() + invokeVirtual.size();
		offset = -offset;
		vector<char> ifBytes;
		ifBytes = CodeGenerationCommands::if_icmp(CodeGenerationCommands::LT, offset);
		CodeGenerationHelpers::appendArrayToByteVector(&res, ifBytes.data(), ifBytes.size());
	}

	return res;

}


// ---------- Expression ----------

vector<char> Expression_node::generateCode(bool isInsideClassMethod, ConstantsTable *constantsTable)
{
	vector<char> res;

	switch (type)
	{
	case IDENTIFIER_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForIdentifier(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case LITERAL_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForLiteral();
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case NUMERIIC_CONSTANT_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForNumericConstant();
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case SELF_EXPRESSION_TYPE: {
		if (!isInsideClassMethod) {
			vector<char> bytes = generateCodeForSelf();
			CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
		}
	}
		break;
	case SUPER_EXPRESSION_TYPE: {
		if (!isInsideClassMethod) {
			vector<char> bytes = generateCodeForSuper();
			CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
		}
	}
		break;
	case MESSAGE_EXPRESSION_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForMessageExpression(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case UMINUS_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForUminus(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case UPLUS_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForUplus(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case PLUS_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForPlus(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case MINUS_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForMinus(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case MUL_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForMul(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case DIV_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForDiv(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case EQUAL_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForEqual(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case NOT_EQUAL_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForNotEqual(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case GREATER_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForGreater(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case LESS_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForLess(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case LESS_EQUAL_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForLessEqual(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case GREATER_EQUAL_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForGreaterEqual(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case ASSIGNMENT_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForAssignment(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case ARROW_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForArrow(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case ARRAY_ELEMENT_ACCESS_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForArrayElementAccess(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case ARRAY_ASSIGNMENT_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForArrayAssignment(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case MEMBER_ACCESS_ASSIGNMENT_EXPRESSION_TYPE: {
		vector<char> bytes = generateCodeForMemberAccessAssignment(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case CHAR_CAST: {
		vector<char> bytes = generateCodeForCharCast(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case INT_CAST: {
		vector<char> bytes = generateCodeForIntCast(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	case CLASS_CAST: {
		vector<char> bytes = generateCodeForClassCast(isInsideClassMethod, constantsTable);
		CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
	}
		break;
	default:
		break;
	}

	return res;
}
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

vector<char> Expression_node::generateCodeForAssignment(bool isInsideClassMethod, ConstantsTable* constantsTable)
{
	vector<char> res;

	if (Left->DataType->ClassName != "") { //Объект
		if (Left->LocalVariable != NULL) { //Запись в локальную переменную
			vector<char>rightOperand = Right->generateCode(isInsideClassMethod, constantsTable); //Левая часть выражения
			CodeGenerationHelpers::appendArrayToByteVector(&res, rightOperand.data(), rightOperand.size());
			vector<char> bytes = CodeGenerationCommands::astore(Left->LocalVariable->Id - isInsideClassMethod); //Команда
			CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
		}
		else if (Left->Field != NULL) { //Запись в поле
			vector<char> obj = CodeGenerationCommands::aload(0); //Объект
			CodeGenerationHelpers::appendArrayToByteVector(&res, obj.data(), obj.size());

			if (constantsTable->items.count(Left->Constant) == 0) {
				string msg = "Class doesn't have constant " + to_string(Constant);
				throw new std::exception(msg.c_str());
			}
			else if (constantsTable->items[Left->Constant]->Type != FieldRef) {
				string msg = "Constant " + to_string(Left->Constant) + " is not fieldRef";
				throw new std::exception(msg.c_str());
			}
			vector<char>rightOperand = Right->generateCode(isInsideClassMethod, constantsTable); //Левая часть выражения
			CodeGenerationHelpers::appendArrayToByteVector(&res, rightOperand.data(), rightOperand.size());

			vector<char> field = CodeGenerationCommands::putfield(Left->Constant); //Поле
			CodeGenerationHelpers::appendArrayToByteVector(&res, field.data(), field.size());
		}
		else {
			string msg = "Unknown identifier '" + string(name);
			throw new std::exception(msg.c_str());
		}
	}
	else { //Целое число
		if (Left->LocalVariable != NULL) {//Запись в локальную переменную
			vector<char>rightOperand = Right->generateCode(isInsideClassMethod, constantsTable); //Левая часть выражения
			CodeGenerationHelpers::appendArrayToByteVector(&res, rightOperand.data(), rightOperand.size());
			vector<char> bytes = CodeGenerationCommands::istore(Left->LocalVariable->Id - isInsideClassMethod); //Команда
			CodeGenerationHelpers::appendArrayToByteVector(&res, bytes.data(), bytes.size());
		}
		else if (Left->Field != NULL) { //Запись в поле
			vector<char> obj = CodeGenerationCommands::aload(0); //Объект
			CodeGenerationHelpers::appendArrayToByteVector(&res, obj.data(), obj.size());

			if (constantsTable->items.count(Left->Constant) == 0) {
				string msg = "Class doesn't have constant " + to_string(Constant);
				throw new std::exception(msg.c_str());
			}
			else if (constantsTable->items[Left->Constant]->Type != FieldRef) {
				string msg = "Constant " + to_string(Left->Constant) + " is not fieldRef";
				throw new std::exception(msg.c_str());
			}
			vector<char>rightOperand = Right->generateCode(isInsideClassMethod, constantsTable); //Левая часть выражения
			CodeGenerationHelpers::appendArrayToByteVector(&res, rightOperand.data(), rightOperand.size());

			vector<char> field = CodeGenerationCommands::putfield(Left->Constant); //Поле
			CodeGenerationHelpers::appendArrayToByteVector(&res, field.data(), field.size());
		}
		else {
			string msg = "Unknown identifier '" + string(name);
			throw new std::exception(msg.c_str());
		}
	}

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
