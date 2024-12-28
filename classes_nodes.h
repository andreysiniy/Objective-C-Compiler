#include <vector>
#include <set>
#include <iostream>
using namespace std;

//---------- Прототипы классов ----------
class Init_declarator_list_node;
class Init_declarator_node;
class Expression_node;
class Parameter_declaration_node;
class Receiver_node;
class Message_selector_node;
class Keyword_argument_list_node;
class Keyword_argument_node;
class Class_declaration_list_node;
class Statement_list_node;
class Class_interface_node;
class Class_implementation_node;
class Interface_body_node;
class Interface_declaration_list_node;
class Implementation_definition_list_node;
class Class_list_node;
class Property_node;
class Method_declaration_node;
class Method_selector_node;
class Method_definition_node;
class Keyword_selector_node;
class Keyword_declaration_node;
class Attribute_node;
class Function_and_class_list_node;
class Function_node;
class Instance_variables_declaration_list_node;
class Expression_list_node;


// -------------------- Типы --------------------

// ---------- type ----------

enum type_type {
    INT_TYPE,
    CHAR_TYPE,
    FLOAT_TYPE,
    ID_TYPE,
    CLASS_NAME_TYPE,
    VOID_TYPE
};

class Type_node
{
    public:
        int id;
        enum type_type type;
        char *ClassName;

        static Type_node* createTypeNode(type_type type);
        static Type_node* createTypeNodeFromClassName(type_type type, char *name);
};
// -------------------- Константы --------------------

// ---------- numeric_constant ----------

enum numeric_constant_type {
    FLOAT_CONSTANT_TYPE,
    INTEGER_CONSTANT_TYPE
};

class Numeric_constant_node
{
    public:
        int id;
        enum numeric_constant_type type;
        union number
        {
            int Int;
            float Float;
        } number;

        static Numeric_constant_node* createNumericConstantNodeFromInteger(int number);
        static Numeric_constant_node* createNumericConstantNodeFromFloat(float number);

};
enum literal_type {
    STRING_CONSTANT_TYPE,
    CHAR_CONSTANT_TYPE,
    NSSTRING_CONSTANT_TYPE
};
class Literal_node
{
    public:
        int id;
        enum literal_type type;
        char *value;

        static Literal_node* createLiteralNode(literal_type type, char *value);
};
