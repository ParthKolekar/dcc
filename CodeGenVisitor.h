#ifndef _CodeGenVisitor_H
#define _CodeGenVisitor_H

#include <llvm/Analysis/Verifier.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <cctype>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include "AST.h"
#include "Visitor.h"

llvm::IRBuilder<> Builder(llvm::getGlobalContext());

class CodeGenVisitor : public Visitor
{
private:
    llvm::Module * TheModule;
    std::map<std::string, llvm::Value *> NamedValues;
    ASTProgram * start;
public:
    CodeGenVisitor(ASTProgram * start) {
        llvm::LLVMContext &Context = llvm::getGlobalContext();
        TheModule = new llvm::Module("LLVM IR", Context);
        this->start = start;
    }
    ~CodeGenVisitor() {
    }
    void codeGen() {
        start->accept(this);
        TheModule->dump();
    }
    llvm::Value * ErrorHandler(const char * error) {
        std::cerr << error;
        return 0;
    }
    void * visit(ASTProgram * node) {
        return NULL;
    }
    void * visit(ASTFieldDecl * node) {
        return NULL;
    }
    void * visit(ASTVarDecl * node) {
        return NULL;
    }
    void * visit(ASTIdentifier * node) {
        return NULL;
    }
    void * visit(ASTVarIdentifier * node) {
        return NULL;
    }
    void * visit(ASTArrayIdentifier * node) {
        return NULL;
    }
    void * visit(ASTMethodDecl * node) {
        return NULL;
    }
    void * visit(ASTTypeIdentifier * node) {
        return NULL;
    }
    void * visit(ASTStatement * node) {
        return NULL;
    }
    void * visit(ASTExpression * node) {
        return NULL;
    }
    void * visit(ASTBlockStatement * node) {
        return NULL;
    }
    void * visit(ASTAssignmentStatement * node) {
        return NULL;
    }
    void * visit(ASTMethodCall * node) {
        return NULL;
    }
    void * visit(ASTNormalMethod * node) {
        return NULL;
    }
    void * visit(ASTCalloutMethod * node) {
        return NULL;
    }
    void * visit(ASTCalloutArg * node) {
        return NULL;
    }
    void * visit(ASTStringCalloutArg * node) {
        return NULL;
    }
    void * visit(ASTExpressionCalloutArg * node) {
        return NULL;
    }
    void * visit(ASTIfStatement * node) {
        return NULL;
    }
    void * visit(ASTForStatement * node) {
        return NULL;
    }
    void * visit(ASTReturnStatement * node) {
        return NULL;
    }
    void * visit(ASTContinueStatement * node) {
        return NULL;
    }
    void * visit(ASTBreakStatement * node) {
        return NULL;
    }
    void * visit(ASTLocation * node) {
        return NULL;
    }
    void * visit(ASTVarLocation * node) {
        return NULL;
    }
    void * visit(ASTArrayLocation * node) {
        return NULL;
    }
    void * visit(ASTLiteralExpression * node) {
        return NULL; // Should never be called.
    }
    void * visit(ASTIntegerLiteralExpression * node) {
        return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(64, node->getValue()));
    }
    void * visit(ASTCharLiteralExpression * node) {
        return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(64, node->getValue()));   
    }
    void * visit(ASTTrueLiteralExpression * node) {
        return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(64, node->getValue()));
    }
    void * visit(ASTFalseLiteralExpression * node) {
        return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(64, node->getValue()));
    }
    void * visit(ASTBinaryOperationExpression * node) {
        return NULL;
    }
    void * visit(ASTUnaryOperationExpression * node) {
        return NULL;
    }
};
#ifdef TEST

int main()
{
    ASTProgram * obj = new ASTProgram("yolo",NULL,NULL);
    obj->accept(new CodeGenVisitor());
    return 0;
}

#endif
#endif
