%{
    #include <iostream>
    #include <fstream>
    #include <FlexLexer.h>
    #include <string>
    #include "./SymbolTable/SymbolTable.h"

    #define KEYWORD_COUNT 21
    #define BUCKET_COUNT 7

    using namespace std;

    int lineNo = 1;
    ofstream logF;
    ofstream outF;
    ifstream inputFile;
    string logFile = "log.txt";
    string tokenFile = "token.txt";
    SymbolTable* keywordTable;

    class Keyword {
    public:
        Keyword(string name, string token)
        {
            this->name = name;
            this->token = token;
        }

        string name;
        string token;
    };

    void populateKeyword(SymbolTable* keywordTable);
%}

%option c++
%option noyywrap

DIGIT [0-9]
LETTER [a-zA-Z]
 
    /*RULE SECTION*/
%%

({LETTER}|_)({LETTER}|{DIGIT}|_)* {
    /*Rule for ID*/
}

\n {lineNo++;}
<INITIAL>[ \t] {/*does nothing*/}

%%
    /*SUBROUTINES*/
int main(int argc,char** argv)
{
    if(argc < 2)
        return 1;
    
    logF.open(logFile);
    outF.open(logFile);
    inputFile.open(argv[1]);
    
    if(!logF.is_open() || !outF.is_open() || !inputFile.is_open())
        return 1;
    
    keywordTable = new SymbolTable(BUCKET_COUNT);
    populateKeyword(keywordTable);

    yyFlexLexer lexer(&inputFile);
    lexer.yylex();

    logF.close();
    outF.close();

    return 0;
}

void populateKeyword(SymbolTable* keywordTable)
{
    Keyword keyword[KEYWORD_COUNT] = {{"if", "IF"}, {"for", "FOR"}, {"do", "DO"}, {"int", "INT"}, {"float", "FLOAT"},
                   {"void", "VOID"}, {"switch", "SWITCH"}, {"default", "DEFAULT"}, {"else", "ELSE"},
                   {"while", "WHILE"}, {"break", "BREAK"}, {"char", "CHAR"}, {"double", "DOUBLE"}, 
                   {"return", "RETURN"}, {"case", "CASE"}, {"continue", "CONTINUE"}, {"goto", "GOTO"}, 
                   {"long", "LONG"}, {"short", "SHORT"}, {"static", "STATIC"}, {"unsigned", "UNSIGNED"}};

    for(int i = 0; i < KEYWORD_COUNT; ++i)
    {
        keywordTable->insertSymbol(keyword[i].name, keyword[i].token);
    }
}