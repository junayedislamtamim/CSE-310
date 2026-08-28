#ifndef SYMBOL_INFO
#define SYMBOL_INFO

#include <string>
#include <cstdio>
#include <vector>

using namespace std;

enum class BaseType { INT, FLOAT, VOID, UNKNOWN };

struct TypeInfo {
    BaseType base = BaseType::UNKNOWN;
    bool isArray = false;
    int arraySize = 0; 
    int stackOffset = 0;  

    bool operator==(const TypeInfo& o) const {
        return base == o.base && isArray == o.isArray;
    }
};

struct FunctionInfo {
    TypeInfo returnType;
    vector<TypeInfo> paramTypes;
    bool isDefined = false; 

    FunctionInfo(TypeInfo rt, vector<TypeInfo> pt, bool def)
        : returnType(std::move(rt)), paramTypes(std::move(pt)), isDefined(def) {}
};

class SymbolInfo
{
private:
    string symbolName, symbolType;
    SymbolInfo *next = nullptr;
    bool silent = false;
    shared_ptr<FunctionInfo> fInfo;
    TypeInfo tInfo;
    bool isFunc = false;
public:
    SymbolInfo() {}
    SymbolInfo(const string&  symbolName, string symbolType,  shared_ptr<FunctionInfo> fI, bool silent = false)
    : fInfo(fI)
    {
        this->symbolName = symbolName;
        this->symbolType = symbolType;
        this->silent = silent;
        isFunc = true;
    }

    SymbolInfo(const string&  symbolName, string symbolType, TypeInfo tI, bool silent = false)
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
    bool isDefinedFunction() { return isFunc && fInfo != nullptr && fInfo->isDefined; }
    bool isFunction() { return isFunc; }
    void defineFunction() {
        if(fInfo != nullptr)
            fInfo->isDefined = true;
    }
    TypeInfo getVarType() { return tInfo; }
    shared_ptr<FunctionInfo> getFuncInfo() { return fInfo; }
};

#endif