#!/bin/bash

# Check if a grammar file was provided
if [ -z "$1" ]; then
    echo "Usage: $0 <GrammarFile.g4> [input_file.txt]"
    exit 1
fi

GRAMMAR_FILE="$1"
INPUT_FILE="$2"

# 1. Generate parser C++ files from the target grammar
antlr4 -v 4.13.2 -Dlanguage=Cpp -visitor -no-listener "$GRAMMAR_FILE"

# 2. Compile all .cpp files
g++ -std=c++17 -w -I/usr/local/include/antlr4-runtime *.cpp -L/usr/local/lib/ -lantlr4-runtime -pthread -o compiler.out

# 3. Run the compiled executable with optional input file
if [ -n "$INPUT_FILE" ]; then
    LD_LIBRARY_PATH=/usr/local/lib ./compiler.out "$INPUT_FILE"
else
    LD_LIBRARY_PATH=/usr/local/lib ./compiler.out
fi