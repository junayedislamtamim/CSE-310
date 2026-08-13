grammar CSubset;
import Lexer;

start : program ;

program
    : program unit #program_program_unit
    | unit  #program_unit
    ;

unit
    : var_declaration #unit_var_declaration
    | func_declaration  #unit_func_declaration
    | func_definition   #unit_func_definition
    ;

func_declaration
    : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON #func_declaration_param
    | type_specifier ID LPAREN RPAREN SEMICOLON #func_declaration_no_param
    ;

func_definition
    : type_specifier ID LPAREN parameter_list RPAREN compound_statement #func_definition_param
    | type_specifier ID LPAREN RPAREN compound_statement #func_definition_no_param
    ;

parameter_list
    : parameter_list COMMA type_specifier ID #paramlist_comma_typespec_id
    | parameter_list COMMA type_specifier   #paramlist_comma_typespec
    | type_specifier ID #paramlist_typespec_id
    | type_specifier    #paramList_typespec
    ;

compound_statement
    : LCURL statements RCURL #compound_statement_statements
    | LCURL RCURL   #compound_statement_nostatement
    ;   

var_declaration
    : type_specifier declaration_list SEMICOLON ;

type_specifier
    : INT
    | FLOAT
    | VOID
    ;

declaration_list
    : declaration_list COMMA ID #declaration_list_comma
    | declaration_list COMMA ID LTHIRD CONST_INT RTHIRD #declaration_list_comma_id_
    | ID    #declaration_list_id
    | ID LTHIRD CONST_INT RTHIRD #declaration_list_id_
    ;

statements
    : statement #statements_statement
    | statements statement #statements_statements
    ;

statement
    : var_declaration #statement_one
    | expression_statement #statement_two
    | compound_statement #statement_three
    | FOR LPAREN expression_statement expression_statement expression RPAREN statement #statement_four
    | IF LPAREN expression RPAREN statement #statement_five
    | IF LPAREN expression RPAREN statement ELSE statement #statement_six
    | WHILE LPAREN expression RPAREN statement #statement_seven
    | PRINTLN LPAREN ID RPAREN SEMICOLON #statement_eight
    | RETURN expression SEMICOLON #statement_nine
    ;

expression_statement
    : SEMICOLON #expression_stmt_semicolon
    | expression SEMICOLON #expression_stmt_expr_semicolon
    ;

variable
    : ID #variable_one
    | ID LTHIRD expression RTHIRD #variable_two
    ;

expression
    : logic_expression #expression_one
    | variable ASSIGNOP logic_expression #expression_two
    ;

logic_expression
    : rel_expression #logic_expression_one
    | rel_expression LOGICOP rel_expression #logic_expression_two
    ;

rel_expression
    : simple_expression #rel_expression_one
    | simple_expression RELOP simple_expression #rel_expression_two
    ;

simple_expression
    : term #simple_expression_one
    | simple_expression ADDOP term #simple_expression_two
    ;

term
    : unary_expression #term_one
    | term MULOP unary_expression #term_two
    ;

unary_expression
    : ADDOP unary_expression #unary_expression_one
    | NOT unary_expression #unary_expression_two
    | factor #unary_expression_three
    ;

factor
    : variable #factor_one
    | ID LPAREN argument_list RPAREN #factor_two
    | LPAREN expression RPAREN #factor_three
    | CONST_INT #factor_four
    | CONST_FLOAT #factor_five
    | variable INCOP #factor_six
    | variable DECOP #factor_seven
    ;

argument_list
    : arguments #argument_list_one
    | #argument_list_two
    ;

arguments
    : arguments COMMA logic_expression #arguments_two
    | logic_expression #arguments_one
     ;
