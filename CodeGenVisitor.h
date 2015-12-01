#ifndef _CodeGenVisitor_H
#define _CodeGenVisitor_H

#include <llvm/Analysis/Verifier.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/PassManager.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Pass.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MathExtras.h>
#include <iostream>
#include "SymbolTable.h"
#include "AST.h"
#include "Visitor.h"

class CodeGenVisitor : public Visitor
{
private:
    llvm::Module * module;
    ASTProgram * start;
    llvm::Function * mainFunction;
    SymbolTable symbolTable;
public:
    CodeGenVisitor(ASTProgram * start) {
        module = new llvm::Module(start->getId(), llvm::getGlobalContext());
        module->setTargetTriple("x86_64-pc-linux-gnu");
        this->start = start;
        llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm::getGlobalContext()), false);
        mainFunction = llvm::Function::Create(ftype, llvm::GlobalValue::ExternalLinkage, "main", module);
        // module->dump();
    }
    ~CodeGenVisitor() {
    }
    void codeGen() {
        llvm::BasicBlock *block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", mainFunction, 0);
        symbolTable.pushBlock(block);
        this->visit(start);
        block = symbolTable.topBlock();
        symbolTable.popBlock();
        llvm::ReturnInst::Create(llvm::getGlobalContext(), block);
        llvm::verifyModule(*module, llvm::PrintMessageAction);
        llvm::PassManager PM;
        PM.add(llvm::createPrintModulePass(&llvm::outs()));
        PM.run(*module);
    }
    llvm::Value * ErrorHandler(const char * error) {
        std::cerr << error;
        // return 0;
        exit(0);
    }
    void * visit(ASTProgram * node) {
        if (node->getFdl()) {
            for(auto it = (node->getFdl())->begin() ; it != (node->getFdl())->end(); it++) {
                this->visit(*it);
            }               
        }
        llvm::Function * iterator = NULL;
        llvm::Function * userMain = NULL;
        if (node->getMdl()) {
            for(auto it = (node->getMdl())->begin() ; it != (node->getMdl())->end(); it++) {
                iterator = static_cast<llvm::Function *>(this->visit(*it));
                if ((*it)->getId() == "main" && !userMain) {
                    userMain = iterator;
                }
                if ((*it)->getId() == "main" && (*it)->getArguments()) {
                    return ErrorHandler("Main cannot have any arguments");
                }
            }               
        }
        if (!userMain)
            return ErrorHandler("No main Found");
        else {
            llvm::CallInst::Create(userMain, "", symbolTable.topBlock());
        }
        return NULL;
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
        return NULL;
    }
    void * visit(ASTVarDecl * node) {
        if (node->getId_list()) {
            for(auto it = (node->getId_list())->begin() ; it != (node->getId_list())->end(); it++) {
                this->visit(*it);
            }
        }
        return NULL;
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
        if(symbolTable.topBlock() == symbolTable.bottomBlock()) {
            llvm::GlobalVariable * globalInteger = new llvm::GlobalVariable(*module, llvm::Type::getInt64Ty(llvm::getGlobalContext()), false, llvm::GlobalValue::CommonLinkage, NULL, node->getId());
            globalInteger->setInitializer(llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(64, llvm::StringRef("0"), 10)));
            symbolTable.declareLocalVariables(node->getId(), globalInteger);
            return globalInteger;
        }
        else {
            llvm::AllocaInst * allocaInst = new llvm::AllocaInst(llvm::Type::getInt64Ty(llvm::getGlobalContext()), node->getId(), symbolTable.topBlock());
            symbolTable.declareLocalVariables(node->getId(), allocaInst);
            return allocaInst;
        }
    }
    void * visit(ASTArrayIdentifier * node) {
        if (node->getSize() <= 1) {
            return ErrorHandler("Invalid Array Size");
        }
        llvm::GlobalVariable* variable = new llvm::GlobalVariable(*module, llvm::ArrayType::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), node->getSize()), false, llvm::GlobalValue::CommonLinkage, NULL, node->getId());
        variable->setInitializer(llvm::ConstantAggregateZero::get(llvm::ArrayType::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), node->getSize())));
        symbolTable.declareLocalVariables(node->getId(), variable);
        return variable;
    }
    llvm::Type * parseType(Datatype type) {
        switch(type) {
            case Datatype::int_type: 
                return llvm::Type::getInt64Ty(llvm::getGlobalContext());
            case Datatype::void_type: 
                return llvm::Type::getVoidTy(llvm::getGlobalContext());
            case Datatype::bool_type: 
                return llvm::Type::getInt64Ty(llvm::getGlobalContext());                   
        }
        return NULL;
    }
    void * visit(ASTMethodDecl * node) {
        std::vector<llvm::Type*> argTypes;
        if (node->getArguments()) {
            for (auto it = (node->getArguments())->begin(); it != (node->getArguments())->end(); it++) {
                argTypes.push_back(parseType((*it)->getType()));
            }
        } 

        llvm::FunctionType *ftype = llvm::FunctionType::get(parseType(node->getReturnType()), llvm::makeArrayRef(argTypes), false);
        llvm::Function *function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, node->getId(), module);
        llvm::BasicBlock *block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", function, 0); 
        symbolTable.pushBlock(block);
        if (node->getArguments()) {
            auto it2 = function->arg_begin();
            for (auto it = (node->getArguments())->begin(); it != (node->getArguments())->end(); it++, it2++) {
                llvm::Value * arg = it2;
                arg->setName((*it)->getId());
                llvm::AllocaInst * allocaInst = NULL;
                allocaInst = new llvm::AllocaInst(llvm::Type::getInt64Ty(llvm::getGlobalContext()), (*it)->getId(), symbolTable.topBlock());
                new llvm::StoreInst(arg, allocaInst, false, symbolTable.topBlock());
                symbolTable.declareLocalVariables((*it)->getId(), allocaInst);
            }
        }
        // symbolTable.pushBlock();
        this->visit(node->getBlock());
        if(!symbolTable.topBlock()->getTerminator()) {
            if(node->getReturnType() == Datatype::void_type)
                llvm::ReturnInst::Create(llvm::getGlobalContext(), symbolTable.topBlock());
            else 
                llvm::ReturnInst::Create(llvm::getGlobalContext(), llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 0, true), symbolTable.topBlock());
        }
        
        symbolTable.popBlock();
        return function;
    }
    void * visit(ASTTypeIdentifier * node) {
        return NULL; 
    }
    void * visit(ASTStatement * node) { 
        llvm::BasicBlock * block = symbolTable.topBlock();
        if (block->getTerminator()) {
            // terminate any and all instructions which end the current block when there are still instructions to do
            llvm::Instruction * terminator = block->getTerminator();
            terminator->eraseFromParent();
        }
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
            symbolTable.pushBlock(NULL);
            this->visit(blockStatement);
            symbolTable.popBlock();
            return NULL;
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
                ASTReturnStatement * returnStatement = dynamic_cast<ASTReturnStatement *>(*it);
                ASTBreakStatement * breakStatement = dynamic_cast<ASTBreakStatement *>(*it);
                ASTContinueStatement * continueStatement = dynamic_cast<ASTContinueStatement *>(*it);
                if (returnStatement or breakStatement or continueStatement) 
                    break;                
            }
        }
        return NULL;
    }
    void * visit(ASTAssignmentStatement * node) {
        llvm::Value * location = NULL;
        llvm::Value * existingValue = NULL;
        ASTVarLocation * varLocation = dynamic_cast<ASTVarLocation *>(node->getLocation());
        ASTArrayLocation * arrayLocation = dynamic_cast<ASTArrayLocation *>(node->getLocation());
        if (arrayLocation) {
            if (!symbolTable.lookupGlobalVariables(arrayLocation->getId())) {
                return ErrorHandler("Variable Not Declared");
            }
            std::vector <llvm::Value *> index;
            index.push_back(llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(64, llvm::StringRef("0"), 10)));
            index.push_back(static_cast<llvm::Value *>(this->visit(arrayLocation->getIndex())));
            llvm::Value * val = symbolTable.returnLocalVariables(arrayLocation->getId());
            location = llvm::GetElementPtrInst::CreateInBounds(val, index, "tmp", symbolTable.topBlock());
        }
        if (varLocation) {
            if (!symbolTable.lookupGlobalVariables(varLocation->getId())) {
                return ErrorHandler("Variable Not Declared");
            }
            location = symbolTable.returnLocalVariables(varLocation->getId());
        }
        llvm::Value * expr = static_cast<llvm::Value *>(this->visit(node->getExpr()));
        switch(node->getOp()) {
            case AssignOp::plus_equal:
                existingValue = new llvm::LoadInst(location, "load", symbolTable.topBlock());
                expr = llvm::BinaryOperator::Create(llvm::Instruction::Add, existingValue, expr, "tmp", symbolTable.topBlock());
                break;
            case AssignOp::minus_equal: 
                existingValue = new llvm::LoadInst(location, "load", symbolTable.topBlock());
                expr = llvm::BinaryOperator::Create(llvm::Instruction::Sub, existingValue, expr, "tmp", symbolTable.topBlock());
                break;
            case AssignOp::equal: 
                break;
        }
        if (!expr->getType()->isIntegerTy(64)) {
            return ErrorHandler("RHS of assignment statement is invalid");
        }
        if (!location->getType()->isPointerTy()) {
            return ErrorHandler("LHS of assignment statement is invalid");
        }
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
        if (node->getId() == "main") {
            return ErrorHandler("You can't call main");
        }
        std::vector<llvm::Value *> args;
        llvm::Function * function = module->getFunction(node->getId());
        if (!function) {
            return ErrorHandler("No Function Defined");
        }
        if (!function->isVarArg() && (node->getArguments()) && (function->getArgumentList().size() != node->getArguments()->size())) {
            return ErrorHandler("Invalid Number of Arguments");
        }
        if (node->getArguments()) {
            for (auto it = (node->getArguments())->begin(); it != (node->getArguments())->end(); it++) {
                args.push_back(static_cast<llvm::Value *>(this->visit(*it)));
            }
        }
        if (function->getReturnType()->isVoidTy()) {
            return llvm::CallInst::Create(function, llvm::makeArrayRef(args), "", symbolTable.topBlock());    
        }
        return llvm::CallInst::Create(function, llvm::makeArrayRef(args), node->getId(), symbolTable.topBlock());
    }
    void * visit(ASTCalloutMethod * node) {
        llvm::Function * function = module->getFunction(node->getMethod_name());
        if (!function) {
            llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), true);
            function = llvm::Function::Create(ftype, llvm::GlobalValue::ExternalLinkage, node->getMethod_name(), module);
        }
        std::vector<llvm::Value *> args;
        if (node->getArguments()) {
            for (auto it = (node->getArguments())->begin(); it != (node->getArguments())->end(); it++) {
                args.push_back(static_cast<llvm::Value *>(this->visit(*it)));
            }
        }
        llvm::CallInst *call = llvm::CallInst::Create(function, llvm::makeArrayRef(args), node->getMethod_name(), symbolTable.topBlock());
        return call;
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
        llvm::GlobalVariable* variable = new llvm::GlobalVariable(*module, llvm::ArrayType::get(llvm::IntegerType::get(llvm::getGlobalContext(), 8), node->getArgument().size() + 1), true, llvm::GlobalValue::InternalLinkage, NULL, "string");
        variable->setInitializer(llvm::ConstantDataArray::getString(llvm::getGlobalContext(), node->getArgument(), true));
        return variable;
    }
    void * visit(ASTExpressionCalloutArg * node) {
        return this->visit(node->getArgument());
    }
    void * visit(ASTIfStatement * node) {
        llvm::BasicBlock * entryBlock = symbolTable.topBlock();
        llvm::Value * condition = static_cast<llvm::Value *>(this->visit(node->getCondition()));
        llvm::ICmpInst * comparison = new llvm::ICmpInst(*entryBlock, llvm::ICmpInst::ICMP_NE, condition, llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 0, true), "tmp");
        llvm::BasicBlock * ifBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "ifBlock", entryBlock->getParent());
        llvm::BasicBlock * mergeBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "mergeBlock", entryBlock->getParent());

        llvm::BasicBlock * returnedBlock = NULL;

        symbolTable.pushBlock(ifBlock);
        this->visit(node->getIf_block());
        returnedBlock = symbolTable.topBlock();
        symbolTable.popBlock();
        if (!returnedBlock->getTerminator()) {
            llvm::BranchInst::Create(mergeBlock, returnedBlock);
        }
        if (node->getElse_block()) {
            llvm::BasicBlock * elseBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "elseBlock", entryBlock->getParent());
            
            symbolTable.pushBlock(elseBlock);
            this->visit(node->getElse_block());
            returnedBlock = symbolTable.topBlock();
            symbolTable.popBlock();
            if (!returnedBlock->getTerminator()) {
                llvm::BranchInst::Create(mergeBlock, returnedBlock);
            }
            llvm::BranchInst::Create(ifBlock, elseBlock, comparison, entryBlock);
        } else {
            llvm::BranchInst::Create(ifBlock, mergeBlock, comparison, entryBlock);
        }
        auto localVariables = symbolTable.getLocalVariables();
        symbolTable.popBlock();
        symbolTable.pushBlock(mergeBlock);
        symbolTable.setLocalVariables(localVariables);
        return NULL;
    }
    void * visit(ASTForStatement * node) {
        if (!symbolTable.lookupGlobalVariables(node->getId())) {
            return ErrorHandler("Variable Not Declared");
        }
        llvm::BasicBlock * entryBlock = symbolTable.topBlock();
        llvm::BasicBlock * headerBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "loop_header", entryBlock->getParent(), 0);
        llvm::BasicBlock * bodyBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "loop_body", entryBlock->getParent(), 0);
        llvm::BasicBlock * afterLoopBlock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "after_loop", entryBlock->getParent(), 0);

        symbolTable.pushBCS(afterLoopBlock, headerBlock);

        new llvm::StoreInst(static_cast<llvm::Value *>(this->visit(node->getInit_condition())), symbolTable.returnLocalVariables(node->getId()), false, entryBlock);
        llvm::Value * val = new llvm::LoadInst(symbolTable.returnLocalVariables(node->getId()), "load", headerBlock);
        llvm::ICmpInst * comparison = new llvm::ICmpInst(*headerBlock, llvm::ICmpInst::ICMP_NE, val, static_cast<llvm::Value *>(this->visit(node->getEnd_condition())), "tmp");
        llvm::BranchInst::Create(bodyBlock, afterLoopBlock, comparison, headerBlock);
        llvm::BranchInst::Create(headerBlock, entryBlock);

        symbolTable.pushBlock(bodyBlock);
        this->visit(node->getBlock());
        bodyBlock = symbolTable.topBlock();
        symbolTable.popBlock();
        if (!bodyBlock->getTerminator()) {
            llvm::BranchInst::Create(headerBlock, bodyBlock);
        }

        auto localVariables = symbolTable.getLocalVariables();
        symbolTable.popBlock();
        symbolTable.pushBlock(afterLoopBlock);
        symbolTable.setLocalVariables(localVariables);

        symbolTable.popBCS();
        return NULL;
    }
    void * visit(ASTReturnStatement * node) {
        llvm::Function * function = symbolTable.topBlock()->getParent();
        llvm::Type * type = function->getReturnType();
        if (type->isVoidTy()) {
            if(node->getExpr()) {
                return ErrorHandler("Unknown Return for Void Type");
            } else {
                return llvm::ReturnInst::Create(llvm::getGlobalContext(),symbolTable.topBlock());
            }
        } else {
            if (node->getExpr()) {
                llvm::Value * expression = static_cast<llvm::Value *>(this->visit(node->getExpr()));
                return llvm::ReturnInst::Create(llvm::getGlobalContext(), expression, symbolTable.topBlock());
            } else {
                return ErrorHandler("Void type return for non void function");
            }
        }
    }
    void * visit(ASTContinueStatement * node) {
        llvm::BasicBlock * block = symbolTable.getCS();
        if (!block) {
            return ErrorHandler("Incorrect continue usage");
        }
        llvm::BasicBlock * curBlock = symbolTable.topBlock();
        return llvm::BranchInst::Create(block, curBlock);
    }
    void * visit(ASTBreakStatement * node) {
        llvm::BasicBlock * block = symbolTable.getBS();
        if (!block) {
            return ErrorHandler("Incorrect break usage");
        }
        llvm::BasicBlock * curBlock = symbolTable.topBlock();
        auto localVariables = symbolTable.getLocalVariables();
        symbolTable.popBlock();
        symbolTable.pushBlock(block);
        symbolTable.setLocalVariables(localVariables);
        return llvm::BranchInst::Create(block, curBlock);
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
        if (!symbolTable.lookupGlobalVariables(node->getId())) {
            return ErrorHandler("Variable Not Declared");
        }
        llvm::Value * val = symbolTable.returnLocalVariables(node->getId());
        if (val)
            return new llvm::LoadInst(val, "tmp", symbolTable.topBlock());
        return ErrorHandler("Variable Not Initilized");
    }
    void * visit(ASTArrayLocation * node) {
        if (!symbolTable.lookupGlobalVariables(node->getId())) {
            return ErrorHandler("Variable Not Declared");
        }
        std::vector <llvm::Value *> index;
        index.push_back(llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(64, llvm::StringRef("0"), 10)));
        index.push_back(static_cast<llvm::Value *>(this->visit(node->getIndex())));
        llvm::Value * val = symbolTable.returnLocalVariables(node->getId());
        llvm::Value * offset = llvm::GetElementPtrInst::CreateInBounds(val, index, "tmp", symbolTable.topBlock());
        if (val) {
            llvm::LoadInst * load = new llvm::LoadInst(offset, "tmp", symbolTable.topBlock());
            return load;
        }
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
                return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SLT, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())),"tmp", symbolTable.topBlock()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", symbolTable.topBlock());
            case BinOp::greaterthan_op: 
                return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SGT, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())),"tmp", symbolTable.topBlock()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", symbolTable.topBlock());
            case BinOp::lessequal_op: 
                return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SLE, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())),"tmp", symbolTable.topBlock()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", symbolTable.topBlock());
            case BinOp::greaterequal_op: 
                return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SGE, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())),"tmp", symbolTable.topBlock()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", symbolTable.topBlock());
            case BinOp::notequal_op: 
                return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_NE, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())),"tmp", symbolTable.topBlock()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", symbolTable.topBlock());
            case BinOp::equalequal_op: 
                return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_EQ, static_cast<llvm::Value*>(this->visit(node->getLeft())), static_cast<llvm::Value*>(this->visit(node->getRight())),"tmp", symbolTable.topBlock()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", symbolTable.topBlock());
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
