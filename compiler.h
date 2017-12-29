/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <string>
#include <vector>

struct ValueNode
{
    std::string name;
    int value;
};

typedef enum {
    PRIMARY = 100,
    EXPR,
    VARACCESS
} expr_type;

struct GotoStatement
{
    struct StatementNode * target;
};

enum ArithmeticOperatorType {
    OPERATOR_NONE = 123,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_MULT,
    OPERATOR_DIV
};

struct AssignmentStatement
{
    struct VarAccessNode * left_hand_side;
    struct ExprNode* expr;

    /*struct ValueNode * operand1;
    struct ValueNode * operand2;*/


    /*
     * If op == OPERATOR_NONE then only operand1 is meaningful.
     * Otherwise both operands are meaningful
     */
    //ArithmeticOperatorType op;
};

enum ConditionalOperatorType {
    CONDITION_GREATER = 345,
    CONDITION_LESS,
    CONDITION_NOTEQUAL
};

struct IfStatement
{
    /*struct ValueNode * condition_operand1;
    struct ValueNode * condition_operand2;*/

    struct ExprNode * condition_operand1;
    struct ExprNode * condition_operand2;


    ConditionalOperatorType condition_op;

    struct StatementNode * true_branch;
    struct StatementNode * false_branch;
};

enum StatementType
{
    NOOP_STMT = 1000,
    PRINT_STMT,
    ASSIGN_STMT,
    IF_STMT,
    GOTO_STMT
};

struct StatementNode
{
    StatementType type;

    union
    {
        struct AssignmentStatement * assign_stmt;
        struct PrintStatement * print_stmt;
        struct IfStatement * if_stmt;
        struct GotoStatement * goto_stmt;
    };

    struct StatementNode * next; // next statement in the list or NULL
};

struct ExprNode
{
    ArithmeticOperatorType op;
    expr_type tag; // PRIMARY or EXPR
    struct ValueNode* primary;
    struct ExprNode * Operand_left;
    struct ExprNode * Operand_right;
    struct VarAccessNode* var_access;
};

struct VarAccessNode
{
    expr_type tag;
    std::string expr_name;
    struct ValueNode* primary;
    struct ExprNode* exprNode;
};

struct PrintStatement
{
    struct VarAccessNode * id;
};

extern struct ValueNode* variableList[500];
extern int count_var;


void debug(const char* format, ...);

//---------------------------------------------------------
// You should write the following function:

struct StatementNode * parse_generate_intermediate_representation();
struct ValueNode* searchVariable(std::string str);

/*
  NOTE:

  You need to write a function with the above signature. This function
  is supposed to parse the input program and generate an intermediate
  representation for it. The output of this function is passed to the
  execute_program function in main().

  Write your code in a separate file and include this header file in
  your code.
*/

#endif /* _COMPILER_H_ */
