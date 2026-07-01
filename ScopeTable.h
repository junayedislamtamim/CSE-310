#pragma once
#include <string>
#include "SymbolInfo.h"

using namespace std;

class ScopeTable
{
private:
    ScopeTable* parentScope;
    SymbolInfo* hashTable = new SymbolInfo[1000]; 
public:
    static unsigned int SDBMHash(string str, unsigned int num_buckets)
    {
        unsigned int hash = 0;
        unsigned int i = 0;
        unsigned int len = str.length();

        for (i = 0; i < len; i++)
        {
            hash = ((str[i]) + (hash << 6) + (hash << 16) - hash) % num_buckets;
        }

        return hash;
    }

    bool insert()
    {

    }

    void setParentScope(ScopeTable* parentScope)
    {
        this->parentScope = parentScope;
    }

    ScopeTable* getParentScope() { return parentScope; }
};