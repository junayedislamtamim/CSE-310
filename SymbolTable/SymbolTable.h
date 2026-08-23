#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include <string>
#include "ScopeTable.h"

class SymbolTable
{
private:
    ScopeTable *currentScopeTable = nullptr;
    int counter = 0;
    int bucketNumber;
    bool silent = false;
public:
    SymbolTable(int n, bool silent = false)
    {
        bucketNumber = n;
        currentScopeTable = new ScopeTable(bucketNumber, nullptr, silent);
        this->silent = silent;
    }

    ~SymbolTable()
    {
        ScopeTable *head = currentScopeTable;
        while (head != nullptr)
        {
            ScopeTable *temp = head;
            head = head->getParentScope();
            delete temp;
        }
    }

    void enterScope()
    {
        ScopeTable *prevScopeTable = currentScopeTable;
        currentScopeTable = new ScopeTable(bucketNumber, prevScopeTable, silent);
    }

    void exitScope()
    {
        if (currentScopeTable == nullptr)
            return;

        ScopeTable *temp = currentScopeTable;
        currentScopeTable = currentScopeTable->getParentScope();
        delete temp;
    }

    bool insertSymbol(const string&  symbolName, string symbolType, const TypeInfo& typeInfo)
    {
        return currentScopeTable->insertSymbol(symbolName, symbolType, typeInfo);
    }

    bool insertSymbol(const string&  symbolName, string symbolType, shared_ptr<FunctionInfo> functionInfo)
    {
        return currentScopeTable->insertSymbol(symbolName, symbolType, functionInfo);
    }

    bool deleteSymbol(const string&  symbolName, string symbolType = "")
    {
        return currentScopeTable->deleteSymbol(symbolName, symbolType);
    }

    SymbolInfo *lookUp(const string&  symbolName)
    {
        ScopeTable *head = currentScopeTable;

        while (head != nullptr)
        {
            SymbolInfo *symbol = head->lookUpSymbol(symbolName);
            if (symbol != nullptr)
                return symbol;

            head = head->getParentScope();
        }

        if(!silent) cout << "   " << "'" << symbolName << "' not found in any of the ScopeTables\n";
        return nullptr;
    }

    void printCurrentScopeTable(ostream& out = cout)
    {
        if (currentScopeTable == nullptr)
            return;

        currentScopeTable->print(out);
    }

    void printAllScopeTables(ostream& out = cout)
    {
        ScopeTable *head = currentScopeTable;
        string spacing = "  ";
        while (head != nullptr)
        {
            head->print(out, spacing);
            head = head->getParentScope();
            spacing += "    ";
        }

        out << '\n';
    }
};

#endif