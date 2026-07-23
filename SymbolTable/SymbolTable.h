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
public:
    SymbolTable(int n)
    {
        bucketNumber = n;
        currentScopeTable = new ScopeTable(bucketNumber);
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
        currentScopeTable = new ScopeTable(bucketNumber);
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

    bool insertSymbol(const string&  symbolName, string symbolType)
    {
        return currentScopeTable->insertSymbol(symbolName, symbolType);
    }

    bool deleteSymbol(const string&  symbolName, string symbolType = "")
    {
        return currentScopeTable->deleteSymbol(symbolName, symbolType);
    }

    SymbolInfo *lookUp(const string&  symbolName, bool silent = false)
    {
        ScopeTable *head = currentScopeTable;

        while (head != nullptr)
        {
            SymbolInfo *symbol = head->lookUpSymbol(symbolName, silent);
            if (symbol != nullptr)
                return symbol;

            head = head->getParentScope();
        }

        if(!silent) cout << "   " << "'" << symbolName << "' not found in any of the ScopeTables\n";
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
        string spacing = "  ";
        while (head != nullptr)
        {
            head->print(spacing);
            head = head->getParentScope();
            spacing += "    ";
        }
    }
};

#endif