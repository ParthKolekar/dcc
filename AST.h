#ifndef _AST_H
#define _AST_H

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "Visitor.h"

class ASTProgram;
class ASTFieldDecl;
class ASTVarDecl;
class ASTIdentifier;
class ASTMethodDecl;
class ASTTypeIdentifier;
class ASTStatement;
class ASTExpression;
class ASTBlockStatement;
class ASTAssignmentStatement;
class ASTMethodCall;
class ASTNormalMethod;
class ASTCalloutMethod;
class ASTCalloutArg;
class ASTStringCalloutArg;
class ASTExpressionCalloutArg;
class ASTIfStatement;
class ASTForStatement;
class ASTReturnStatement;
class ASTContinueStatement;
class ASTBreakStatement;
class ASTLocation;
class ASTVarLocation;
class ASTArrayLocation;
class ASTLiteralExpression;
class ASTIntegerLiteralExpression;
class ASTCharLiteralExpression;
class ASTTrueLiteralExpression;
class ASTFalseLiteralExpression;
class ASTBinaryOperationExpression;
class ASTUnaryOperationExpression;

enum class AssignOp {
    plus_equal,
    minus_equal,
    equal
};

enum class BinOp {
    plus_op,
    minus_op,
    multiply_op,
    divide_op,
    modulo_op,
    lessthan_op,
    greaterthan_op,
    lessequal_op,
    greaterequal_op,
    notequal_op,
    equalequal_op,
    and_op,
    or_op
};

enum class Datatype {
    int_type,
    void_type,
    bool_type
};

enum class UnOp {
    minus_op,
    not_op
};

class ASTNode {
public:
    ASTNode() {
    }
    ~ASTNode() {
    }
};

class ASTProgram : public ASTNode
{
    std::string id;
    std::vector<ASTFieldDecl *> * fdl;
    std::vector<ASTMethodDecl *> * mdl;
public:
    ASTProgram(std::string id, std::vector<ASTFieldDecl *> * fdl, std::vector<ASTMethodDecl *> * mdl){
        this->id = id;
        this->mdl = mdl;
        this->fdl = fdl;
    }
    std::string getId(){
        return this->id;
    }
    std::vector<ASTMethodDecl *> * getMdl() {
        return this->mdl;
    }
    std::vector<ASTFieldDecl *> * getFdl() {
        return this->fdl;
    }
    ~ASTProgram() {

    }
};

class ASTFieldDecl : public ASTNode
{
    Datatype type;
    std::vector<ASTIdentifier *> * id_list;
public:
    ASTFieldDecl(std::vector<ASTIdentifier *> * id_list, Datatype type){
        this->type = type;
        this->id_list = id_list;
    }
    std::vector<ASTIdentifier *> * getId_list() {
        return this->id_list;
    }
    Datatype getType() {
        return this->type;
    }
    ~ASTFieldDecl() {

    }
};

class ASTVarDecl : public ASTNode
{
    Datatype type;
    std::vector<ASTIdentifier *> * id_list;
public:
    ASTVarDecl(std::vector<ASTIdentifier *> * id_list, Datatype type) {
        this->type = type;
        this->id_list = id_list;
    }
    std::vector<ASTIdentifier *> * getId_list() {
        return this->id_list;
    }
    Datatype getType() {
        return this->type;
    }
    ~ASTVarDecl() {

    }
};

class ASTIdentifier
{
    std::string id;
    int size;
public:
    ASTIdentifier(std::string id, int size) {
        if (size <= 0) {
            std::cerr << "Invalid Size" << std::endl;
            exit(-1);
        }

        this->id = id;
        this->size = size;
    }
    ASTIdentifier(std::string id) {
        this->id = id;
        this->size = 0;
    }
    std::string getId() {
        return this->id;
    }
    int getSize() {
        return this->size;
    }
    ~ASTIdentifier() {

    }
};

class ASTMethodDecl : public ASTNode
{
    std::string id;
    Datatype returnType;
    std::vector<ASTTypeIdentifier *> * arguments;
    ASTBlockStatement * block;
public:
    ASTMethodDecl(std::string id, Datatype returnType, std::vector<ASTTypeIdentifier *> * arguments, ASTBlockStatement * block) {
        this->id = id;
        this->returnType = returnType;
        this->arguments = arguments;
        this->block = block;
    }
    std::string getId() {
        return this->id;
    }
    Datatype getReturnType() {
        return this->returnType;
    }
    std::vector<ASTTypeIdentifier *> * getArguments() {
        return this->arguments;
    }
    ASTBlockStatement * getBlock() {
        return this->block;
    }
    ~ASTMethodDecl() {

    }
};

class ASTTypeIdentifier : public ASTNode
{
    std::string id;
    Datatype type;
public:
    ASTTypeIdentifier(std::string id, Datatype type) {
        this->id = id;
        this->type = type;
    }
    ~ASTTypeIdentifier() {

    }
    std::string getId() {
        return this->id;
    }
    Datatype getType() {
        return this->type;
    }
};

class ASTStatement : public ASTNode 
{
public:
    ASTStatement() {

    }
    virtual ~ASTStatement() {

    }
};


class ASTExpression : public ASTNode
{
public:
    ASTExpression() {

    }
    virtual ~ASTExpression() {

    }
};

class ASTBlockStatement : public ASTStatement
{
    std::vector<ASTVarDecl *> * id_list;
    std::vector<ASTStatement *> * stmtlist;
public:
    ASTBlockStatement(std::vector<ASTStatement *> * stmtlist, std::vector<ASTVarDecl *> * id_list) {
        this->stmtlist = stmtlist;
        this->id_list = id_list;
    }
    std::vector<ASTVarDecl *> * getId_list() {
        return this->id_list;
    }
    std::vector<ASTStatement *> * getStmtlist() {
        return this->stmtlist;
    }
    ~ASTBlockStatement() {

    }
};

class ASTAssignmentStatement : public ASTStatement
{
    AssignOp op;
    ASTLocation * location;
    ASTExpression * expr;
public:
    ASTAssignmentStatement(AssignOp op, ASTLocation * location, ASTExpression * expr) {
        this->op = op;
        this->location = location;
        this->expr = expr;
    }
    AssignOp getOp() {
        return this->op;
    }
    ASTLocation * getLocation() {
        return this->location;
    }
    ASTExpression * getExpr() {
        return this->expr;
    }
    ~ASTAssignmentStatement() {

    }
};

class ASTMethodCall : public ASTStatement, public ASTExpression 
{
public:
    ASTMethodCall() {

    }
    virtual ~ASTMethodCall() {

    }
};

class ASTNormalMethod : public ASTMethodCall 
{
    std::string id;
    std::vector<ASTExpression *> * arguments;
public:
    ASTNormalMethod(std::string id, std::vector<ASTExpression *> * arguments) {
        this->id = id;
        this->arguments = arguments;
    }
    std::string getId() {
        return this->id;
    }
    std::vector<ASTExpression *> * getArguments() {
        return this->arguments;
    }
    ~ASTNormalMethod() {

    }
};

class ASTCalloutMethod : public ASTMethodCall 
{
    std::string method_name;
    std::vector<ASTCalloutArg *> * arguments;
public:
    ASTCalloutMethod(std::string method_name, std::vector<ASTCalloutArg *> * arguments) {
        this->method_name = method_name;
        this->arguments = arguments;
    }
    std::string getMethod_name() {
        return this->method_name;
    }
    std::vector<ASTCalloutArg *> * getArguments() {
        return this->arguments;
    }
    ~ASTCalloutMethod() {

    }
};

class ASTCalloutArg : public ASTExpression
{
public:
    ASTCalloutArg() {

    }
    virtual ~ASTCalloutArg() {

    }
};

class ASTStringCalloutArg : public ASTCalloutArg 
{
    std::string argument;
public:
    ASTStringCalloutArg(std::string argument) {
        this->argument = argument;
    }
    std::string getArgument() {
        return this->argument;
    }
    ~ASTStringCalloutArg() {

    }
};

class ASTExpressionCalloutArg : public ASTCalloutArg 
{
    ASTExpression * argument;
public:
    ASTExpressionCalloutArg(ASTExpression * argument) {
        this->argument = argument;
    }
    ASTExpression * getArgument() {
        return this->argument;
    }
    ~ASTExpressionCalloutArg() {

    }
};

class ASTIfStatement : public ASTStatement 
{
    ASTExpression * condition;
    ASTBlockStatement * if_block;
    ASTBlockStatement * else_block;
public:
    ASTIfStatement(ASTExpression * condition, ASTBlockStatement * if_block, ASTBlockStatement * else_block) {
        this->condition = condition;
        this->if_block = if_block;
        this->else_block = else_block;
    }
    ASTExpression * getCondition() {
        return this->condition;
    }
    ASTBlockStatement * getIf_block() {
        return this->if_block;
    }
    ASTBlockStatement * getElse_block() {
        return this->else_block;
    }
    ~ASTIfStatement() {

    }
};

class ASTForStatement : public ASTStatement 
{
    std::string id;
    ASTExpression * init_condition;
    ASTExpression * end_condition;
    ASTBlockStatement * block;
public:
    ASTForStatement(ASTExpression * init_condition, ASTExpression * end_condition, ASTBlockStatement * block, std::string id) {
        this->init_condition = init_condition;
        this->end_condition = end_condition;
        this->block = block;
        this->id = id;
    }
    std::string getId() {
        return this->id;
    }
    ASTExpression * getInit_condition() {
        return this->init_condition;
    }
    ASTExpression * getEnd_condition() {
        return this->end_condition;
    }
    ASTBlockStatement * getBlock() {
        return this->block;
    }
    ~ASTForStatement() {

    }
};

class ASTReturnStatement : public ASTStatement
{
    ASTExpression * expr;
public:
    ASTReturnStatement(ASTExpression * expr) {
        this->expr = expr;
    }
    ASTExpression * getExpr() {
        return this->expr;
    }
    ~ASTReturnStatement() {

    }
};

class ASTContinueStatement : public ASTStatement
{
public:
    ASTContinueStatement() {

    }
    ~ASTContinueStatement() {

    }
};

class ASTBreakStatement : public ASTStatement
{
public:
    ASTBreakStatement() {

    }
    ~ASTBreakStatement() {

    }
};

class ASTLocation : public ASTExpression
{
public:
    ASTLocation() {

    }
    virtual ~ASTLocation() {

    }
};

class ASTVarLocation : public ASTLocation 
{
    std::string id;
public:
    ASTVarLocation(std::string id) {
        this->id = id;
    }
    std::string getId() {
        return this->id;
    }
    ~ASTVarLocation() {

    }
};

class ASTArrayLocation : public ASTLocation
{
    std::string id;
    ASTExpression * index;
public:
    ASTArrayLocation(std::string id, ASTExpression * index) {
        this->id = id;
        this->index = index;
    }
    std::string getId() {
        return this->id;
    }
    ASTExpression * getIndex() {
        return this->index;
    }
    ~ASTArrayLocation() {

    }
};

class ASTLiteralExpression : public ASTExpression
{
public:
    ASTLiteralExpression() {

    }
    virtual ~ASTLiteralExpression() {

    }
};

class ASTIntegerLiteralExpression : public ASTLiteralExpression
{
    int value;
public:
    ASTIntegerLiteralExpression(int value) {
        this->value = value;
    }
    int getValue() {
        return this->value;
    }
    ~ASTIntegerLiteralExpression() {

    }
};

class ASTCharLiteralExpression : public ASTLiteralExpression
{
    char value;
public:
    ASTCharLiteralExpression(char value) {
        this->value = value;
    }
    char getValue() {
        return this->value;
    }
    ~ASTCharLiteralExpression() {

    }
};

class ASTTrueLiteralExpression : public ASTLiteralExpression
{
public:
    ASTTrueLiteralExpression() {

    }
    bool getValue() {
        return true;
    }
    ~ASTTrueLiteralExpression() {

    }
};

class ASTFalseLiteralExpression : public ASTLiteralExpression
{
public:
    ASTFalseLiteralExpression() {

    }
    bool getValue() {
        return false;
    }
    ~ASTFalseLiteralExpression() {

    }
};

class ASTBinaryOperationExpression : public ASTExpression
{
    ASTExpression * left;
    ASTExpression * right;
    BinOp op;
public:
    ASTBinaryOperationExpression(ASTExpression * left, ASTExpression * right, BinOp op) {
        this->left = left;
        this->right = right;
        this->op = op;
    }
    ASTExpression * getLeft() {
        return this->left;
    }
    ASTExpression * getRight() {
        return this->right;
    }
    BinOp getOp() {
        return this->op;
    }
    ~ASTBinaryOperationExpression() {

    }
};

class ASTUnaryOperationExpression : public ASTExpression
{
    ASTExpression * expr;
    UnOp op;
public:
    ASTUnaryOperationExpression(ASTExpression * expr, UnOp op) {
        this->expr = expr;
        this->op = op;
    }
    ASTExpression * getExpr() {
        return this->expr;
    }
    UnOp getOp () {
        return this->op;
    }
    ~ASTUnaryOperationExpression() {

    }
};

#endif

