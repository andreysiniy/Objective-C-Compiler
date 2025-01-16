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
// ---------- Таблица классов ----------

class ClassesTableElement
{
public:
    int Name; // Ссылка на константу с именем класса
    int SuperclassName; // Ссылка на константу с именем родительского класса
    bool IsImplementation; //Флаг, который показывает является ли класс реализацией (ВОЗМОЖНО, СТОИТ ДОБАВИТЬ ИМЕННО НАЛИЧИЕ РЕАЛИЗАЦИИ КЛАССА)
    bool IsHaveInterface = false; // Флаг, который показывает имеет ли реализация интерфейс
    int ThisClass; // Ссылка на константу текущего класса
    int Superclass; // Ссылка на константу родительского класса
    FieldsTable* Fields; // Ссылка на соответствующую таблицу полей класса
    MethodsTable* Methods; // Ссылка на соответстующую таблицу методов класса
    PropertiesTable* Properties; // Ссылка на соответствующую таблицу свойств класса
    ConstantsTable* ConstantTable; // Таблица констант
    int constructorNumber; //Ссылка на константу с methodRef конструктора родителя

    ClassesTableElement(string name, string *superclassName, bool isImplementation);


	void addConstantsToTable(); //Добавляет константы типа Class в таблицу


private:
	void appendConstructor();
};

class ClassesTable
{
public:
    static map<string, ClassesTableElement*> *items; // Таблица классов, в качестве ключа - Имя класса

    static ClassesTableElement* addClass(string name, string *superclassName, bool isImplementation, Class_block_node *classBlock);

	static void initRTL();

private:
    static void initClassProgram();
	static void initClassInOutFuncs();
    static void initClassNSObject();
	static void initClassNSString();
    static void initClassNSArray();
};

// ---------- Таблица полей класса ----------

class FieldsTableElement
{
public:
    int Name = NULL; // Ссылка на константу с именем поля
    int Descriptor = NULL; // Ссылка на константу с дескриптором поля
    bool IsInstance = NULL; // Флаг, показывающий является ли поле частью экземпляра класса
	int InstanceIndex = NULL; // Индекс instance variable в списке (нужно, так как важен их порядок)
    Type* type; // Тип поля
    string NameStr; // Имя поля
    string DescriptorStr; // Дескриптор поля
	Expression_node* InitialValue = NULL; // Инициализирующее выражение

    FieldsTableElement(int name, int descriptor, bool isInstance, int instanceIndex, Type* type, string nameStr, string descriptorStr, Expression_node* initialValue);

    void fillLiterals(ConstantsTable* constantTable);

};

class FieldsTable
{
public:
	int maxInstanceIndex = 1;
    map < string, FieldsTableElement*> items; //Таблица полей класса, в качестве ключа - Имя поля класса

    void addField(ConstantsTable* constantTable, string name, string descriptor, bool isInstance, Type* type, Expression_node* initValue);

};

// ---------- Таблица методов ----------

class MethodsTableElement
{
public:
    int Name = NULL; // Ссылка на константу с именем метода
    int Descriptor = NULL; // Ссылка на константу с дескриптором метода (НУЖНО ЛИ НА KEYWORD ИЛИ ИХ НУЖНО ПРЕОБРАЗОВЫВАТЬ К СТАНДАРТНОМУ ДЕСКРИПТОРУ ФУНКЦИИ?)
    bool IsClassMethod = NULL; // Флаг, который показывает принадлежность метода к классу, а не объекту
    Statement_node* BodyStart = NULL; // Ссылка на узел начала тела метода
    LocalVariablesTable* LocalVariables = NULL; // Ссылка на соответствующую таблицу локальных переменных
    Type* ReturnType; //Тип возвращаемого значения
    vector<Type*>* ParamsTypes = NULL; //Тип параметров
    vector<Type*>* KeywordsTypes = NULL; //Тип параметров keyword

    string NameStr; // Имя метода
    string DescriptorStr; // Дескриптор метода

    MethodsTableElement(int name, int descriptor, bool isClassMethod, Statement_node* bodyStart, Type* returnType, vector<Type*>* paramsTypes, vector<Type*>* keywordsTypes, string nameStr, string descriptorStr);
	void fillLiterals(ConstantsTable* constantTable);


};

class MethodsTable
{
public:
    map< string, MethodsTableElement* > items; //Таблица методов класса, в качестве ключа - Имя метода класса

    MethodsTableElement* addMethod(ConstantsTable* constantTable, string name, string descriptor, bool isClassMethod, Statement_node* bodyStart, Type* returnType, vector<Type*>* paramsTypes, vector<Type*>* keywordsTypes);

	void addConstantsToTable(ConstantsTable* constantsTable); //Добавляет константы типа Class в таблицу
};
};
