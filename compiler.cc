/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cctype>
#include <cstring>
#include "string"
#include "compiler.h"
#include "ir_debug.h"
#include "fstream"
#include "iostream"

#define DEBUG 0     // 1 => Turn ON debugging, 0 => Turn OFF debugging
struct ValueNode* variableList[500];
int count_var;

void debug(const char* format, ...)
{
    va_list args;
    if (DEBUG)
    {
        va_start (args, format);
        vfprintf (stdout, format, args);
        va_end (args);
    }
}

int evaluate_expr(struct ExprNode* expr){

    int result;
    //TODO write the function definition to eval expr

    if(expr->tag == PRIMARY)
    {
        result = expr->primary->value;
    }
    else if(expr->tag == VARACCESS)
    {
        if(expr->var_access->tag == PRIMARY)
        {
            result = expr->var_access->primary->value;
        }
        else
        {
            std::string varname = expr->var_access->expr_name;
            int num = evaluate_expr(expr->var_access->exprNode);
            varname = varname + std::to_string(num);
            struct ValueNode* val = searchVariable(varname);
            result = val->value;
        }
    }
    else if(expr->tag == EXPR){

        int left, right;

        left = evaluate_expr(expr->Operand_left);
        right = evaluate_expr(expr->Operand_right);

        if(expr->op == OPERATOR_PLUS)
            result = left + right;
        else if(expr->op == OPERATOR_MULT)
            result = left * right;
    }

    return result;
}

void execute_program(struct StatementNode * program)
{
    struct StatementNode * pc = program;
    struct ValueNode* val;
    int op1, op2, result, expr_val;
    std::string varName;

    while (pc != NULL)
    {
        switch (pc->type)
        {
            case NOOP_STMT:
                pc = pc->next;
                break;

            case PRINT_STMT:
                if (pc->print_stmt == NULL)
                {
                    debug("Error: pc points to a print statement but pc->print_stmt is null.\n");
                    exit(1);
                }
                if (pc->print_stmt->id == NULL) {
                    debug("Error: print_stmt->id is null.\n");
                    exit(1);
                }
                if(pc->print_stmt->id->tag == PRIMARY)
                {
                    if (pc->print_stmt->id->primary == NULL) {
                        debug("Error: print_stmt->id->primary is null.\n");
                        exit(1);
                    }
                }
                else
                {
                    if (pc->print_stmt->id->exprNode== NULL) {
                        debug("Error: print_stmt->id->primary is null.\n");
                         exit(1);
                    }
                }

                if(pc->print_stmt->id->tag == PRIMARY){
                    printf("%d\n", pc->print_stmt->id->primary->value);
                }
                else{
                    int num = evaluate_expr(pc->print_stmt->id->exprNode);
                    varName = pc->print_stmt->id->expr_name;
                    varName = varName + std::to_string(num);
                    val = searchVariable(varName);
                    printf("%d\n", val->value);
//                    pc->print_stmt->id->tag = PRIMARY;
//                    pc->print_stmt->id->primary = val;
//                    printf("%d\n", pc->print_stmt->id->primary->value);
                }

                pc = pc->next;
                break;

            case ASSIGN_STMT:
                if (pc->assign_stmt == NULL)
                {
                    debug("Error: pc points to an assignment statement but pc->assign_stmt is null.\n");
                    exit(1);
                }
                if (pc->assign_stmt->expr == NULL)
                {
                    debug("Error: assign_stmt->operand1 is null.\n");
                    exit(1);
                }
                /*if (pc->assign_stmt->op != OPERATOR_NONE)
                {
                    if (pc->assign_stmt->operand2 == NULL)
                    {
                        debug("Error: assign_stmt->op requires two operands but assign_stmt->operand2 is null.\n");
                        exit(1);
                    }
                }*/
                if (pc->assign_stmt->left_hand_side == NULL)
                {
                    debug("Error: assign_stmt->left_hand_side is null.\n");
                    exit(1);
                }

                switch(pc->assign_stmt->expr->tag)
                {
                    case PRIMARY:
                        op1 = pc->assign_stmt->expr->primary->value;
                        result = op1;

                        break;
                    case VARACCESS:
                        varName = pc->assign_stmt->expr->var_access->expr_name;
                        expr_val = evaluate_expr(pc->assign_stmt->expr->var_access->exprNode);
                        varName = varName + std::to_string(expr_val);
                        val = searchVariable(varName);
                        result = val->value;
                        break;
                    case EXPR:
                        result = evaluate_expr(pc->assign_stmt->expr);
                        break;
                }

                /*switch (pc->assign_stmt->op)
                {
                    case OPERATOR_PLUS:
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 + op2;
                        break;
                    case OPERATOR_MINUS:
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 - op2;
                        break;
                    case OPERATOR_MULT:
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 * op2;
                        break;
                    case OPERATOR_DIV:
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 / op2;
                        break;
                    case OPERATOR_NONE:
                        op1 = pc->assign_stmt->operand1->value;
                        result = op1;
                        break;
                    default:
                        debug("Error: invalid value for assign_stmt->op (%d).\n", pc->assign_stmt->op);
                        exit(1);
                        break;
                }*/

                if(pc->assign_stmt->left_hand_side->tag == PRIMARY)
                {
                    pc->assign_stmt->left_hand_side->primary->value = result;
                }
                else
                {
                    int num = evaluate_expr(pc->assign_stmt->left_hand_side->exprNode);
                    varName = pc->assign_stmt->left_hand_side->expr_name;
                    varName = varName + std::to_string(num);
                    val = searchVariable(varName);
                    val->value = result;
//                    pc->assign_stmt->left_hand_side->tag = PRIMARY;
//                    pc->assign_stmt->left_hand_side->primary = val;
                }
                //pc->assign_stmt->left_hand_side-> = result;
                pc = pc->next;
                break;

            case IF_STMT:
                if (pc->if_stmt == NULL)
                {
                    debug("Error: pc points to an if statement but pc->if_stmt is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->true_branch == NULL)
                {
                    debug("Error: if_stmt->true_branch is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->false_branch == NULL)
                {
                    debug("Error: if_stmt->false_branch is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->condition_operand1 == NULL)
                {
                    debug("Error: if_stmt->condition_operand1 is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->condition_operand2 == NULL)
                {
                    debug("Error: if_stmt->condition_operand2 is null.\n");
                    exit(1);
                }
                op1 = evaluate_expr(pc->if_stmt->condition_operand1);
                op2 = evaluate_expr(pc->if_stmt->condition_operand2);
                switch (pc->if_stmt->condition_op)
                {
                    case CONDITION_GREATER:
                        if (op1 > op2)
                            pc = pc->if_stmt->true_branch;
                        else
                            pc = pc->if_stmt->false_branch;
                        break;
                    case CONDITION_LESS:
                        if (op1 < op2)
                            pc = pc->if_stmt->true_branch;
                        else
                            pc = pc->if_stmt->false_branch;
                        break;
                    case CONDITION_NOTEQUAL:
                        if (op1 != op2)
                            pc = pc->if_stmt->true_branch;
                        else
                            pc = pc->if_stmt->false_branch;
                        break;
                    default:
                        debug("Error: invalid value for if_stmt->condition_op (%d).\n", pc->if_stmt->condition_op);
                        exit(1);
                        break;
                }
                break;

            case GOTO_STMT:
                if (pc->goto_stmt == NULL)
                {
                    debug("Error: pc points to a goto statement but pc->goto_stmt is null.\n");
                    exit(1);
                }
                if (pc->goto_stmt->target == NULL)
                {
                    debug("Error: goto_stmt->target is null.\n");
                    exit(1);
                }
                pc = pc->goto_stmt->target;
                break;

            default:
                debug("Error: invalid value for pc->type (%d).\n", pc->type);
                exit(1);
                break;
        }
    }
}

int main()
{
//  std::ifstream in("/Users/nikitashanker/CLionProjects/proj4_bonus/tests_bonus/test09.txt");
//   std::cin.rdbuf(in.rdbuf());

    struct StatementNode * program;
    program = parse_generate_intermediate_representation();
    if (DEBUG)
    {
        printf("\n-------------------------------------------\n");
        print_program(program);
        printf("\n-------------------------------------------\n");
    }
    execute_program(program);
    return 0;
}
