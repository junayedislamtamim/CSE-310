#include <iostream>
#include <sstream>
#include <string>
#include <limits>
#include "SymbolTable.h"

using namespace std;

string formatSymbolType(string rest)
{
    stringstream ss(rest);
    string token, symbolType;
    ss >> token;
    symbolType += token;
    symbolType.reserve(250);

    /*
        GENERATES A FORMAT LIKE THIS:
        FUNCTION,RETURN_TYPE<==(argument1_type,argument2_type,...,arguementN_type)
        FROM THIS:
        FUNCTION INT INT FLOAT INT
    */
    if (token == "FUNCTION")
    {
        symbolType += ",";
        ss >> token;
        symbolType += token + "<==(";

        if(ss >> token)
            symbolType += token;
        
        while (ss >> token)
        {
            symbolType += "," + token;
        }

        symbolType += ")";
    }
    /*
        GENERATES A FORMAT LIKE THIS:
        STRUCT,{(INT,a),(FLOAT,b)}
        FROM THIS:
        foo STRUCT INT a FLOAT b
    */
    else if (token == "STRUCT" || token == "UNION")
    {
        symbolType += ",";
        symbolType += "{";

        if(ss >> token)
        {
            symbolType += "(" + token + ",";
            ss >> token;
            symbolType += token + ")";
        }

        while(ss >> token)
        {
            symbolType += ",(" + token + ",";
            ss >> token;
            symbolType += token + ")";
        }

        symbolType += "}";
    }

    return symbolType;
}

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        cerr<<"Wrong command line arguements\n"<<"Enter in the following format:\n<program> <inputfile> <outputfile>\n";
        return 1;
    }

    freopen(argv[1], "r", stdin);
    freopen(argv[2], "w", stdout);

    int n, i = 1;
    cin >> n;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    SymbolTable *symbolTable = new SymbolTable(n);
    string inputLine, token, extra;

    while (getline(cin, inputLine))
    {
        cout << "Cmd " << i++ << ": " << inputLine << '\n';
        stringstream ss = stringstream(inputLine);
        if (ss >> token)
        {
            string symbolName, symbolType;
            if (token == "I")
            {
                ss >> symbolName;
                getline(ss, inputLine);
                symbolType = formatSymbolType(inputLine);
                if(!symbolName.empty() && !symbolType.empty())
                    symbolTable->insertSymbol(symbolName, symbolType);
                else
                    cout << "   " << "Number of parameters mismatch for the command I\n";
            }
            else if (token == "L")
            {
                ss >> symbolName;
                if(!(ss >> extra) && !symbolName.empty())
                    symbolTable->lookUp(symbolName);
                else
                    cout << "   " << "Number of parameters mismatch for the command L\n";
            }
            else if (token == "D")
            {
                ss >> symbolName;
                if(!(ss >> extra) && !symbolName.empty())
                    symbolTable->deleteSymbol(symbolName);
                else
                    cout << "   " << "Number of parameters mismatch for the command D\n";
            }
            else if (token == "P")
            {
                ss >> token;
                if(!(ss >> extra) && !token.empty())
                {
                    if (token == "C")
                        symbolTable->printCurrentScopeTable();
                    else if (token == "A")
                        symbolTable->printAllScopeTables();
                }
                else 
                    cout << "   " << "Number of parameters mismatch for the command P\n";
            }
            else if (token == "S")
            {
                if(!(ss >> extra))
                    symbolTable->enterScope();
                else
                    cout << "   " << "Number of parameters mismatch for the command S\n";
            }
            else if (token == "E")
            {
                if(!(ss >> extra))
                    symbolTable->exitScope();
                else
                   cout << "   " << "Number of parameters mismatch for the command E\n"; 
            }
            else if (token == "Q")
            {
                if(!( ss >> extra))
                {
                    delete symbolTable;
                    break;
                }
                else
                    cout << "   " << "Number of parameters mismatch for the command Q\n";
            }
        }
    }
}