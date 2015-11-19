#ifndef _CodeGenVisitor_H
#define _CodeGenVisitor_H

#include <llvm/Analysis/Verifier.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include "SymbolTable.h"
#include "AST.h"
#include "Visitor.h"

llvm::IRBuilder<> Builder(llvm::getGlobalContext());

class CodeGenVisitor : public Visitor
{
private:
    llvm::Module * module;
    ASTProgram * start;
    llvm::Function * mainFunction;
    SymbolTable symbolTable;
public:
    CodeGenVisitor(ASTProgram * start) {
        llvm::LLVMContext &Context = llvm::getGlobalContext();
        module = new llvm::Module(start->getId(), Context);
        this->start = start;
        llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()), false);
        mainFunction = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, "main", module);
    }
    ~CodeGenVisitor() {
    }
    void codeGen() {
        llvm::BasicBlock *block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", mainFunction, 0);
        symbolTable.pushBlock(block);
        this->visit(start);
        symbolTable.popBlock();
        module->dump();
    }
    llvm::Value * ErrorHandler(const char * error) {
        std::cerr << error;
        return 0;
    }
    void * visit(ASTProgram * node) {
        if (node->getFdl()) {
            for(auto it = (node->getFdl())->begin() ; it != (node->getFdl())->end(); it++) {
                this->visit(*it);
            }               
        }
        if (node->getMdl()) {
            for(auto it = (node->getMdl())->begin() ; it != (node->getMdl())->end(); it++) {
                this->visit(*it);
            }               
        }
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTFieldDecl * node) {
        if (node->getVar_id_list()) {
            for(auto it = (node->getVar_id_list())->begin() ; it != (node->getVar_id_list())->end(); it++) {
                this->visit(*it);            
            }   
        }
        if (node->getArray_id_list()) {
            for(auto it = (node->getArray_id_list())->begin() ; it != (node->getArray_id_list())->end(); it++) {
                this->visit(*it);
            }
        }
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTVarDecl * node) {
        if (node->getId_list()) {
            for(auto it = (node->getId_list())->begin() ; it != (node->getId_list())->end(); it++) {
                this->visit(*it);
            }
        }
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTIdentifier * node) {
        ASTVarIdentifier * varIdentifier = dynamic_cast<ASTVarIdentifier *>(node);
        ASTArrayIdentifier * arrayIdentifier = dynamic_cast<ASTArrayIdentifier *>(node);
        if (varIdentifier) 
            return this->visit(varIdentifier);
        if (arrayIdentifier)
            return this->visit(arrayIdentifier);
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTVarIdentifier * node) {
        llvm::AllocaInst * allocaInst = NULL;
        allocaInst = new llvm::AllocaInst(llvm::Type::getInt64Ty(llvm::getGlobalContext()), node->getId(), symbolTable.topBlock());
        symbolTable.declareLocalVariables(node->getId(), allocaInst);
        return allocaInst;
    }
    void * visit(ASTArrayIdentifier * node) {
        llvm::AllocaInst * allocaInst = NULL;
        allocaInst = new llvm::AllocaInst(llvm::ArrayType::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), node->getSize()),node->getId(),symbolTable.topBlock());
        symbolTable.declareLocalVariables(node->getId(), allocaInst);
        return allocaInst;
    }
    void * visit(ASTMethodDecl * node) {
        if (node->getArguments()) {
            for (auto it = (node->getArguments())->begin(); it != (node->getArguments())->end(); it++) {
                this->visit(*it);
            }
        }
        this->visit(node->getBlock());
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTTypeIdentifier * node) {
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTStatement * node) { 
        ASTAssignmentStatement * assignmentStatement = dynamic_cast<ASTAssignmentStatement *>(node);
        ASTBlockStatement * blockStatement = dynamic_cast<ASTBlockStatement *>(node);
        ASTMethodCall * methodCall = dynamic_cast<ASTMethodCall *>(node);
        ASTIfStatement * ifStatement = dynamic_cast<ASTIfStatement *>(node);
        ASTForStatement * forStatement = dynamic_cast<ASTForStatement *>(node);
        ASTReturnStatement * returnStatement = dynamic_cast<ASTReturnStatement *>(node);
        ASTContinueStatement * continueStatement = dynamic_cast<ASTContinueStatement *>(node);
        ASTBreakStatement * breakStatement = dynamic_cast<ASTBreakStatement *>(node);
        if (assignmentStatement) {
            return this->visit(assignmentStatement);
        }
        if (blockStatement) {
            return this->visit(blockStatement);
        }
        if (methodCall) {
            return this->visit(methodCall);
        }
        if (ifStatement) {
            return this->visit(ifStatement);
        }
        if (forStatement) {
            return this->visit(forStatement);
        }
        if (returnStatement) {
            return this->visit(returnStatement);
        }
        if (continueStatement) {
            return this->visit(continueStatement);
        }
        if (breakStatement) {
            return this->visit(breakStatement);
        }
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTExpression * node) {
        ASTBinaryOperationExpression * binaryOperationExpression = dynamic_cast<ASTBinaryOperationExpression *>(node);
        ASTLiteralExpression * literalExpression = dynamic_cast<ASTLiteralExpression *>(node);
        ASTMethodCall * methodCall = dynamic_cast<ASTMethodCall *>(node);
        ASTUnaryOperationExpression * unaryOperationExpression = dynamic_cast<ASTUnaryOperationExpression *>(node);
        ASTLocation * location = dynamic_cast<ASTLocation *>(node);
        ASTCalloutArg * calloutArg = dynamic_cast<ASTCalloutArg *>(node);
        if (binaryOperationExpression) 
            return this->visit(binaryOperationExpression);
        else if (literalExpression) 
            return this->visit(literalExpression);
        else if (methodCall)
            return this->visit(methodCall);
        else if (unaryOperationExpression)
            return this->visit(unaryOperationExpression);
        else if (location)
            return this->visit(location);
        else if (calloutArg)
            return this->visit(calloutArg);
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTBlockStatement * node) {
        if(node->getId_list()){
            for(auto it = (node->getId_list())->begin() ; it != (node->getId_list())->end(); it++) {
                this->visit(*it);
            }
        }
        if(node->getStmtlist()){
            for(auto it = (node->getStmtlist())->begin() ; it != (node->getStmtlist())->end(); it++) {
                this->visit(*it);
            }
        }
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTAssignmentStatement * node) {
        llvm::Value * location = NULL;
        ASTVarLocation * varLocation = dynamic_cast<ASTVarLocation *>(node->getLocation());
        ASTArrayLocation * arrayLocation = dynamic_cast<ASTArrayLocation *>(node->getLocation());
        if (arrayLocation) {
            if (!symbolTable.lookupLocalVariables(arrayLocation->getId())) {
                return ErrorHandler("Variable Not Declared");
            }
            llvm::Value * val = symbolTable.returnLocalVariables(arrayLocation->getId());
            llvm::Value * index = static_cast<llvm::Value *>(this->visit(arrayLocation->getIndex()));
            location = llvm::GetElementPtrInst::CreateInBounds(val, index, "tmp", symbolTable.topBlock());
        }
        if (varLocation) {
            if (!symbolTable.lookupLocalVariables(varLocation->getId())) {
                return ErrorHandler("Variable Not Declared");
            }
            location = symbolTable.returnLocalVariables(varLocation->getId());
        }
        llvm::Value * expr = static_cast<llvm::Value *>(this->visit(node->getExpr()));
        return new llvm::StoreInst(expr, location, false, symbolTable.topBlock());
    }
    void * visit(ASTMethodCall * node) {
        ASTCalloutMethod * calloutMethod = dynamic_cast<ASTCalloutMethod *>(node);
        ASTNormalMethod * normalMethod = dynamic_cast<ASTNormalMethod *>(node);
        if (calloutMethod) 
            return this->visit(calloutMethod);
        if (normalMethod) 
            return this->visit(normalMethod);
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTNormalMethod * node) {
        if (node->getArguments()) {
            for (auto it = (node->getArguments())->begin(); it != (node->getArguments())->end(); it++) {
                this->visit(*it);
            }
        }
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTCalloutMethod * node) {
        if (node->getArguments()) {
            for (auto it = (node->getArguments())->begin(); it != (node->getArguments())->end(); it++) {
                this->visit(*it);
            }
        }
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTCalloutArg * node) { 
        ASTStringCalloutArg * stringCalloutArg = dynamic_cast<ASTStringCalloutArg *>(node);
        ASTExpressionCalloutArg * expressionCalloutArg = dynamic_cast<ASTExpressionCalloutArg *>(node);
        if (stringCalloutArg) 
            return this->visit(stringCalloutArg);
        if (expressionCalloutArg)
            return this->visit(expressionCalloutArg);
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTStringCalloutArg * node) {
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTExpressionCalloutArg * node) {
        this->visit(node->getArgument());
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTIfStatement * node) {
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTForStatement * node) {
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTReturnStatement * node) {
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTContinueStatement * node) {
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTBreakStatement * node) {
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTLocation * node) {
        ASTArrayLocation * arrayLocation = dynamic_cast<ASTArrayLocation *>(node);
        ASTVarLocation * varLocation = dynamic_cast<ASTVarLocation *>(node);
        if (arrayLocation) 
            return this->visit(arrayLocation);
        if (varLocation)
            return this->visit(varLocation);
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTVarLocation * node) {
        if (!symbolTable.lookupLocalVariables(node->getId())) {
            return ErrorHandler("Variable Not Declared");
        }
        llvm::Value * val = symbolTable.returnLocalVariables(node->getId());
        if (val)
            return new llvm::LoadInst(val, "tmp", symbolTable.topBlock());
        return ErrorHandler("Variable Not Initilized");
    }
    void * visit(ASTArrayLocation * node) {
        if (!symbolTable.lookupLocalVariables(node->getId())) {
            return ErrorHandler("Variable Not Declared");
        }
        llvm::Value * index = static_cast<llvm::Value *>(this->visit(node->getIndex()));
        llvm::Value * val = symbolTable.returnLocalVariables(node->getId());
        llvm::Value * offset = llvm::GetElementPtrInst::CreateInBounds(val, index, "tmp", symbolTable.topBlock());
        if (val)
            return new llvm::LoadInst(offset, "tmp", symbolTable.topBlock());
        return ErrorHandler("Variable Not Initilized");
    }
    void * visit(ASTLiteralExpression * node) {
        ASTIntegerLiteralExpression * integerLiteralExpression = dynamic_cast<ASTIntegerLiteralExpression *>(node);
        ASTCharLiteralExpression * charLiteralExpression = dynamic_cast<ASTCharLiteralExpression *>(node);
        ASTTrueLiteralExpression * trueLiteralExpression = dynamic_cast<ASTTrueLiteralExpression *>(node);
        ASTFalseLiteralExpression * falseLiteralExpression = dynamic_cast<ASTFalseLiteralExpression *>(node);
        if (integerLiteralExpression) {
            return this->visit(integerLiteralExpression);
        }
        if (charLiteralExpression) {
            return this->visit(charLiteralExpression);
        }
        if (trueLiteralExpression) {
            return this->visit(trueLiteralExpression);
        }
        if (falseLiteralExpression) {
            return this->visit(falseLiteralExpression);
        }
        return ErrorHandler("Should Never Be Called"); // Should never be called.
    }
    void * visit(ASTIntegerLiteralExpression * node) {
        return llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), node->getValue(), true);
    }
    void * visit(ASTCharLiteralExpression * node) {
        return llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), node->getValue(), true);
    }
    void * visit(ASTTrueLiteralExpression * node) {
        return llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), node->getValue(), true);
    }
    void * visit(ASTFalseLiteralExpression * node) {
        return llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), node->getValue(), true);
    }
    void * visit(ASTBinaryOperationExpression * node) {
        switch(node->getOp()) {
             case BinOp::plus_op:
                return llvm::BinaryOperator::Create(llvm::Instruction::Add, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())), "tmp", symbolTable.topBlock());
            case BinOp::minus_op: 
                return llvm::BinaryOperator::Create(llvm::Instruction::Sub, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())), "tmp", symbolTable.topBlock());
            case BinOp::multiply_op: 
                return llvm::BinaryOperator::Create(llvm::Instruction::Mul, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())), "tmp", symbolTable.topBlock());
            case BinOp::divide_op: 
                return llvm::BinaryOperator::Create(llvm::Instruction::SDiv, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())), "tmp", symbolTable.topBlock());
            case BinOp::modulo_op: 
                return llvm::BinaryOperator::Create(llvm::Instruction::SRem, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())), "tmp", symbolTable.topBlock());
            case BinOp::lessthan_op: 
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SLT, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())),"tmp", symbolTable.topBlock());
            case BinOp::greaterthan_op: 
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SGT, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())),"tmp", symbolTable.topBlock());
            case BinOp::lessequal_op: 
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SLE, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())),"tmp", symbolTable.topBlock());
            case BinOp::greaterequal_op: 
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SGE, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())),"tmp", symbolTable.topBlock());
            case BinOp::notequal_op: 
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_NE, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())),"tmp", symbolTable.topBlock());
            case BinOp::equalequal_op: 
                return llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_EQ, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())),"tmp", symbolTable.topBlock());
            case BinOp::and_op: 
                return llvm::BinaryOperator::Create(llvm::Instruction::And, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())), "tmp", symbolTable.topBlock());
            case BinOp::or_op: 
                return llvm::BinaryOperator::Create(llvm::Instruction::Or, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())), "tmp", symbolTable.topBlock());
        }
        return ErrorHandler("No Known BinaryOperator");
    }
    void * visit(ASTUnaryOperationExpression * node) {
        switch(node->getOp()) {
            case UnOp::minus_op: 
                return llvm::BinaryOperator::Create(llvm::Instruction::Sub, llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 0, true), static_cast<llvm::Value*>(this->visit(node->getExpr())), "tmp", symbolTable.topBlock());
            case UnOp::not_op: 
                return llvm::BinaryOperator::Create(llvm::Instruction::Xor, llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), -1, true), static_cast<llvm::Value*>(this->visit(node->getExpr())), "tmp", symbolTable.topBlock());
        }
        return ErrorHandler("No Known UnaryOperator");
    }
};
#ifdef TEST


#endif
#endif
