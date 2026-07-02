#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include <string>
#include "ScopeTable.h"

class SymbolTable
{
private:
    ScopeTable *currentScopeTable = nullptr;

public:
    void enterScope()
    {
        ScopeTable *prevScopeTable = currentScopeTable;
        currentScopeTable = new ScopeTable(prevScopeTable->getBucketNumber());
        currentScopeTable->setParentScope(prevScopeTable);
    }

    void exitScope()
    {
        if (currentScopeTable == nullptr)
            return;

        ScopeTable *temp = currentScopeTable;
        currentScopeTable = currentScopeTable->getParentScope();
        delete temp;
    }

    bool insertSymbol(string symbolName, string symbolType)
    {
        return currentScopeTable->insertSymbol(symbolName, symbolType);
    }

    bool deleteSymbol(string symbolName, string symbolType = "")
    {
        return currentScopeTable->deleteSymbol(symbolName, symbolType);
    }

    SymbolInfo *lookUp(string symbolName)
    {
        ScopeTable *head = currentScopeTable;

        while (head != nullptr)
        {
            SymbolInfo *symbol = head->lookUpSymbol(symbolName);
            if (symbol != nullptr)
                return symbol;

            head = head->getParentScope();
        }

        return nullptr;
    }

    void printCurrentScopeTable()
    {
        if (currentScopeTable == nullptr)
            return;

        currentScopeTable->print();
    }

    void printAllScopeTables()
    {
        ScopeTable *head = currentScopeTable;

        while (head != nullptr)
        {
            head->print();
            head = head->getParentScope();
        }
    }
};

#endif