#ifndef SYMBOL_INFO
#define SYMBOL_INFO

#include <string>
#include <cstdio>

using namespace std;

enum class BaseType { INT, FLOAT, VOID, UNKNOWN };

struct TypeInfo {
    BaseType base = BaseType::UNKNOWN;
    bool isArray = false;
    int arraySize = 0;   

    bool operator==(const TypeInfo& o) const {
        return base == o.base && isArray == o.isArray;
    }
};

struct FunctionInfo {
    TypeInfo returnType;
    vector<TypeInfo> paramTypes;
    bool isDefined = false; 
};

class SymbolInfo
{
private:
    string symbolName, symbolType;
    SymbolInfo *next = nullptr;
    bool silent = false;
    FunctionInfo fInfo;
    TypeInfo tInfo;
    bool isFunc = false;
public:
    SymbolInfo() {}
    SymbolInfo(const string&  symbolName, string symbolType, bool silent = false, FunctionInfo fI)
    : fInfo(fI)
    {
        this->symbolName = symbolName;
        this->symbolType = symbolType;
        this->silent = silent;
        isFunc = true;
    }

    SymbolInfo(const string&  symbolName, string symbolType, bool silent = false, TypeInfo tI)
    : tInfo(tI)
    {
        this->symbolName = symbolName;
        this->symbolType = symbolType;
        this->silent = silent;
    }

    void print(ostream& out = cout) { out << "<" + symbolName + "," + symbolType + "> ";  }

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