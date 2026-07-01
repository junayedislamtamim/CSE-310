#pragma once
#include <string>
#include "SymbolInfo.h"

using namespace std;

const int BUCKET_NUMBER = 10000;

class ScopeTable
{
private:
    ScopeTable* parentScope;
    SymbolInfo** hashTable = new SymbolInfo*[BUCKET_NUMBER]; 
public:
    static unsigned int SDBMHash(string str, unsigned int num_buckets = BUCKET_NUMBER)
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

    SymbolInfo* lookUp(string symbolName)
    {
        int hashBucket = SDBMHash(symbolName);
        SymbolInfo* head = hashTable[hashBucket];

        while(head != NULL)
        {
            if(head->getSymbolName() == symbolName)
                return head;

            head = head->getNext();
        }

        return NULL;
    }

    bool insert(SymbolInfo* symbol)
    {
        if(this->lookUp(symbol->getSymbolName()) != NULL)
            return false;
        
        int hashBucket = SDBMHash(symbol -> getSymbolName());
        SymbolInfo* head = hashTable[hashBucket];

        while(head != NULL && head->getNext() != NULL)
        {
            head = head->getNext();
        }

        head->setNext(symbol);
        return true;
    }

    void setParentScope(ScopeTable* parentScope)
    {
        this->parentScope = parentScope;
    }

    ScopeTable* getParentScope() { return parentScope; }
};