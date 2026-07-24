%{
    #include <iostream>
    #include <fstream>
    #include <string>
    #include <cctype>
    #include "../SymbolTable/SymbolTable.h"

    #define KEYWORD_COUNT 21
    #define BUCKET_COUNT 7
    #define SILENT true

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
    string getParenType(const string& str);
    string getOPType(const string& str);
    string processString(const string& str);
%}

%option c++
%option noyywrap

%x STRING 
%x SLCOMM
%x MLCOMM

DIGIT [0-9]
LETTER [a-zA-Z]
ESC_CHAR \[nt\'afrbv0]
OP ([+\-*/%<>=]|"++"|"--"|"<="|">="|"=="|"!="|"&&"|"||")
PAREN [(){}\[\],;]
 
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
        string type = keywordTable->lookUp(yytext)->getSymbolType();
        token = "<" + type + ">";
    }
    else
    {
        token =  "<ID, " + string(yytext) + ">"; 
    }

    outF << token << ' ';
    log(lineNo, token, yytext);
}

<INITIAL>{DIGIT}+ {
    string token = string("<CONST_INT, ") + yytext + ">";

    outF << token << ' ';
    log(lineNo, token, yytext);
}

<INITIAL>({DIGIT}|{LETTER}|[+-])*\.({DIGIT}|{LETTER}|[+-])* {
    /*RULE for FLOATS*/
    int dots = 0;
    int e = 0;

    string str{yytext};
    for (const auto& ch: str) 
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
        return 0;
    }
    if(yyleng == 2 && yytext[1] == '\'')
    {
        logF << "Line No. " << lineNo << " Empty character constant\n";
        ++errorCount;
        return 0;
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
        return 0;
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
        return 0;
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
                return 0;
        }
    }
    else  
        ch = yytext[1];
    
    string token = string("<CONST_CHAR, ") + ch + ">";
    outF << token << ' ';
    log(lineNo, token, yytext);
}

<INITIAL>{OP} {
    string type = getOPType(yytext);

    string token = string("<") + type + ", " + yytext + ">";
    outF << token << ' ';
    log(lineNo, token, yytext);
}

<INITIAL>{PAREN} {
    string type = getParenType(yytext);

    string token = string("<") + type + ", " + yytext + ">";
    outF << token << ' ';
    log(lineNo, token, yytext);
}

<INITIAL>\" {
    yymore();
    BEGIN(STRING);
}

<STRING>\\\n {
    lineNo++;
    yymore();
}

<STRING>\n {
    lineNo++;
    logF << "Line No. " << lineNo << " Unfinished string.\n";
    BEGIN(INITIAL);
}

<STRING>{ESC_CHAR} {
    yymore();
}

<STRING>[^\"] {
    yymore();
}

<STRING>\" {
    string str = processString(yytext);
    string token = string("<STRING, ") + str + ">";
    log(lineNo, token, str);
    outF << token << ' ';
    BEGIN(INITIAL);
}

<INITIAL>"//" {
    yymore();
    BEGIN(SLCOMM);
}

<SLCOMM>\\\n {
    yymore();
    ++lineNo;
}

<SLCOMM>[^\n] {
    yymore();
}

<SLCOMM>\n {
    string token = "COMMENT";
    log(lineNo, token, yytext);
    BEGIN(INITIAL);
}

<INITIAL>"/*" {
    yymore();
    BEGIN(MLCOMM);
}

<MLCOMM>\n {
    lineNo++;
    yymore();
}

<MLCOMM>. {
    yymore();
}

<MLCOMM>"*/" {
    log(lineNo, "COMMENT", yytext);
    BEGIN(INITIAL);
}

<MLCOMM><<EOF>> {
    logF << "Line No. " << lineNo << " Unfinished comment.\n";
    yyterminate();
}

\n {lineNo++;}
<INITIAL>[ \t] {/*does nothing*/}

%%
    /*SUBROUTINES*/
int main(int argc,char** argv)
{
    if(argc < 2)
        return 0;
    
    logF.open(logFile);
    outF.open(tokenFile);
    inputFile.open(argv[1]);
    
    if(!logF.is_open() || !outF.is_open() || !inputFile.is_open())
        return 0;
    
    keywordTable = new SymbolTable(BUCKET_COUNT, SILENT);
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

bool isKeyword(const string& str)
{
    return keywordTable->lookUp(str);
}

void log(int lineNo, const string& token, const string& lexeme)
{
    logF << "Line No. " << lineNo << ": Token "  << token <<  " Lexeme "  << lexeme << " found.\n";
}

bool isFloat(const string& str) 
{
    //FLOAT = ({DIGIT})*\.({DIGIT})+(E[+-]?{DIGIT}+)? 

    int i = 0;
    int n = str.size();

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

string getParenType(const string& str)
{
    char ch = str[0];

    switch(ch)
    {
        case '(' : return "LPAREN"; 
        case ')' : return "RPAREN";
        case '{' : return "LCURL";
        case '}' : return "RCURL";
        case '[' : return "LTHIRD";
        case ']' : return "RTHIRD";
        case ',' : return "COMMA";
        case ';' : return "SEMICOLON";
        default : return "UNKNOWN PARENTHESES";
    }
}

string getOPType(const string& str)
{
    if(str == "+")
        return "ADDOP";
    if(str == "-")
        return "ADDOP";
    if(str == "*")
        return "MULOP";
    if(str == "/")
        return "MULOP";
    if(str == "%")
        return "MULOP";
    if(str == "++")
        return "INCOP";
    if(str == "--")
        return "INCOP";
    if(str == "<")
        return "RELOP";
    if(str == "<=")
        return "RELOP";
    if(str == ">")
        return "RELOP";
    if(str == ">=")
        return "RELOP";
    if(str == "==")
        return "RELOP";
    if(str == "!=")
        return "RELOP";
    if(str == "=")
        return "ASSIGNOP";
    if(str == "&&")
        return "LOGICOP";
    if(str == "||")
        return "LOGICOP";
    if(str == "!")
        return "NOT";

    return "UNKNOWN OPERATION";    
}

string processString(const string& str)
{
    string type = "";

    //exclude the beginning and closing \"s
    for(int i =1; i < str.size() - 1; ++i)
    {
        if(str[i] != '\\')
            type += str[i];
        else
        {
            ++i;

            if(i < str.size())
            {
                switch(str[i])
                {
                    case 'n': type += (unsigned char)'\n'; break;
                    case 't': type += (unsigned char)'\t'; break;
                    case '\\': type += (unsigned char)'\\'; break;
                    case '\'': type += (unsigned char)'\''; break;
                    case 'a': type += (unsigned char)'\a'; break;
                    case 'f': type += (unsigned char)'\f'; break;
                    case 'r': type += (unsigned char)'\r'; break;
                    case 'b': type += (unsigned char)'\b'; break;
                    case 'v': type += (unsigned char)'\v'; break;
                    case '0': type += (unsigned char)'\0'; break;
                    case '"': type += (unsigned char) '\"'; break;
                    case '\n': break; //this is the valid \\\n case not the \n one
                    default: type += string("\\") + str[i];
                }
            }
        }
    }

    return type;
}