#ifndef SCOPE_TABLE
#define SCOPE_TABLE 

#include <string>
#include <cstdio>
#include <iostream>
#include "SymbolInfo.h"

using namespace std;

int unique_id_counter = 1;

class ScopeTable
{
private:
    ScopeTable* parentScope = nullptr;
    SymbolInfo** hashTable = nullptr;
    const int unique_id = unique_id_counter++;
    int bucketNumber;
    bool silent = false;
public:
    ScopeTable(int n, bool silent = false)
    {
        bucketNumber = n;
        hashTable = new SymbolInfo*[bucketNumber]{};
        this->silent = silent;
        if(!silent) cout << "   " << "ScopeTable# " << unique_id << " created\n";
    }
    
    ~ScopeTable()
    {
        for(int i = 0; i < bucketNumber; ++i)
        {
            SymbolInfo* head = hashTable[i];
            while(head != nullptr)
            {
                SymbolInfo* prev = head;
                head = head->getNext();
                delete prev;
            }
        }

        delete[] hashTable;

        if(!silent) cout << "   " << "ScopeTable# " << unique_id << " removed\n";
    }

    unsigned int SDBMHash(const string&  str)
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

    SymbolInfo* lookUpSymbol(const string&  symbolName)
    {
        int i = 1;
        int hashBucket = SDBMHash(symbolName);
        SymbolInfo* head = hashTable[hashBucket];
        while(head != nullptr && head->getSymbolName() != symbolName)
        {
            head = head->getNext();
            ++i;
        }
        
        if(head != nullptr && !silent) if(!silent) cout << "   " << "'" << symbolName << "' found in ScopeTable# " << unique_id << " at position " << hashBucket + 1 << ", " << i << '\n'; 
        return head;
    }

    bool insertSymbol(const string&  symbolName, string symbolType)
    {
        int i = 1;
        if(symbolName.empty() || this->lookUpSymbol(symbolName) != nullptr)
        {
            if(!silent) cout<< "    " << "Insertion Failed :" << (symbolName.empty() ? "Empty SymbolName" : "SymbolName already exists") <<"\n";
            return false;
        }
        if(!silent) cout << "   " << "Inserted in ScopeTable# "<<unique_id<<" at position ";
        int hashBucket = SDBMHash(symbolName);
        SymbolInfo* head = hashTable[hashBucket];

        while(head != nullptr && head->getNext() != nullptr)
        {
            head = head->getNext();
            ++i;
        }

        if(!silent) cout << hashBucket + 1 << ", " << (head == nullptr ? i : i + 1) << "\n";

        if(head == nullptr)
        {
            head = new SymbolInfo(symbolName, symbolType, silent);
            hashTable[hashBucket] = head;
        }
        else
            head->setNext(new SymbolInfo(symbolName, symbolType, silent));
        
        return true; 
    }

    bool deleteSymbol(const string&  symbolName, string symbolType = "")
    {
        if(symbolName.empty() || this->lookUpSymbol(symbolName) == nullptr)
        {
            if(!silent) cout << "   " << "Not found in the current ScopeTable\n";
            return false;
        }

        int hashBucket = SDBMHash(symbolName);
        int i = 1;
        SymbolInfo* head = hashTable[hashBucket];
        SymbolInfo* next = head->getNext(); // since symbol already exists head wont be null
        
        if(head->getSymbolName() == symbolName)
        {
            hashTable[hashBucket] = next;
            delete head;
            if(!silent) cout << "   " << "Deleted " << "'" << symbolName << "' from ScopeTable# " << unique_id << " at position " << hashBucket + 1 << ", " << i << '\n';
            return true;
        }

        while(next != nullptr && next->getSymbolName() != symbolName)
        {
            head = head->getNext();
            next = next->getNext();
            ++i;
        }

        if(next != nullptr)
        {
            head->setNext(next->getNext());
            delete next;
        }

        if(!silent) cout << "   " <<  "Deleted " << "'" << symbolName << "' from ScopeTable# " << unique_id << " at position " << hashBucket + 1 << ", " << i + 1 << '\n';
        return true;
    }

    void print(const string&  spacing = "   ")
    {
        if(!silent) cout << spacing << "ScopeTable# " << unique_id << "\n";
        for(int i =0; i < bucketNumber; ++i)
        {
            SymbolInfo* head = hashTable[i];
            if(!silent) cout << spacing <<  i + 1 << "--> ";
            while(head != nullptr)
            {
                head->print();
                head = head->getNext();
            }
            
            if(!silent) cout << '\n';
        }
    }

    void setParentScope(ScopeTable* parentScope)
    {
        this->parentScope = parentScope;
    }

    ScopeTable* getParentScope() { return parentScope; }
    int getBucketNumber() { return bucketNumber; }
};

#endif