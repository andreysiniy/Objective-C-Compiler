#include <iostream>
#include <map>
#include "classes_nodes.h"
using namespace std;

class LocalVariablesTable;
class FieldsTable;
class FieldsTableElement;
class MethodsTableElement;
class MethodsTable;
class PropertiesTable;
class Type;

// ---------- Таблица констант ----------

enum constantType {
    UTF8 = 1,
    Integer = 3,
    String = 8,
    Class = 7,
    NameAndType = 12,
    FieldRef = 9,
    MethodRef = 10
};

class ConstantsTableElement
{
public:
    int Id = NULL; // Номер константы
    constantType Type; // Тип константы
    string* Utf8String = NULL; // Строка для значения UTF-8 констант
    int Number = NULL; // Число для значения Integer констант
    int FirstRef = NULL; // Ссылка на 1-ую константу 
    int SecondRef = NULL; // Ссылка на 2-ую константу

    ConstantsTableElement(int id, constantType type, string utf8string);
    ConstantsTableElement(int id, constantType type, int number = NULL, int firstRef = NULL, int secondRef = NULL);

    string toCsvString(char separator = '|'); //Преобразование в строку формата CSV

	vector<char> generateBytes(); //Генерация байт кода константы
};

class ConstantsTable
{
public:
    int maxId = 1; // Наибольший номер константы
    map<int, ConstantsTableElement*> items; // Таблица констант


    ConstantsTable();

};
// ---------- Таблица функций ----------

class FunctionsTableElement
{
public:
    Statement_node* BodyStart = NULL; // Ссылка на узел начала тела функции
    LocalVariablesTable* LocalVariables = NULL; // Ссылка на соответствующую таблицу локальных переменных
    string NameStr; // Имя функции
    string DescriptorStr; // Дескриптор функции
	vector<Type*>* ParametersTypes = NULL; // Типы параметров
	Type* ReturnType = NULL; // Тип возвращаемого значения

    FunctionsTableElement(Statement_node* bodyStart, string nameStr, string descriptorStr, vector<Type*> *params, Type* returnType);


};

class FunctionsTable
{
public:
    static map<string, FunctionsTableElement*> items; // Таблица функций, в качестве ключа - Имя функции

    static FunctionsTableElement* addFunction(string name, string descriptor, Statement_node* bodyStart, vector<Type*>* params, Type* returnType);
};
