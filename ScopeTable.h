#pragma once
#include <string>
#include <cstdio>
#include <iostream>
#include "SymbolInfo.h"

using namespace std;

int unique_id_counter = 0;

class ScopeTable
{
private:
    ScopeTable* parentScope;
    SymbolInfo** hashTable;
    const int unique_id = unique_id_counter++;
    int bucketNumber;
public:
    ScopeTable(int n)
    {
        hashTable = new SymbolInfo*[n];
    }
    
    ~ScopeTable()
    {
        for(int i = 0; i < bucketNumber; ++i)
        {
            SymbolInfo* head = hashTable[i];
            while(head != nullptr)
            {
                SymbolInfo* prev = head;
                delete prev;
                head = head->getNext();
            }
        }

        delete[] hashTable;
    }

    unsigned int SDBMHash(string str)
    {
        unsigned int hash = 0;
        unsigned int i = 0;
        unsigned int len = str.length();

        for (i = 0; i < len; i++)
        {
            hash = ((str[i]) + (hash << 6) + (hash << 16) - hash) % bucketNumber;
        }

        return hash;
    }

    SymbolInfo* lookUpSymbol(string symbolName)
    {
        int hashBucket = SDBMHash(symbolName);
        SymbolInfo* head = hashTable[hashBucket];

        while(head != nullptr && head->getSymbolName() != symbolName)
            head = head->getNext();

        return head;
    }

    bool insertSymbol(string symbolName, string symbolType, SymbolInfo* next)
    { 
        if(symbolName.empty() || this->lookUpSymbol(symbolName) != nullptr)
            return false;
        
        int hashBucket = SDBMHash(symbolName);
        SymbolInfo* head = hashTable[hashBucket];

        while(head != nullptr && head->getNext() != nullptr)
            head = head->getNext();

        if(head == nullptr)
            head = new SymbolInfo(symbolName, symbolType);
        else 
            head->setNext(new SymbolInfo(symbolName, symbolType));

        return true; 
    }

    bool deleteSymbol(string symbolName)
    {
        if(symbolName.empty() || this->lookUpSymbol(symbolName) == nullptr)
            return false;

        int hashBucket = SDBMHash(symbolName);
        SymbolInfo* head = hashTable[hashBucket];
        SymbolInfo* next = head->getNext(); // since symbol already exists head wont be null

        while(next != nullptr && next->getSymbolName() != symbolName)
        {
            head = head->getNext();
            next = next->getNext();
        }

        if(next != nullptr)
        {
            head->setNext(next->getNext());
            delete next;
        }
        else
            delete head;
        
        return true;
    }

    void print()
    {
        cerr<<"Printing scope table:  \n";
        for(int i =0; i < bucketNumber; ++i)
        {
            SymbolInfo* head = hashTable[i];
            while(head != nullptr)
            {
                head->print();
                head = head->getNext();
            }
            
            cerr<<endl;
        }

        cerr<<endl;
    }

    void setParentScope(ScopeTable* parentScope)
    {
        this->parentScope = parentScope;
    }

    ScopeTable* getParentScope() { return parentScope; }
};