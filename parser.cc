//
// Created by Nikita Shanker on 4/20/17.
//

#include "iostream"
#include "cstdlib"
#include "string"
#include "compiler.h"
#include "lexer.h"
#include "vector"

using namespace std;

//methods
void syntax_error(string msg);
struct StatementNode* parse_program();
void parse_varSection();
struct StatementNode* parse_body();
void parse_idList();
struct StatementNode* parse_stmtList();
struct StatementNode* parse_stmt();
struct AssignmentStatement* parse_assign_stmt();
struct PrintStatement* parse_print_stmt();
struct StatementNode* parse_while_stmt();
struct IfStatement* parse_if_stmt();
struct StatementNode* parse_switch_stmt();
struct StatementNode* parse_for_stmt();
//void parse_expr();
//void parse_op();
struct IfStatement* parse_condition();
struct StatementNode* parse_caseList(struct ValueNode* id, struct StatementNode* noop);
//void parse_default_caseList();
//void parse_case();
struct ValueNode* parse_primary();
void addVariableList(string str);
struct ValueNode* searchVariable(string str);
ArithmeticOperatorType getMathOp(TokenType tokenType);
ConditionalOperatorType getCondOpr(TokenType tokenType);

//variables
struct ValueNode* variableList[50];
LexicalAnalyzer lexer;
bool debugger = true;
int count_var = 0;

void syntax_error(string msg){
    if(debugger)
        cout<<msg<<endl;
    exit(1);
}

ArithmeticOperatorType getMathOp(TokenType tokenType){
    if(tokenType == PLUS)
        return OPERATOR_PLUS;
    else if (tokenType == MINUS)
        return OPERATOR_MINUS;
    else if (tokenType == MULT)
        return OPERATOR_MULT;
    else if (tokenType == DIV)
        return OPERATOR_DIV;
    else
        return OPERATOR_NONE;
}

ConditionalOperatorType getCondOpr(TokenType tokenType){
    if(tokenType == GREATER)
        return CONDITION_GREATER;
    else if (tokenType == LESS)
        return CONDITION_LESS;
    else if (tokenType == NOTEQUAL)
        return CONDITION_NOTEQUAL;
}

struct StatementNode* parse_generate_intermediate_representation(){

    struct StatementNode* program = parse_program();
    return program;
}

struct StatementNode* parse_program(){

    struct StatementNode* program;
    parse_varSection();
    program = parse_body();
    return program;
}

void parse_varSection(){
    parse_idList();
    Token t = lexer.GetToken();
    if(t.token_type != SEMICOLON)
        syntax_error("Error in Parse_varSection line 70");
}

struct StatementNode* parse_body(){

    Token t1 = lexer.GetToken();
    struct StatementNode* program;

    if (t1.token_type == LBRACE){
        program = parse_stmtList();
        t1 = lexer.GetToken();
        if(t1.token_type != RBRACE)
            syntax_error("Error in Parse_body - RBRACE expected");
        return program;
    }
    else
        syntax_error("Error in parse_body - LBRACE expected");

    return NULL;
}

struct StatementNode* parse_stmtList(){

    struct StatementNode* st;
    struct StatementNode* st1;

    st = parse_stmt();

    Token t = lexer.GetToken();
    if(t.token_type == ID || t.token_type == PRINT || t.token_type == WHILE || t.token_type == IF || t.token_type == SWITCH || t.token_type == FOR)
    {
        lexer.UngetToken(t);
       // st1 = new StatementNode;
        st1 = parse_stmtList();

        if(st->type == IF_STMT)
        {
            st->next->next = st1;
        }
        else
        {
            struct StatementNode* head = st;
            //head = st;

            while(1)
            { if (head->next == NULL)
                    break;
                head = head->next;
            }
            head->next = st1;
        }
        return st;
    }
    else
    {
        lexer.UngetToken(t);
        return st;
    }

    return NULL;
}

struct StatementNode* parse_stmt(){

    struct StatementNode* st = new StatementNode;

    Token t1 = lexer.GetToken();
    if (t1.token_type == ID)
    {
        lexer.UngetToken(t1);
        st->assign_stmt = parse_assign_stmt();
        st->type = ASSIGN_STMT;
        return st;
    }

    else if(t1.token_type == PRINT)
    {
        //lexer.UngetToken(t1);
        st->type = PRINT_STMT;
        st->print_stmt =parse_print_stmt() ;
        t1 = lexer.GetToken();

        if(t1.token_type == SEMICOLON)
            return st;
        else
            syntax_error("error: print Semicolon \n");
    }

    else if(t1.token_type == WHILE)
    {
        st = parse_while_stmt();
        return st;
        //return NULL;
    }

    else if (t1.token_type == IF)
    {
        st->type = IF_STMT;
        struct IfStatement* if_st;
        struct StatementNode* noop = new StatementNode;
        struct StatementNode* head_if = new StatementNode;
        noop->type = NOOP_STMT;
        if_st = parse_if_stmt();

        head_if = if_st->true_branch;
        while (if_st->true_branch != NULL)
        {
            if(if_st->true_branch->next == NULL)
            {
                if_st->true_branch->next = noop;
                break;
            }
            else
                if_st->true_branch = if_st->true_branch->next;
        }

        if_st->true_branch = head_if;
        if_st->false_branch = noop;
        st->if_stmt = if_st;
        st->next = noop;

        return st;
    }

    else if (t1.token_type ==  SWITCH)
    {
        st = parse_switch_stmt();
        return st;
        //return NULL;
    }

    else if (t1.token_type ==  FOR)
    {
        st = parse_for_stmt();
        return st;
        //return NULL;
    }
    else
    {
        syntax_error("error in parse_stmt");
    }
    return NULL;
}

void parse_idList(){
    Token t = lexer.GetToken();
    if(t.token_type == ID){
        //add to list
        addVariableList(t.lexeme);
        t = lexer.GetToken();
        if(t.token_type == COMMA)
            parse_idList();
        else if(t.token_type == SEMICOLON)
            lexer.UngetToken(t);
        else
            syntax_error("Error in parse_idlist - line 57");
    }
    else syntax_error("Error in parse_idList - line 59");
}

struct ValueNode* parse_primary(){

    struct ValueNode* val;
    Token t = lexer.GetToken();

    if (t.token_type == ID)
        val = searchVariable(t.lexeme);
    else if (t.token_type == NUM){
        val = new ValueNode;
        val->value = atoi(t.lexeme.c_str());
        val->name = "";
    }
    else
        val = NULL;

    return val;
}

struct ValueNode* searchVariable(string str){

    for (int i = 0;i<count_var;i++){

        string string1 = variableList[i]->name;

        if(string1.compare(str) == 0)
            return variableList[i];
    }
    return NULL;
}

void addVariableList(string str){

    variableList[count_var] = new ValueNode;
    variableList[count_var]->name = str;
    variableList[count_var]->value = 0;
    count_var++;
}

struct PrintStatement* parse_print_stmt()
{
    struct PrintStatement* st = new PrintStatement;
    Token t = lexer.GetToken();

    if (t.token_type== ID)
        st->id = searchVariable(t.lexeme);
    else
    {
        cout<<"error in print";
        st->id  = NULL;
    }

    return st;
}

struct AssignmentStatement* parse_assign_stmt()
{
    struct AssignmentStatement* st = new AssignmentStatement;
    Token t = lexer.GetToken();

    if(t.token_type == ID)
    {
        st->left_hand_side = searchVariable(t.lexeme);;

        t = lexer.GetToken();
        if(t.token_type == EQUAL)
        {
            t = lexer.GetToken();

            if(t.token_type == ID || t.token_type == NUM)
            {
                lexer.UngetToken(t);
                st->operand1 = parse_primary();
                t = lexer.GetToken();

                if(t.token_type == SEMICOLON)
                {
                    //assign_stmt -> ID EQUAL primary SEMICOLON
                    st->op =OPERATOR_NONE ;
                    return st;
                }
                else if(t.token_type ==PLUS || t.token_type == MINUS || t.token_type == MULT || t.token_type == DIV)
                {
                    //assign_stmt -> ID EQUAL expr SEMICOLON

                    st->op = getMathOp(t.token_type);
                    t = lexer.GetToken();
                    if (t.token_type == ID || t.token_type == NUM)
                    {
                        lexer.UngetToken(t);
                        st->operand2 = parse_primary();
                        t = lexer.GetToken();
                        if(t.token_type == SEMICOLON)
                            return st;
                        else
                            syntax_error("error in parse_assign - rule 2 expr SEMICOLON error\n");
                    }
                    else
                        syntax_error("Error in parse_assign - operand 2 - line 320\n");
                }
                else
                    syntax_error("Error in parse_assign - Rule 2 op error - line 323 \n");
            }
            else
                syntax_error("Error in parse_assign Operand1\n");
        }
        else syntax_error("Error in parse_assign - EQUAL error");
    }
    else
        syntax_error("error in parse_assign - lhs ID error\n");
    return NULL;
}

struct IfStatement* parse_if_stmt()
{
    struct IfStatement* if_st;

    if_st = parse_condition();
    if_st->true_branch = parse_body();
    return if_st;
}

struct IfStatement* parse_condition()
{
    struct IfStatement* st = new IfStatement;
    Token t = lexer.GetToken();

    if (t.token_type == ID || t.token_type == NUM){
        lexer.UngetToken(t);
        st->condition_operand1 = parse_primary();

        t = lexer.GetToken();
        if(t.token_type == GREATER || t.token_type == LESS || t.token_type == NOTEQUAL){
            st->condition_op = getCondOpr(t.token_type);
            t = lexer.GetToken();
            if (t.token_type == ID || t.token_type == NUM){
                lexer.UngetToken(t);
                st->condition_operand2 = parse_primary();
                return st;
            }
            else
                syntax_error("Error in if condition - op2 - line 355");
        }
        else
            syntax_error("Error in if condition - relop - line 355");


    }
    else
        syntax_error("Error in IF condition op1 line 360");
    return NULL;
}

struct StatementNode* parse_while_stmt(){

    struct StatementNode* stmt = new StatementNode;

    struct IfStatement* if_st;

    stmt->type = IF_STMT;
    if_st = parse_if_stmt();

    struct StatementNode* gt = new StatementNode;
    gt->type = GOTO_STMT;
    struct GotoStatement* goto_node = new GotoStatement;
    gt->goto_stmt = goto_node;
    gt->goto_stmt->target = stmt;

    //appending gt to body of while
    struct StatementNode* body_head = new StatementNode;
    body_head = if_st->true_branch;
    while(if_st->true_branch != NULL)
    {
        if(if_st->true_branch->next == NULL)
        {
            if_st->true_branch->next = gt;
            break;
        }
        else
            if_st->true_branch = if_st->true_branch->next;
    }

    if_st->true_branch = body_head;
    struct StatementNode* noop = new StatementNode;
    noop->type = NOOP_STMT;
    if_st->false_branch = noop;
    stmt->if_stmt = if_st;
    stmt->next = noop;

    return stmt;
}

struct StatementNode* parse_switch_stmt(){
    struct StatementNode* st;
    struct StatementNode* st_node;
    struct StatementNode* noop = new StatementNode;
    noop->type = NOOP_STMT;

    struct ValueNode* id = parse_primary();

    Token t = lexer.GetToken();
    if (t.token_type == LBRACE)
    {
        st_node = parse_caseList(id,noop);
        t = lexer.GetToken();
        if (t.token_type == RBRACE)
        {
            st = st_node;
            st->type = IF_STMT;
            st->next = noop;

            return st;
        }
    }
    return NULL;
}

struct StatementNode* parse_caseList(struct ValueNode* id, struct StatementNode* noop){

    struct StatementNode* st;
    struct IfStatement* if_st;
    //struct StatementNode* noop;
    Token t = lexer.GetToken();
    if(t.token_type == CASE)
    {
        st = new StatementNode;
        if_st = new IfStatement;
        //noop = new StatementNode;
        //noop->type = NOOP_STMT;

        st->type = IF_STMT;

        if_st->condition_operand1 = id;
        if_st->condition_op = CONDITION_NOTEQUAL;
        if_st->condition_operand2 = parse_primary(); //NUM = op2
        t = lexer.GetToken();
        if(t.token_type == COLON)
        {
            if_st->false_branch = parse_body();
            struct StatementNode* head = new StatementNode;
            head = if_st->false_branch;
            while (if_st->false_branch != NULL)
            {
                if(if_st->false_branch->next == NULL)
                {
                    if_st->false_branch ->next = noop;
                    break;
                }
                else
                {
                    if_st->false_branch = if_st->false_branch->next;
                }
            }

            if_st->false_branch = head;

            t = lexer.GetToken();
            if(t.token_type == CASE || t.token_type == DEFAULT)
            {
                lexer.UngetToken(t);
                struct StatementNode* case_list = new StatementNode;
                case_list = parse_caseList(id,noop);
                if_st->true_branch = case_list;
            }
            else
            {
                lexer.UngetToken(t);
                if_st->true_branch = noop;
            }
            st->if_stmt = if_st;
            st->next = noop;
            return st;
        }
        else
            return NULL;
    }

    else if (t.token_type == DEFAULT)
    {
        struct StatementNode* def;
        t = lexer.GetToken();
        if (t.token_type == COLON)
        {
            //def = ALLOC(struct StatementNode);
            def = parse_body();
            def->next = noop;
            return def;
        }
        else
            return NULL;
    }

    else
    {
        lexer.UngetToken(t);
        return NULL;
    }
}

struct StatementNode* parse_for_stmt(){

    struct StatementNode* st = new StatementNode;
    struct AssignmentStatement* assign_st1;
    struct AssignmentStatement* assign_st2;
    struct StatementNode* cond_st;
    struct IfStatement* if_cond;
    struct StatementNode* noop = new StatementNode;
    noop->type = NOOP_STMT;

    Token t = lexer.GetToken();
    if(t.token_type == LPAREN){

        t = lexer.GetToken();

        if(t.token_type == ID){

            lexer.UngetToken(t);
            assign_st1 = parse_assign_stmt();
            st->assign_stmt = assign_st1;
            st->type = ASSIGN_STMT;

            t = lexer.GetToken();
            if(t.token_type == ID|| t.token_type == NUM)
            {
                lexer.UngetToken(t);
                cond_st = new StatementNode;
                cond_st->type = IF_STMT;

                struct StatementNode* gt = new StatementNode;
                gt->type = GOTO_STMT;
                struct GotoStatement* goto_node = new GotoStatement;
                gt->goto_stmt = goto_node;
                gt->goto_stmt->target = cond_st;

                if_cond = parse_condition();
                t = lexer.GetToken();
                if(t.token_type == SEMICOLON){

                    struct StatementNode* st_assign_2 = new StatementNode;
                    st_assign_2->type = ASSIGN_STMT;
                    assign_st2 = parse_assign_stmt();
                    st_assign_2->assign_stmt = assign_st2;
                    //assign goto to next of assign_2 stmt
                    st_assign_2->next = gt;

                    t = lexer.GetToken();
                    if(t.token_type == RPAREN){
                        if_cond->true_branch = parse_body();

                        //append assign2 to true_branch

                        struct StatementNode* body_head = new StatementNode;
                        body_head = if_cond->true_branch;
                        while(if_cond->true_branch != NULL)
                        {
                            if(if_cond->true_branch->next == NULL)
                            {
                                if_cond->true_branch->next = st_assign_2;
                                break;
                            }
                            else
                                if_cond->true_branch = if_cond->true_branch->next;
                        }

                        if_cond->true_branch = body_head;

                        //append goto to body of if
//                        body_head = if_cond->true_branch;
//                        while(if_cond->true_branch != NULL){
//                            if(if_cond->true_branch->next == NULL)
//                            {
//                                if_cond->true_branch->next = gt;
//                                break;
//                            }
//                            else
//                                if_cond->true_branch = if_cond->true_branch->next;
//                        }
//
//                        if_cond->true_branch = body_head;

                        if_cond->false_branch = noop;
                        cond_st->if_stmt = if_cond;
                        cond_st->next = noop;

                        //append if_stmt to original statement
                        st->next = cond_st;
                        return st;

                    }
                    else syntax_error("Error in For Stmt - RPAREN missing");
                }
                else
                    syntax_error("Error in For Stmt - SEMICOLON missing");
            }
            else
                syntax_error("Error in For stmt - Condition error");
        }
        else
            syntax_error("Error in For Statement - ID missing for Assign1");
    }
    else
        syntax_error("Error - For Stmt - LPAREN ");

    return NULL;
}

