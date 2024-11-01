%option noyywrap
%option never-interactive

DIGIT [0-9]
UNDERLINE_LITERAL [A-Za-z_]

%x MULTILINE_COMMENT
%x STRING
%x CHAR

%{
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>

    int isContainsSymbol(char *string, char symbol);
    void removeSymbol(char *string, char symbol);
    void removeFloatSuffix(char *string);
	void removeULSuffix(char *string);
    int countChar(const char *str, char character);
%}

%%
%{
    char tmp[10000];
    int rowCount = 1;
%}

"int" {printf("Found keyword INT: %s\n", yytext);}
"char" {printf("Found keyword CHAR: %s\n", yytext);}
"if" {printf("Found keyword IF: %s\n", yytext);}
"else" {printf("Found keyword ELSE: %s\n", yytext);}
"while" {printf("Found keyword WHILE: %s\n", yytext);}
"do" {printf("Found keyword DO: %s\n", yytext);}
"for" {printf("Found keyword FOR: %s\n", yytext);}
"in" {printf("Found keyword IN: %s\n", yytext);}
"float" {printf("Found keyword FLOAT: %s\n", yytext);}
"double" {printf("Found keyword DOUBLE: %s\n", yytext);}
"readonly" {printf("Found keyword READONLY: %s\n", yytext);}
"readwrite" {printf("Found keyword READWRITE: %s\n", yytext);}
"enum" {printf("Found keyword ENUM: %s\n", yytext);}
"unsigned" {printf("Found keyword UNSIGNED: %s\n", yytext);}
"long" {printf("Found keyword LONG: %s\n", yytext);}
"return" {printf("Found keyword RETURN: %s\n", yytext);}
"self" {printf("Found keyword SELF: %s\n", yytext);}
"super" {printf("Found keyword SUPER: %s\n", yytext);}
"id" {printf("Found keyword ID: %s\n", yytext);}
"void" {printf("Found keyword VOID: %s\n", yytext);}
"@class" {printf("Found keyword CLASS: %s\n", yytext);}
"@interface" {printf("Found keyword INTERFACE: %s\n", yytext);}
"@end" {printf("Found keyword END: %s\n", yytext);}
"@implementation" {printf("Found keyword IMPLEMENTATION: %s\n", yytext);}
"@property" {printf("Found keyword PROPERTY: %s\n", yytext);}
"@private" {printf("Found keyword PRIVATE: %s\n", yytext);}
"@public" {printf("Found keyword PUBLIC: %s\n", yytext);}

"+" {printf("Found arithmetic operation +: %s\n", yytext);}
"-" {printf("Found arithmetic operation -: %s\n", yytext);}
"*" {printf("Found arithmetic operation *: %s\n", yytext);}
"/" {printf("Found arithmetic operation /: %s\n", yytext);}

"==" {printf("Found comparison operation ==: %s\n", yytext);}
"!=" {printf("Found comparison operation !=: %s\n", yytext);}
">" {printf("Found comparison operation >: %s\n", yytext);}
"<" {printf("Found comparison operation <: %s\n", yytext);}
">=" {printf("Found comparison operation >=: %s\n", yytext);}
"<=" {printf("Found comparison operation <=: %s\n", yytext);}

"=" {printf("Found assignment operation =: %s\n", yytext);}

"(" {printf("Found bracket (: %s\n", yytext);}
")" {printf("Found bracket ): %s\n", yytext);}
"{" {printf("Found bracket {: %s\n", yytext);}
"}" {printf("Found bracket }: %s\n", yytext);}
"[" {printf("Found bracket [: %s\n", yytext);}
"]" {printf("Found bracket ]: %s\n", yytext);}

";" {printf("Found delimiter ;: %s\n", yytext);}
":" {printf("Found delimiter :: %s\n", yytext);}
"," {printf("Found delimiter ,: %s\n", yytext);}

<*>\\\n {rowCount++;}

(0|[1-9][0-9]*)([uU]|l{1,2}|L{1,2})? {
	strcpy(tmp, yytext);
    removeULSuffix(tmp);
	printf("Found integer: %d\n", atoi(tmp));}
(0x|0X)[0-9a-fA-F]+ {
    char* a;
    long number = strtol(yytext+2, &a,16);
    printf("Found hexadecimal number: %d\n", number);
    }
	
0[xX][0-9a-zA-Z_]+ {  
	printf("ERROR! Incorrect hexadecimal number: %s in row: %d\n", yytext, rowCount);
}
0[0-7]+ {
    char *a;
    long number = strtol(yytext+2, &a,8);
    printf("Found octal number: %d\n", number);
    }
0[0-9]+ {
    printf("ERROR! Incorrect octal number: %s in row: %d\n", yytext, rowCount);
}

\' strcpy(tmp,""); BEGIN(CHAR);
@\" strcpy(tmp,""); BEGIN(STRING);
<STRING>[^\\\n\"]+ strcat(tmp,yytext);
<CHAR>[^\\\n\'] strcat(tmp,yytext);
<STRING,CHAR>\\b strcat(tmp,"\b");
<STRING,CHAR>\\f strcat(tmp,"\f");
<STRING,CHAR>\\n strcat(tmp,"\n");
<STRING,CHAR>\\r strcat(tmp,"\r");
<STRING,CHAR>\\t strcat(tmp,"\t");
<STRING,CHAR>\\v strcat(tmp,"\v");
<STRING,CHAR>\\\' strcat(tmp,"\'");
<STRING,CHAR>\\\" strcat(tmp,"\"");
<STRING,CHAR>\\\\ strcat(tmp,"\\");
<STRING,CHAR>\\\? strcat(tmp,"\?");
<STRING,CHAR>\\x[0-9a-fA-F]{1,2} {
    char dig[2];
    char *a;
    dig[0]=strtol(yytext+2, &a, 16);
    dig[1]='\0';
    strcat(tmp, dig);}
<STRING,CHAR>\\[0-7]{1,3} {
    char dig[2];
    char *a;
    dig[0]=strtol(yytext+1, &a, 8);
    dig[1]='\0';
strcat(tmp, dig);}
<STRING,CHAR>\n {printf("ERROR! String Breakes in row: %d\n", rowCount); rowCount++;}
<STRING,CHAR><<EOF>> {printf("ERROR! haven't found closing quotes in row: %d\n", rowCount); BEGIN(INITIAL);}
<STRING>\" {printf("Found String literal: %s\n",tmp);BEGIN(INITIAL);}

<CHAR>\' {
    if(strlen(tmp) != 1)
    {
        printf("ERROR! char length != 1 in row: %d\n", rowCount);BEGIN(INITIAL);
    }
    else
    {
        printf("Found Char literal: %s\n",tmp);BEGIN(INITIAL);
    }
}

({DIGIT}+\.{DIGIT}*|\.{DIGIT}+)(f|F)? {
    strcpy(tmp, yytext);
    removeFloatSuffix(tmp);
    printf("Found float constant: %f\n", (float)atof(tmp));
}

({DIGIT}+\.?{DIGIT}*|\.{DIGIT}+)(e|E)[-+]?{DIGIT}+[fF]? {
    strcpy(tmp, yytext);
    removeFloatSuffix(tmp);
    printf("Found float constant: %f\n", (float)atof(tmp));
}

{UNDERLINE_LITERAL}({UNDERLINE_LITERAL}|{DIGIT})* {printf("Found identifier: %s\n", yytext);}
{DIGIT}({UNDERLINE_LITERAL}|{DIGIT})+ {printf("ERROR! Incorrect identifier name: %s in row: %d\n", yytext, rowCount);}

"//"([^\n]|\\\n)* {
	strcpy(tmp, "");
    strcpy(tmp, yytext + 2);
    printf("Found line comment: %s\n", tmp);
	rowCount += countChar(tmp, '\n');
}

"/*" {tmp[0] = '\0'; BEGIN(MULTILINE_COMMENT);}
<MULTILINE_COMMENT>[^*\n]+ {strcat(tmp, yytext);}
<MULTILINE_COMMENT>[^*\n]+\n {rowCount++; strcat(tmp, yytext);}
<MULTILINE_COMMENT>"*"+[^*/] {strcat(tmp, yytext);}
<MULTILINE_COMMENT>"*"+\n {rowCount++; strcat(tmp, yytext);}
<MULTILINE_COMMENT>"*"+"/" {printf("Found multiline comment: %s\n", tmp);BEGIN(INITIAL);}

<MULTILINE_COMMENT><<EOF>> {printf("ERROR! Multiline comment \"%s\" isn't closed in row: %d\n", tmp, rowCount); BEGIN(INITIAL);}

<*>\n {rowCount++;}

[\r\t\f\v ]+ {printf("", yytext);}

<*>. {printf("unrecognized element: %s\n", yytext);}

%%

int isContainsSymbol(char *string, char symbol)
{
    return strchr(string, symbol) != NULL;
}

void removeSymbol(char *string, char symbol)
{
    if (isContainsSymbol(string, symbol))
    {
        char* pointer = strchr(string, symbol);
        int symbolPos = pointer - string;
        int len = strlen(string);
        for (int i=symbolPos; i < len; i++)
        {
            string[i] = string[i+1];
        }
        string[len-1] = '\0';
    }
}

void removeFloatSuffix(char *string)
{
    if (isContainsSymbol(string, 'f') || isContainsSymbol(string, 'F'))
    {
        removeSymbol(string, 'f');
        removeSymbol(string, 'F');
    }
}

void removeULSuffix(char *string)
{
    if (isContainsSymbol(string, 'u') || isContainsSymbol(string, 'U') || isContainsSymbol(string, 'l') ||
	isContainsSymbol(string, 'L'))
    {
        removeSymbol(string, 'u');
        removeSymbol(string, 'U');
		removeSymbol(string, 'L');
		removeSymbol(string, 'L');
		removeSymbol(string, 'l');
		removeSymbol(string, 'l');
    }
}

int countChar(const char *str, char character)
{
    int count = 0;
    int len = strlen(str);
    for (int i=0; i<len; i++)
    {
        if (str[i] == character)
            count++;
    }
    return count;
}

int main(int argc, char* argv[])
{
    if ( argc > 1 )
            yyin = fopen( argv[1], "r" );
    else
            yyin = stdin;

    yylex();
}
