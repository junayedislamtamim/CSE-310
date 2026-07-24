%{
    #include <iostream>
    #include <fstream>
    #include <FlexLexer.h>
    #include <string>
    #include <cctype>
    #include "./SymbolTable/SymbolTable.h"

    #define KEYWORD_COUNT 21
    #define BUCKET_COUNT 7

    using namespace std;

    int lineNo = 1;
    int errorCount = 0;
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
    bool isKeyword(const string& str);
    void log(int lineNo, const string& token, const string& lexeme);
    bool isFloat(const string& str);
%}

%option c++
%option noyywrap

%x STRING 
%x FLOAT

DIGIT [0-9]
LETTER [a-zA-Z]
ESC_CHAR \[nt\'afrbv0]
 
    /*RULE SECTION*/
%%
<INITIAL>{DIGIT}+{LETTER}+ {
    logF << "Invalid prefix on identifier " << string(yytext) << '\n';
}

<INITIAL>({LETTER}|_)({LETTER}|{DIGIT}|_)* {
    /*Rule for ID and KEYWORD*/
    string token;
    if(isKeyword(yytext))
    {
        string type = keywordTable->lookUp(yytext, true)->getSymbolType();
        token = "<" + type + ">";
    }
    else
    {
        token =  "<ID, " + string(yytext) + ">\n"; 
    }

    outF << token << ' ';
    log(lineNo, token, yytext);
}

    // <INITIAL>{DIGIT}*\. {
    //     /*Rule for floating point number*/
    //     yymore();
    //     BEGIN(FLOAT);
    // }

    // <FLOAT>{DIGIT}+((E[+-]?)?{DIGIT}+)? {
    //     string token = "<CONST_FLOAT, " + string(yytext) + ">";
    //     outF << token << ' ';
    //     log(lineNo, token, yytext);
    //     BEGIN(INITIAL);
    // }

    // <FLOAT>({DIGIT}|\.)+((E[+-]?)?{DIGIT}+)* {
    //     ++errorCount;
    //     logF << "Too many decimal points " << string(yytext) << '\n';
    //     BEGIN(INITIAL);
    // }

    // <INITIAL>{DIGIT}*E+({DIGIT}|E)*\.({DIGIT}|E)* {
    //     logF << "Ill formed number " << string(yytext) << '\n';
    // }

<INITIAL>({DIGIT}|{LETTER}|[+-])*\.({DIGIT}|{LETTER}|[+-])* {
    int dots = 0;
    int e = 0;

    for (const auto& ch: yytext) 
    {
        if(ch == '.')
            dots++;
        else if(ch == 'E')
            e++;
    }

    if(dots > 1)
        logF << "Line No. " << lineNo << " Too many dots. " << '\n';
    else if(!isFloat(yytext))
        logF << "Line No. " << lineNo << " Ill formed floating point number" << '\n';
    else
    {
        string token = string("<CONST_FLOAT, ") + yytext + ">";
        log(lineNo, token, yytext);
        outF << token << ' ';
    }
}

<INITIAL>'[^']'? {
    /*Rule for const chars*/
    char ch;
    if(yyleng == 1)
    {
        logF << "Line No. " << lineNo << " Stray '\n";
        ++errorCount;
        return;
    }
    if(yyleng == 2 && yytext[1] == '\'')
    {
        logF << "Line No. " << lineNo << " Empty character constant\n";
        ++errorCount;
        return;
    }
    if(yytext[1] != '\\' && yyleng != 3)
    {
        if(yyleng > 3)
        {
            logF << "Line No. " << lineNo << " Multi character constant error. Lexeme: '" << string(yytext) << '\n';
        }
        else
        {
            logF << "Line No. " << lineNo << " Unfinished character: '" << yytext << '\n';
        }

        ++errorCount;
        return;
    }

    if(yytext[1] == '\\' && yyleng != 4)
    {
        if(yyleng > 4)
        {
            logF << "Line No. " << lineNo << " Multi character constant error. Lexeme: '" << string(yytext) << '\n';
        }
        else
        {
            logF << "Line No. " << lineNo << " Unfinished character: '" << yytext << '\n';
        }

        ++errorCount;
        return;
    }

    if(yytext[1] == '\\')
    {
        switch(yytext[2])
        {
            case 'n': ch = (unsigned char)'\n'; break;
            case 't': ch = (unsigned char)'\t'; break;
            case '\\': ch = (unsigned char)'\\'; break;
            case '\'': ch = (unsigned char)'\''; break;
            case 'a': ch = (unsigned char)'\a'; break;
            case 'f': ch = (unsigned char)'\f'; break;
            case 'r': ch = (unsigned char)'\r'; break;
            case 'b': ch = (unsigned char)'\b'; break;
            case 'v': ch = (unsigned char)'\v'; break;
            case '0': ch = (unsigned char)'\0'; break;
            default:
                logF << "Line No. " << lineNo << " Unrecognized character " << string(yytext) << '\n';
                ++errorCount;
                return;
        }
    }
    else  
        ch = yytext[1];
    
    string token = string("<CONST_CHAR, ") + ch + ">";
    outF << token << ' ';
    log(lineNo, token, yytext);
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

bool isKeyword(SymbolTable* keywordTable, const string& str)
{
    return keywordTable->lookUp(str, true);
}

void log(int lineNo, const string& token, const string& lexeme)
{
    logF << "Line No. " << lineNo << ": Token "  << token <<  " Lexeme "  << lexeme << " found.\n";
}

bool isFloat(const string& str) 
{
    //FLOAT = ({DIGIT})*\.({DIGIT})+(E[+-]?{DIGIT}+)? 

    int i = 0;
    int n = yyleng;

    if(i == n || !(isdigit((unsigned char)str[i]) || str[i] == '.')) return false;

    while(i < n && isdigit((unsigned char)str[i])) ++i;

    if(i < n && str[i] == '.') ++i;
    else 
        return false;

    if(i == n || !isdigit((unsigned char)str[i])) return false;
    while(i < n && isdigit((unsigned char)str[i])) ++i;

    if(i < n && str[i] == 'E')
    {
        ++i;
        if(i < n && (str[i] == '+' || str[i] == '-')) ++i;

        if(i == n || !isdigit((unsigned char)str[i])) return false;
        while(i < n && isdigit((unsigned char)str[i])) ++i;
    }

    return i == n;
}