#ifndef SYMBOL_INFO
#define SYMBOL_INFO

#include <string>
#include <cstdio>

using namespace std;

class SymbolInfo
{
private:
    string symbolName, symbolType;
    SymbolInfo *next = nullptr;

public:
    SymbolInfo() {}
    SymbolInfo(string symbolName, string symbolType)
    {
        this->symbolName = symbolName;
        this->symbolType = symbolType;
    }

    void print() { cout<<"<" + symbolName + "," + symbolType + "> ";  }

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

#endif