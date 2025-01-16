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
