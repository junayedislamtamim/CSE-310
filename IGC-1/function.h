#ifndef FUNCTION
#define FUNCTION

#include <fstream>
#include "../SymbolTable/SymbolTable.h"

string spacing = "    ";

void init(ofstream &out, stringstream &data, stringstream &code)
{
    out << "format ELF executable 3\n";
    out << "entry main\n";
    out << "\n";

    out << "segment readable writeable\n";
    out << data.str();
    out << "\n\n";

    out << "segment readable executable\n";
    out << "main: \n";
    out << code.str();
    out << "\n";

    out << "    MOV EAX, 1\n";
    out << "    XOR EBX, EBX\n";
    out << "    INT 0x80\n";
    out << "\n";
}

void insertData(stringstream &ss, pair<string, TypeInfo> a)
{
    int size = 1;
    if (a.second.isArray == true)
    {
        size = a.second.arraySize;
    }
    
    if (a.second.base == BaseType::INT || a.second.base == BaseType::FLOAT)
    {
        string initValue = (a.second.base == BaseType::FLOAT? "0.0" : "0");
        ss << "    " << a.first << "\tdd " << size << " dup (" << initValue << ")\n";
    }
}

void beginFunc(stringstream &ss, pair<string, shared_ptr<FunctionInfo>> a, int variableCount)
{
    //if the function has no params
    if(a.second->paramTypes.size() == 0)
    {
        ss << a.first << ":\n"; //print the label
        ss << "    " << "PUSH " << "EBP\n";
        ss << spacing << "MOV EBP, ESP\n";
        ss << spacing << "SUB ESP, " << variableCount * 4 << "\n";
    }
}

void endFunc(stringstream &ss, pair<string, shared_ptr<FunctionInfo>> a, int variableCount)
{
    //function has no parameters
    if(a.second->paramTypes.size() == 0)
    {
        ss << a.first << "_exit:\n";
        ss << spacing << "ADD ESP, " << variableCount * 4 << "\n"; // INT and FLOAT both are 4 bytes
        ss << "    POP EBP\n";
        ss << "    RET\n";
    }
}

#endif