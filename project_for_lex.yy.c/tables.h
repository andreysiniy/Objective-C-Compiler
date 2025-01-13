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
