#ifndef IMP
#define IMP
#include "CSubsetVisitor.h"
#include "../SymbolTable/SymbolTable.h"
#include "antlr4-runtime.h"

using namespace std;
using namespace antlr4;

class CSubsetVisitorIMP : public CSubsetVisitor
{
    SymbolTable &symbolTable;
    ofstream &logF;
    antlr4::CommonTokenStream *tokenStream;

public:
    CSubsetVisitorIMP(SymbolTable &st, ofstream &of, antlr4::CommonTokenStream *ts) : symbolTable(st), logF(of), tokenStream(ts) {}

    virtual std::any visitStart(CSubsetParser::StartContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "start", "program");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitProgram_unit(CSubsetParser::Program_unitContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "program", "unit");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitProgram_program_unit(CSubsetParser::Program_program_unitContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "program", "program unit");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitUnit_var_declaration(CSubsetParser::Unit_var_declarationContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "unit", "var_declaration");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitUnit_func_declaration(CSubsetParser::Unit_func_declarationContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "unit", "func_declaration");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitUnit_func_definition(CSubsetParser::Unit_func_definitionContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "unit", "func_definition");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitFunc_declaration_param(CSubsetParser::Func_declaration_paramContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "func_declaration", "type_specifier ID LPAREN parameter_list RPAREN SEMICOLON");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitFunc_declaration_no_param(CSubsetParser::Func_declaration_no_paramContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "func_declaration", "type_specifier ID LPAREN RPAREN SEMICOLON");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitFunc_definition_param(CSubsetParser::Func_definition_paramContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "func_definition", "type_specifier ID LPAREN parameter_list RPAREN SEMICOLON");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitFunc_definition_no_param(CSubsetParser::Func_definition_no_paramContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "func_declaration", "type_specifier ID LPAREN RPAREN SEMICOLON");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitParamlist_typespec_id(CSubsetParser::Paramlist_typespec_idContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "parameter_list", "parameter_list COMMA type_specifier ID");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitParamList_typespec(CSubsetParser::ParamList_typespecContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "parameter_list", "parameter_list COMMA type_specifier");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitParamlist_comma_typespec_id(CSubsetParser::Paramlist_comma_typespec_idContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "parameter_list", "type_specifier ID");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitParamlist_comma_typespec(CSubsetParser::Paramlist_comma_typespecContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "parameter_list", "type_specifier");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitCompound_statement_statements(CSubsetParser::Compound_statement_statementsContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "compound_statement", "LCURL statements RCURL");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitCompound_statement_nostatement(CSubsetParser::Compound_statement_nostatementContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "compound_statement", "LCURL RCURL");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitVar_declaration(CSubsetParser::Var_declarationContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        string type = ctx->type_specifier()->getText();
        stringstream ss(ctx->declaration_list()->getText());
        string id;
        string TYPE = to_upper(type);

        while (getline(ss, id, ','))
        {
            symbolTable.insertSymbol(id, TYPE);
        }

        log(ctx->getStart()->getLine(), "var_declaration", "type_specifier declaration_list SEMICOLON");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitType_specifier(CSubsetParser::Type_specifierContext *ctx) override
    {
        auto temp = visitChildren(ctx);
        string text = to_upper(ctx->getText());

        log(ctx->getStart()->getLine(), "type_specifier", "text");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitDeclaration_list_id(CSubsetParser::Declaration_list_idContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "declaration_list", "ID");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitDeclaration_list_id_(CSubsetParser::Declaration_list_id_Context *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "declaration_list", "ID LTHIRD CONST_INT RTHIRD #declaration_list_id_");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitDeclaration_list_comma(CSubsetParser::Declaration_list_commaContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "declaration_list", "declaration_list COMMA ID");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitDeclaration_list_comma_id_(CSubsetParser::Declaration_list_comma_id_Context *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "declaration_list", "declaration_list COMMA ID LTHIRD CONST_INT RTHIRD");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitStatements_statement(CSubsetParser::Statements_statementContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "statements", "statement");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitStatements_statements(CSubsetParser::Statements_statementsContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "statements", "statements statement");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitStatement_one(CSubsetParser::Statement_oneContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "statement", "var_declaration");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitStatement_two(CSubsetParser::Statement_twoContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "statement", "expression_statement");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitStatement_three(CSubsetParser::Statement_threeContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "statement", "compound_statement");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitStatement_four(CSubsetParser::Statement_fourContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "statement", "FOR LPAREN expression_statement expression_statement expression RPAREN statement");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitStatement_five(CSubsetParser::Statement_fiveContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "statement", "IF LPAREN expression RPAREN statement");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitStatement_six(CSubsetParser::Statement_sixContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "statement", "IF LPAREN expression RPAREN statement ELSE statement");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitStatement_seven(CSubsetParser::Statement_sevenContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "statement", "WHILE LPAREN expression RPAREN statement");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitStatement_eight(CSubsetParser::Statement_eightContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "statement", "PRINTLN LPAREN ID RPAREN SEMICOLON");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitStatement_nine(CSubsetParser::Statement_nineContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "statement", "RETURN expression SEMICOLON");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitExpression_stmt_semicolon(CSubsetParser::Expression_stmt_semicolonContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "expression_statement", "SEMICOLON");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitExpression_stmt_expr_semicolon(CSubsetParser::Expression_stmt_expr_semicolonContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "expression_statement", "expression SEMICOLON");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitVariable_one(CSubsetParser::Variable_oneContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "variable", "ID");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitVariable_two(CSubsetParser::Variable_twoContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "variable", "ID LTHIRD expression RTHIRD");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitExpression_one(CSubsetParser::Expression_oneContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "expression", "logic_expression");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitExpression_two(CSubsetParser::Expression_twoContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "expression", "variable ASSIGNOP logic_expression");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitLogic_expression(CSubsetParser::Logic_expressionContext *ctx) override
    {
        auto temp = visitChildren(ctx);
        return temp;
    }

    virtual std::any visitRel_expression(CSubsetParser::Rel_expressionContext *ctx) override
    {
        auto temp = visitChildren(ctx);
        return temp;
    }

    virtual std::any visitSimple_expression(CSubsetParser::Simple_expressionContext *ctx) override
    {
        auto temp = visitChildren(ctx);
        return temp;
    }

    virtual std::any visitTerm(CSubsetParser::TermContext *ctx) override
    {
        auto temp = visitChildren(ctx);
        return temp;
    }

    virtual std::any visitUnary_expression(CSubsetParser::Unary_expressionContext *ctx) override
    {
        auto temp = visitChildren(ctx);
        return temp;
    }

    virtual std::any visitFactor(CSubsetParser::FactorContext *ctx) override
    {
        auto temp = visitChildren(ctx);
        return temp;
    }

    virtual std::any visitArgument_list(CSubsetParser::Argument_listContext *ctx) override
    {
        auto temp = visitChildren(ctx);
        return temp;
    }

    virtual std::any visitArguments(CSubsetParser::ArgumentsContext *ctx) override
    {
        auto temp = visitChildren(ctx);
        return temp;
    }

    void log(int lineNo, const string &ruleName, const string &expansion)
    {
        logF << "Line " << lineNo << ": " << ruleName << ": " << expansion << "\n\n";
    }

    void log2(const string &str)
    {
        logF << str << "\n\n";
    }

    string to_upper(const string &str)
    {
        string temp;

        for (int i = 0; i < str.size(); ++i)
        {
            if (str[i] >= 'a' && str[i] <= 'z')
                temp.push_back(str[i] + ('A' - 'a'));
            else
                temp.push_back(str[i]);
        }

        return temp;
    }

    string getExactRuleText(ParserRuleContext *ctx, CommonTokenStream *tokenStream)
    {
        CharStream *input = tokenStream->getTokenSource()->getInputStream();

        misc::Interval interval(
            ctx->getStart()->getStartIndex(),
            ctx->getStop()->getStopIndex());

        return input->getText(interval);
    }
};

#endif