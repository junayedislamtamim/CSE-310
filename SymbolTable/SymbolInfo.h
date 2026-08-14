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
    bool silent = false;
public:
    SymbolInfo() {}
    SymbolInfo(const string&  symbolName, string symbolType, bool silent = false)
    {
        this->symbolName = symbolName;
        this->symbolType = symbolType;
        this->silent = silent;
    }

    void print(ostream& out = cout) { if(!silent) out << "<" + symbolName + "," + symbolType + "> ";  }

    void setSymbolName(const string&  symbolName)
    {
        this->symbolName = symbolName;
    }

    void setSymbolType(const string&  symbolType)
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