#!/bin/bash

# Requires at least Grammar file and Start Symbol
if (( $# < 2 || $# > 3 )); then
    echo "Usage: $0 <GrammarFile.g4> <start_symbol> [input_file.txt]"
    echo "Example: $0 CSubset.g4 program input.txt"
    exit 1
fi

GRAMMAR="$1"
START_SYMBOL="$2"
INPUT_FILE="$3"

if [ -n "$INPUT_FILE" ]; then
    # Pass file content directly into antlr4-parse
    antlr4-parse -v 4.13.2 "$GRAMMAR" "$START_SYMBOL" -gui < "$INPUT_FILE"
else
    echo "Type your input code in the terminal and press Ctrl+D when finished:"
    antlr4-parse -v 4.13.2 "$GRAMMAR" "$START_SYMBOL" -gui
fi