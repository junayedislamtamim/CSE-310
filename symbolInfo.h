#pragma once
#include <string>

using namespace std;

class SymbolInfo
{
private:
    string symbolName, symbolType;
    SymbolInfo *next;

public:
    SymbolInfo(string symbolName, string sybolType, SymbolInfo *next)
    {
        this->symbolName = symbolName;
        this->symbolType = sybolType;
        this->next = next;
    }

    void setSymbolName(string symbolNmae)
    {
        this->symbolName = symbolName;
    }

    void setSymbolType(string symbolType)
    {
        this->symbolType = symbolType;
    }

    void setNext(SymbolInfo *next)
    {
        this->next = next;
    }

    string getSymbolName() { return symbolName; }
    string getSymbolType() { return symbolType; }
    SymbolInfo* getNext() { return next; }
};