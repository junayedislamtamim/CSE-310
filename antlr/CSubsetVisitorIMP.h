#ifndef IMP
#define IMP
#include <vector>
#include <sstream>
#include "CSubsetVisitor.h"
#include "../SymbolTable/SymbolTable.h"
#include "antlr4-runtime.h"
#include "../IGC-1/function.h"

using namespace std;
using namespace antlr4;

class CSubsetVisitorIMP : public CSubsetVisitor
{
    SymbolTable &symbolTable;
    ofstream &logF;
    ofstream &errF;
    ofstream &outF;
    stringstream data;
    stringstream code;
    stringstream body;
    antlr4::CommonTokenStream *tokenStream;
    int errorCount = 0;
    int currentStackOffset = 0;
    int labelCounter = 0;

public:
    CSubsetVisitorIMP(SymbolTable &st, ofstream &of, ofstream &ef, ofstream &outf, antlr4::CommonTokenStream *ts) : symbolTable(st), logF(of), errF(ef), outF(outf), tokenStream(ts) {}

    virtual std::any visitStart(CSubsetParser::StartContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "start", "program");
        log2(getExactRuleText(ctx, tokenStream));

        symbolTable.printAllScopeTables(logF);
        symbolTable.exitScope();

        logF << "Total lines: " << ctx->getStop()->getLine() << '\n';
        logF << "Total errors: " << errorCount << '\n';

        init(outF, data, code);

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
        auto funcDefCtx = ctx->func_definition();
        string funcName;
        TypeInfo returnType;
        vector<TypeInfo> paramTypes;

        if (auto p = dynamic_cast<CSubsetParser::Func_definition_paramContext *>(funcDefCtx))
        {
            funcName = p->ID()->getText();
            returnType = makeType(getType(p->type_specifier()->getText()), false, 0);
            paramTypes = extractParamTypes(p->parameter_list());
        }
        else if (auto np = dynamic_cast<CSubsetParser::Func_definition_no_paramContext *>(funcDefCtx))
        {
            funcName = np->ID()->getText();
            returnType = makeType(getType(np->type_specifier()->getText()), false, 0);
        }

        SymbolInfo *prev = symbolTable.lookUp(funcName);
        if (prev && prev->isFunction())
        {
            auto prevFi = prev->getFuncInfo();

            if (prevFi->paramTypes.size() != paramTypes.size())
            {
                errF << "Error at line " << ctx->getStart()->getLine()
                     << ": Total number of arguments mismatch with declaration in function " << funcName << "\n\n";
                errorCount++;
            }

            if (prevFi->returnType.base != returnType.base)
            {
                errF << "Error at line " << ctx->getStart()->getLine()
                     << ": Return type mismatch with function declaration in function " << funcName << "\n\n";
                errorCount++;
            }
        }
        if (!symbolTable.insertSymbol(funcName, "ID", make_shared<FunctionInfo>(returnType, paramTypes, true)))
        {
            errF << "Error at line " << ctx->getStart()->getLine()
                 << ": Multiple declaration of " << funcName << "\n\n";
        }

        currentStackOffset = 0;
        symbolTable.enterScope();

        auto temp = visitChildren(ctx);

        beginFunc(code, {funcName, make_shared<FunctionInfo>(returnType, paramTypes, true)}, symbolTable.getVariableCount());
        code << body.str();
        endFunc(code, {funcName, make_shared<FunctionInfo>(returnType, paramTypes, true)}, symbolTable.getVariableCount());

        body.str("");
        body.clear();

        int pos = 0;
        if (auto p = dynamic_cast<CSubsetParser::Func_definition_paramContext *>(funcDefCtx))
            checkParamNames(p->parameter_list(), funcName, pos);

        log(ctx->getStart()->getLine(), "unit", "func_definition");
        log2(getExactRuleText(ctx, tokenStream));

        symbolTable.printAllScopeTables(logF);
        symbolTable.exitScope();
        currentStackOffset = 0;

        return temp;
    }

    virtual std::any visitFunc_declaration_param(CSubsetParser::Func_declaration_paramContext *ctx) override
    {
        string type = "ID";
        string ID = ctx->ID()->getText();
        TypeInfo returnType;
        vector<TypeInfo> paramTypes;

        returnType = makeType(getType(ctx->type_specifier()->getText()), false, 0);
        paramTypes = extractParamTypes(ctx->parameter_list());

        if (!symbolTable.insertSymbol(ID, "ID", make_shared<FunctionInfo>(returnType, paramTypes, false)))
        {
            errF << "Error at line " << ctx->getStart()->getLine()
                 << ": Multiple declaration of " << ID << "\n\n";
        }

        symbolTable.enterScope();

        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "func_declaration", "type_specifier ID LPAREN parameter_list RPAREN SEMICOLON");
        log2(getExactRuleText(ctx, tokenStream));

        symbolTable.exitScope();
        return temp;
    }

    virtual std::any visitFunc_declaration_no_param(CSubsetParser::Func_declaration_no_paramContext *ctx) override
    {
        string type = "ID";
        string ID = ctx->ID()->getText();
        TypeInfo returnType;
        vector<TypeInfo> paramTypes;

        returnType = makeType(getType(ctx->type_specifier()->getText()), false, 0);
        if (!symbolTable.insertSymbol(ID, "ID", make_shared<FunctionInfo>(returnType, paramTypes, false)))
        {
            errF << "Error at line " << ctx->getStart()->getLine()
                 << ": Multiple declaration of " << ID << "\n\n";
        }

        symbolTable.enterScope();

        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "func_declaration", "type_specifier ID LPAREN RPAREN SEMICOLON");
        log2(getExactRuleText(ctx, tokenStream));

        symbolTable.exitScope();
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

        string type = "ID";
        string ID = ctx->ID()->getText();

        if (!symbolTable.insertSymbol(ID, type, makeType(getType(ctx->type_specifier()->getText()), false, 0)))
        {
            errF << "Error at line " << ctx->getStart()->getLine()
                 << ": Multiple declaration of " << ID << " in parameter\n\n";
            errorCount++;
        }

        log(ctx->getStart()->getLine(), "parameter_list", "type_specifier ID");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitParamList_typespec(CSubsetParser::ParamList_typespecContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "parameter_list", "type_specifier");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitParamlist_comma_typespec_id(CSubsetParser::Paramlist_comma_typespec_idContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        string type = "ID";
        string ID = ctx->ID()->getText();

        if (!symbolTable.insertSymbol(ID, type, makeType(getType(ctx->type_specifier()->getText()), false, 0)))
        {
            errF << "Error at line " << ctx->getStart()->getLine()
                 << ": Multiple declaration of " << ID << " in parameter\n\n";
            errorCount++;
        }

        log(ctx->getStart()->getLine(), "parameter_list", "parameter_list COMMA type_specifier ID");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitParamlist_comma_typespec(CSubsetParser::Paramlist_comma_typespecContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "parameter_list", "parameter_list COMMA type_specifier");
        log2(getExactRuleText(ctx, tokenStream));

        return temp;
    }

    virtual std::any visitCompound_statement_statements(CSubsetParser::Compound_statement_statementsContext *ctx) override
    {
        bool isFunctionBody = dynamic_cast<CSubsetParser::Func_definition_paramContext *>(ctx->parent) != nullptr || dynamic_cast<CSubsetParser::Func_definition_no_paramContext *>(ctx->parent) != nullptr;

        int oldOffset;

        if (!isFunctionBody)
        {
            oldOffset = currentStackOffset;
            symbolTable.enterScope();
        }

        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "compound_statement", "LCURL statements RCURL");
        log2(getExactRuleText(ctx, tokenStream));

        if (!isFunctionBody)
        {
            currentStackOffset = oldOffset;
            symbolTable.exitScope();
        }

        return temp;
    }

    virtual std::any visitCompound_statement_nostatement(CSubsetParser::Compound_statement_nostatementContext *ctx) override
    {
        bool isFunctionBody = dynamic_cast<CSubsetParser::Func_definition_paramContext *>(ctx->parent) != nullptr || dynamic_cast<CSubsetParser::Func_definition_no_paramContext *>(ctx->parent) != nullptr;

        if (!isFunctionBody)
            symbolTable.enterScope();

        auto temp = visitChildren(ctx);

        log(ctx->getStart()->getLine(), "compound_statement", "LCURL RCURL");
        log2(getExactRuleText(ctx, tokenStream));

        if (!isFunctionBody)
            symbolTable.exitScope();

        return temp;
    }

    virtual std::any visitVar_declaration(CSubsetParser::Var_declarationContext *ctx) override
    {
        auto temp = visitChildren(ctx);

        BaseType base = getType(ctx->type_specifier()->getText());
        auto decls = extractDeclarations(ctx->declaration_list(), base);

        for (auto &[id, type] : decls)
        {
            if (!symbolTable.insertSymbol(id, "ID", type))
            {
                errF << "Error at line " << ctx->getStart()->getLine()
                     << ": Multiple declaration of " << id << "\n\n";
                errorCount++;
            }

            if (symbolTable.isGlobalScope())
            {
                insertData(data, {id, type});
            }
            else
            {
                // do nothing, handled inside extractDeclaration function
            }
        }

        log(ctx->getStart()->getLine(), "var_declaration", "type_specifier declaration_list SEMICOLON");
        log2(getExactRuleText(ctx, tokenStream));
        return temp;
    }

    virtual std::any visitType_specifier(CSubsetParser::Type_specifierContext *ctx) override
    {
        auto temp = visitChildren(ctx);
        string text = to_upper(ctx->getText());

        log(ctx->getStart()->getLine(), "type_specifier", text);
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

        string id = ctx->ID()->getText();
        SymbolInfo *sym = symbolTable.lookUp(id);
        if (!sym)
        {
            errF << "Error at line " << ctx->getStart()->getLine()
                 << ": Undeclared variable " << id << "\n\n";
            errorCount++;
        }

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
        string id = ctx->ID()->getText();
        TypeInfo result{BaseType::UNKNOWN, false, 0};
        SymbolInfo *sym = symbolTable.lookUp(id);

        if (!sym)
        {
            errF << "Error at line " << ctx->getStart()->getLine()
                 << ": Undeclared variable " << id << "\n\n";
            errorCount++;
        }
        else if (sym->isFunction())
        {
            errF << "Error at line " << ctx->getStart()->getLine() << ": " << id << " is a function, used as variable\n\n";
            errorCount++;
        }
        else
        {
            result = sym->getVarType();
            if (result.isArray)
            {
                errF << "Error at line " << ctx->getStart()->getLine() << ": Type mismatch, " << id << " is an array\n\n";
                errorCount++;
            }
        }

        log(ctx->getStart()->getLine(), "variable", "ID");
        log2(getExactRuleText(ctx, tokenStream));
        return result;
    }

    virtual std::any visitVariable_two(CSubsetParser::Variable_twoContext *ctx) override
    {
        string id = ctx->ID()->getText();
        TypeInfo result{BaseType::UNKNOWN, false, 0};
        SymbolInfo *sym = symbolTable.lookUp(id);

        auto indexType = std::any_cast<TypeInfo>(visit(ctx->expression()));
        if (indexType.base != BaseType::UNKNOWN && indexType.base != BaseType::INT)
        {
            errF << "Error at line " << ctx->getStart()->getLine()
                 << ": Expression inside third brackets not an integer\n\n";
            errorCount++;
        }

        if (!sym)
        {
            errF << "Error at line " << ctx->getStart()->getLine() << ": Undeclared variable " << id << "\n\n";
            errorCount++;
        }
        else if (sym->isFunction())
        {
            errF << "Error at line " << ctx->getStart()->getLine() << ": " << id << " is a function, used as variable\n\n";
            errorCount++;
        }
        else
        {
            TypeInfo varType = sym->getVarType();
            if (!varType.isArray)
            {
                errF << "Error at line " << ctx->getStart()->getLine()
                     << ": " << id << " is not an array\n\n";
                errorCount++;
            }
            result = makeType(varType.base, false, 0);
        }

        log(ctx->getStart()->getLine(), "variable", "ID LTHIRD expression RTHIRD");
        log2(getExactRuleText(ctx, tokenStream));
        return result;
    }

    virtual std::any visitExpression_one(CSubsetParser::Expression_oneContext *ctx) override
    {
        auto t = std::any_cast<TypeInfo>(visit(ctx->logic_expression()));

        log(ctx->getStart()->getLine(), "expression", "logic_expression");
        log2(getExactRuleText(ctx, tokenStream));

        return t;
    }

    virtual std::any visitExpression_two(CSubsetParser::Expression_twoContext *ctx) override
    {
        auto lhs = std::any_cast<TypeInfo>(visit(ctx->variable()));
        auto rhs = std::any_cast<TypeInfo>(visit(ctx->logic_expression()));

        if (lhs.base == BaseType::VOID || rhs.base == BaseType::VOID)
        {
            errF << "Error at line " << ctx->getStart()->getLine() << ": Void type in assignment\n\n";
            errorCount++;
        }
        else if (lhs.base == BaseType::INT && rhs.base == BaseType::FLOAT)
        {
            errF << "Error at line " << ctx->getStart()->getLine()
                 << ": Type Mismatch\n\n";
        }
        else if (lhs.base == BaseType::FLOAT && rhs.base == BaseType::INT)
        {
            // do nothing
        }
        else if (lhs.base != BaseType::UNKNOWN && rhs.base != BaseType::UNKNOWN && lhs.base != rhs.base)
        {
            errF << "Error at line " << ctx->getStart()->getLine() << ": Type mismatch\n\n";
            errorCount++;
        }

        log(ctx->getStart()->getLine(), "expression", "variable ASSIGNOP logic_expression");
        log2(getExactRuleText(ctx, tokenStream));

        printOP(body, "MOV", getName(ctx->variable()->getText(), lhs), "EAX" );

        return lhs;
    }

    virtual std::any visitLogic_expression_one(CSubsetParser::Logic_expression_oneContext *ctx) override
    {
        auto t = std::any_cast<TypeInfo>(visit(ctx->rel_expression()));

        log(ctx->getStart()->getLine(), "logic_expression", "rel_expression");
        log2(getExactRuleText(ctx, tokenStream));

        return t;
    }

    virtual std::any visitLogic_expression_two(CSubsetParser::Logic_expression_twoContext *ctx) override
    {
        visit(ctx->rel_expression(0));
        body << "   PUSH EAX\n"; 
        visit(ctx->rel_expression(1));
        body << "   POP EBX\n";

        if(ctx->LOGICOP()->getText() == "&&")
            logicAND(body, labelCounter, "EBX", "EAX");
        else
            logicOR(body, labelCounter, "EBX", "EAX");

        log(ctx->getStart()->getLine(), "logic_expression", "rel_expression LOGICOP rel_expression");
        log2(getExactRuleText(ctx, tokenStream));

        return makeType(BaseType::INT);
    }

    virtual std::any visitRel_expression_one(CSubsetParser::Rel_expression_oneContext *ctx) override
    {
        auto t = std::any_cast<TypeInfo>(visit(ctx->simple_expression()));

        log(ctx->getStart()->getLine(), "rel_expression", "simple_expression");
        log2(getExactRuleText(ctx, tokenStream));

        return t;
    }

    virtual std::any visitRel_expression_two(CSubsetParser::Rel_expression_twoContext *ctx) override
    {
        visit(ctx->simple_expression(0));
        visit(ctx->simple_expression(1));

        log(ctx->getStart()->getLine(), "rel_expression", "simple_expression RELOP simple_expression");
        log2(getExactRuleText(ctx, tokenStream));

        return makeType(BaseType::INT);
    }

    virtual std::any visitSimple_expression_two(CSubsetParser::Simple_expression_twoContext *ctx) override
    {
        auto lhs = std::any_cast<TypeInfo>(visit(ctx->simple_expression()));
        auto rhs = std::any_cast<TypeInfo>(visit(ctx->term()));

        log(ctx->getStart()->getLine(), "simple_expression", "simple_expression ADDOP term");
        log2(getExactRuleText(ctx, tokenStream));

        return (lhs.base == BaseType::FLOAT || rhs.base == BaseType::FLOAT)
                   ? makeType(BaseType::FLOAT)
                   : makeType(BaseType::INT);
    }

    virtual std::any visitSimple_expression_one(CSubsetParser::Simple_expression_oneContext *ctx) override
    {
        auto t = std::any_cast<TypeInfo>(visit(ctx->term()));

        log(ctx->getStart()->getLine(), "simple_expression", "term");
        log2(getExactRuleText(ctx, tokenStream));

        return t;
    }

    virtual std::any visitTerm_one(CSubsetParser::Term_oneContext *ctx) override
    {
        auto t = std::any_cast<TypeInfo>(visit(ctx->unary_expression()));
        ;

        log(ctx->getStart()->getLine(), "term", "unary_expression");
        log2(getExactRuleText(ctx, tokenStream));

        return t;
    }

    virtual std::any visitTerm_two(CSubsetParser::Term_twoContext *ctx) override
    {
        auto lhs = std::any_cast<TypeInfo>(visit(ctx->term()));
        auto rhs = std::any_cast<TypeInfo>(visit(ctx->unary_expression()));
        string op = ctx->MULOP()->getText();

        if (op == "%" && (lhs.base != BaseType::INT || rhs.base != BaseType::INT))
        {
            errF << "Error at line " << ctx->getStart()->getLine()
                 << ": Non-integer operand on modulus operator\n\n";
            errorCount++;
        }

        if ((op == "%" || op == "/") && isZeroLiteral(ctx->unary_expression()))
        {
            errF << "Error at line " << ctx->getStart()->getLine()
                 << ": " << (op == "%" ? "Modulus" : "Division") << " by zero\n\n";
            errorCount++;
        }

        log(ctx->getStart()->getLine(), "term", "term MULOP unary_expression");
        log2(getExactRuleText(ctx, tokenStream));

        return (lhs.base == BaseType::FLOAT || rhs.base == BaseType::FLOAT)
                   ? makeType(BaseType::FLOAT)
                   : makeType(BaseType::INT);
    }

    virtual std::any visitUnary_expression_one(CSubsetParser::Unary_expression_oneContext *ctx) override
    {
        auto t = std::any_cast<TypeInfo>(visit(ctx->unary_expression()));

        log(ctx->getStart()->getLine(), "unary_expression", "ADDOP unary_expression");
        log2(getExactRuleText(ctx, tokenStream));

        return t;
    }

    virtual std::any visitUnary_expression_two(CSubsetParser::Unary_expression_twoContext *ctx) override
    {
        auto t = std::any_cast<TypeInfo>(visit(ctx->unary_expression()));

        log(ctx->getStart()->getLine(), "unary_expression", "NOT unary_expression");
        log2(getExactRuleText(ctx, tokenStream));

        return makeType(BaseType::INT);
    }

    virtual std::any visitUnary_expression_three(CSubsetParser::Unary_expression_threeContext *ctx) override
    {
        auto t = std::any_cast<TypeInfo>(visit(ctx->factor()));

        log(ctx->getStart()->getLine(), "unary_expression", "factor");
        log2(getExactRuleText(ctx, tokenStream));

        return t;
    }

    virtual std::any visitFactor_one(CSubsetParser::Factor_oneContext *ctx) override
    {
        auto t = std::any_cast<TypeInfo>(visit(ctx->variable()));

        log(ctx->getStart()->getLine(), "factor", "variable");
        log2(getExactRuleText(ctx, tokenStream));

        return t;
    }

    virtual std::any visitFactor_two(CSubsetParser::Factor_twoContext *ctx) override
    {
        string id = ctx->ID()->getText();
        vector<TypeInfo> argTypes = std::any_cast<vector<TypeInfo>>(visit(ctx->argument_list()));
        TypeInfo result{BaseType::UNKNOWN, false, 0};
        SymbolInfo *sym = symbolTable.lookUp(id);

        if (!sym)
        {
            errF << "Error at line " << ctx->getStart()->getLine() << ": Undeclared function " << id << "\n\n";
            errorCount++;
        }
        else if (!sym->isFunction())
        {
            errF << "Error at line " << ctx->getStart()->getLine() << ": '" << id << "' is not a function\n";
            errorCount++;
        }
        else
        {
            auto fi = sym->getFuncInfo();
            if (fi->paramTypes.size() != argTypes.size())
            {
                errF << "Error at line " << ctx->getStart()->getLine()
                     << ": Total number of arguments mismatch in function " << id << "\n\n";
                errorCount++;
            }
            else
            {
                for (size_t i = 0; i < argTypes.size(); ++i)
                    if (fi->paramTypes[i].base != argTypes[i].base)
                    {
                        errF << "Error at line " << ctx->getStart()->getLine()
                             << ": " << (i + 1) << "th argument type mismatch in function " << id << "\n\n";
                        errorCount++;
                    }
            }
            result = fi->returnType;
            if (result.base == BaseType::VOID && !isDiscardedCallResult(ctx))
            {
                errF << "Error at line " << ctx->getStart()->getLine()
                     << ": Void function used in expression\n\n";
                errorCount++;
            }
        }

        log(ctx->getStart()->getLine(), "factor", "ID LPAREN argument_list RPAREN");
        log2(getExactRuleText(ctx, tokenStream));

        return result;
    }

    virtual std::any visitFactor_three(CSubsetParser::Factor_threeContext *ctx) override
    {
        auto t = std::any_cast<TypeInfo>(visit(ctx->expression()));

        log(ctx->getStart()->getLine(), "factor", "LPAREN expression RPAREN");
        log2(getExactRuleText(ctx, tokenStream));

        return t;
    }

    virtual std::any visitFactor_four(CSubsetParser::Factor_fourContext *ctx) override
    {
        log(ctx->getStart()->getLine(), "factor", "CONST_INT");
        log2(getExactRuleText(ctx, tokenStream));

        return makeType(BaseType::INT);
    }

    virtual std::any visitFactor_five(CSubsetParser::Factor_fiveContext *ctx) override
    {
        log(ctx->getStart()->getLine(), "factor", "CONST_FLOAT");
        log2(getExactRuleText(ctx, tokenStream));

        return makeType(BaseType::FLOAT);
    }

    virtual std::any visitFactor_six(CSubsetParser::Factor_sixContext *ctx) override
    {
        auto t = std::any_cast<TypeInfo>(visit(ctx->variable()));

        log(ctx->getStart()->getLine(), "factor", "variable INCOP");
        log2(getExactRuleText(ctx, tokenStream));

        return t;
    }

    virtual std::any visitFactor_seven(CSubsetParser::Factor_sevenContext *ctx) override
    {
        auto t = std::any_cast<TypeInfo>(visit(ctx->variable()));

        log(ctx->getStart()->getLine(), "factor", "variable DECOP");
        log2(getExactRuleText(ctx, tokenStream));

        return t;
    }

    virtual std::any visitArgument_list_one(CSubsetParser::Argument_list_oneContext *ctx) override
    {
        auto types = std::any_cast<vector<TypeInfo>>(visit(ctx->arguments()));

        log(ctx->getStart()->getLine(), "argument_list", "arguments");
        log2(getExactRuleText(ctx, tokenStream));

        return types;
    }

    virtual std::any visitArgument_list_two(CSubsetParser::Argument_list_twoContext *ctx) override
    {
        log(ctx->getStart()->getLine(), "argument_list", "");
        log2(getExactRuleText(ctx, tokenStream));
        return vector<TypeInfo>{};
    }

    virtual std::any visitArguments_two(CSubsetParser::Arguments_twoContext *ctx) override
    {
        auto types = std::any_cast<vector<TypeInfo>>(visit(ctx->arguments()));
        auto t = std::any_cast<TypeInfo>(visit(ctx->logic_expression()));
        types.push_back(t);

        log(ctx->getStart()->getLine(), "arguments", "arguments COMMA logic_expression");
        log2(getExactRuleText(ctx, tokenStream));

        return types;
    }

    virtual std::any visitArguments_one(CSubsetParser::Arguments_oneContext *ctx) override
    {
        auto t = std::any_cast<TypeInfo>(visit(ctx->logic_expression()));

        log(ctx->getStart()->getLine(), "arguments", "logic_expression");
        log2(getExactRuleText(ctx, tokenStream));

        return vector<TypeInfo>{t};
    }

    virtual std::any visitParamlist_typespec_baddash(CSubsetParser::Paramlist_typespec_baddashContext *ctx) override
    {
        errF << "Error at line " << ctx->getStart()->getLine()
             << ": syntax error, unexpected token(s) '" << ctx->ADDOP()->getText() << "' before ')'\n\n";
        errorCount++;
        log(ctx->getStart()->getLine(), "parameter_list", "type_specifier ADDOP");
        log2(getExactRuleText(ctx, tokenStream));
        return (vector<TypeInfo> *)nullptr; // not used directly
    }

    virtual std::any visitDeclaration_list_id_baddash(CSubsetParser::Declaration_list_id_baddashContext *ctx) override
    {
        errF << "Error at line " << ctx->getStart()->getLine()
             << ": syntax error, unexpected token(s) '" << ctx->ADDOP()->getText()
             << " " << ctx->ID(1)->getText() << "' in declaration list\n\n";
        errorCount++;
        log(ctx->getStart()->getLine(), "declaration_list", "ID ADDOP ID");
        log2(getExactRuleText(ctx, tokenStream));
        return (void *)nullptr;
    }

    virtual std::any visitLogic_expression_badop(CSubsetParser::Logic_expression_badopContext *ctx) override
    {
        auto t = std::any_cast<TypeInfo>(visit(ctx->rel_expression()));
        errF << "Error at line " << ctx->getStart()->getLine()
             << ": syntax error, invalid operand '=' after '+'\n\n";
        errorCount++;
        log(ctx->getStart()->getLine(), "logic_expression", "rel_expression ADDOP ASSIGNOP");
        log2(getExactRuleText(ctx, tokenStream));
        return t;
    }

    virtual std::any visitExpression_stmt_missing_semi(CSubsetParser::Expression_stmt_missing_semiContext *ctx) override
    {
        auto temp = visit(ctx->expression());
        errF << "Error at line " << ctx->getStart()->getLine()
             << ": syntax error, missing ';' after expression '"
             << getExactRuleText(ctx->expression(), tokenStream) << "'\n\n";
        errorCount++;
        log(ctx->getStart()->getLine(), "expression_statement", "expression (missing SEMICOLON)");
        log2(getExactRuleText(ctx, tokenStream));
        return temp;
    }

    BaseType getType(string str)
    {
        if (str == "int")
            return BaseType::INT;
        else if (str == "float")
            return BaseType::FLOAT;
        else if (str == "void")
            return BaseType::VOID;

        return BaseType::UNKNOWN;
    }

    TypeInfo makeType(BaseType tp, bool isGlobal = false, int stackOffset = 0, bool isArray = false, int size = 0)
    {
        return TypeInfo{tp, isArray, size, stackOffset, isGlobal};
    }

    vector<TypeInfo> extractParamTypes(CSubsetParser::Parameter_listContext *ctx)
    {
        if (!ctx)
            return {};

        if (auto a = dynamic_cast<CSubsetParser::Paramlist_typespec_idContext *>(ctx))
            return {makeType(getType(a->type_specifier()->getText()))};

        if (auto a = dynamic_cast<CSubsetParser::ParamList_typespecContext *>(ctx))
            return {makeType(getType(a->type_specifier()->getText()))};

        if (auto a = dynamic_cast<CSubsetParser::Paramlist_comma_typespec_idContext *>(ctx))
        {
            auto types = extractParamTypes(a->parameter_list());
            types.push_back(makeType(getType(a->type_specifier()->getText())));
            return types;
        }

        if (auto a = dynamic_cast<CSubsetParser::Paramlist_comma_typespecContext *>(ctx))
        {
            auto types = extractParamTypes(a->parameter_list());
            types.push_back(makeType(getType(a->type_specifier()->getText())));
            return types;
        }

        return {};
    }

    vector<pair<string, TypeInfo>> extractDeclarations(CSubsetParser::Declaration_listContext *ctx, BaseType base)
    {
        if (auto a = dynamic_cast<CSubsetParser::Declaration_list_idContext *>(ctx))
        {
            currentStackOffset -= 4;
            return {{a->ID()->getText(), makeType(base, symbolTable.isGlobalScope(), currentStackOffset, false)}};
        }

        if (auto a = dynamic_cast<CSubsetParser::Declaration_list_id_Context *>(ctx))
        {
            int size = stoi(a->CONST_INT()->getText());
            currentStackOffset -= size * 4;
            return {{a->ID()->getText(), makeType(base, symbolTable.isGlobalScope(), currentStackOffset, true, size)}};
        }

        if (auto a = dynamic_cast<CSubsetParser::Declaration_list_commaContext *>(ctx))
        {
            auto decls = extractDeclarations(a->declaration_list(), base);
            currentStackOffset -= 4;
            decls.push_back({a->ID()->getText(), makeType(base, symbolTable.isGlobalScope(), currentStackOffset, false)});
            return decls;
        }

        if (auto a = dynamic_cast<CSubsetParser::Declaration_list_comma_id_Context *>(ctx))
        {
            auto decls = extractDeclarations(a->declaration_list(), base);
            int size = stoi(a->CONST_INT()->getText());
            currentStackOffset -= size * 4;
            decls.push_back({a->ID()->getText(), makeType(base, symbolTable.isGlobalScope(), currentStackOffset, true, size)});
            return decls;
        }

        return {};
    }

    void log(int lineNo, const string &ruleName, const string &expansion)
    {
        logF << "Line " << lineNo << ": " << ruleName << " : " << expansion << "\n\n";
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

    bool isZeroLiteral(CSubsetParser::Unary_expressionContext *ctx)
    {
        try
        {
            double val = stod(ctx->getText());
            return val == 0.0;
        }
        catch (...)
        {
            return false;
        }
    }

    bool isDiscardedCallResult(CSubsetParser::Factor_twoContext *ctx)
    {
        antlr4::tree::ParseTree *node = ctx;
        antlr4::tree::ParseTree *parent = node->parent;

        if (!dynamic_cast<CSubsetParser::Unary_expression_threeContext *>(parent))
            return false;
        node = parent;
        parent = node->parent;

        if (!dynamic_cast<CSubsetParser::Term_oneContext *>(parent))
            return false;
        node = parent;
        parent = node->parent;

        if (!dynamic_cast<CSubsetParser::Simple_expression_oneContext *>(parent))
            return false;
        node = parent;
        parent = node->parent;

        if (!dynamic_cast<CSubsetParser::Rel_expression_oneContext *>(parent))
            return false;
        node = parent;
        parent = node->parent;

        if (!dynamic_cast<CSubsetParser::Logic_expression_oneContext *>(parent))
            return false;
        node = parent;
        parent = node->parent;

        if (!dynamic_cast<CSubsetParser::Expression_oneContext *>(parent))
            return false;
        node = parent;
        parent = node->parent;

        return dynamic_cast<CSubsetParser::Expression_stmt_expr_semicolonContext *>(parent) != nullptr;
    }

    void checkParamNames(CSubsetParser::Parameter_listContext *ctx, const string &funcName, int &pos)
    {
        if (!ctx)
            return;
        if (auto a = dynamic_cast<CSubsetParser::Paramlist_comma_typespec_idContext *>(ctx))
        {
            checkParamNames(a->parameter_list(), funcName, pos);
            pos++;
        }
        else if (auto a = dynamic_cast<CSubsetParser::Paramlist_comma_typespecContext *>(ctx))
        {
            checkParamNames(a->parameter_list(), funcName, pos);
            pos++;
            errF << "Error at line " << a->getStart()->getLine() << ": " << pos
                 << "th parameter's name not given in function definition of " << funcName << "\n\n";
            errorCount++;
        }
        else if (dynamic_cast<CSubsetParser::Paramlist_typespec_idContext *>(ctx))
            pos++;
        else if (auto a = dynamic_cast<CSubsetParser::ParamList_typespecContext *>(ctx))
        {
            pos++;
            errF << "Error at line " << a->getStart()->getLine() << ": " << pos
                 << "th parameter's name not given in function definition of " << funcName << "\n\n";
            errorCount++;
        }
        else if (auto a = dynamic_cast<CSubsetParser::Paramlist_typespec_baddashContext *>(ctx))
        {
            pos++;
            errF << "Error at line " << a->getStart()->getLine() << ": " << pos
                 << "th parameter's name not given in function definition of " << funcName << "\n\n";
            errorCount++;
        }
    }

    string getName(const string& id, TypeInfo& t)
    {
        if (t.isGlobal)
            return "[" + id + "]";
        string off = (t.stackOffset >= 0 ? "+" + to_string(t.stackOffset) : to_string(t.stackOffset));
        return "[EBP" + off + "]";
    }
};

#endif