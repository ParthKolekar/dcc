#ifndef _SymbolTable_H
#define _SymbolTable_H

#include <llvm/IR/BasicBlock.h>

#include <iostream>
#include <list>
#include <map>
#include <stack>

class SymbolTableNode
{
public:
    llvm::BasicBlock * block;
    std::map<std::string, llvm::Value *> localVariables;

    SymbolTableNode(llvm::BasicBlock * block) {
        this->block = block;
    }

    ~SymbolTableNode() {

    }

};

class SymbolTable
{
private:
    std::list<SymbolTableNode> table;
    std::stack<std::pair<llvm::BasicBlock *, llvm::BasicBlock *>> break_continue_stack;

public:
    SymbolTable() {

    }

    ~SymbolTable() {

    }

    std::map<std::string, llvm::Value *> getLocalVariables() {
        return this->table.front().localVariables;
    }

    void setLocalVariables(std::map<std::string, llvm::Value *> variables) {
        this->table.front().localVariables.insert(variables.begin(), variables.end());
    }

    bool lookupLocalVariables(std::string name) {
        auto it = this->getLocalVariables();
        return it.find(name) != it.end();
    }

    void declareLocalVariables(std::string name, llvm::Value * value) {
        if (!this->lookupLocalVariables(name)) {
            this->table.front().localVariables.insert(std::pair<std::string, llvm::Value *>(name, value));
        } else {
            std::cerr<<"Variable "<<name<<" already declared";
            exit(0);
        }
    }

    bool lookupGlobalVariables(std::string name) {
        return this->returnLocalVariables(name) != NULL;
    }

    llvm::Value * returnLocalVariables(std::string name) {
        for (auto it = this->table.begin(); it != this->table.end(); it++) {
            auto found_or_end = it->localVariables.find(name);
            if (found_or_end != it->localVariables.end()) {
                return found_or_end->second;
            }
        }
        return NULL;
    }

    void pushBlock(llvm::BasicBlock * block) {
        this->table.push_front(SymbolTableNode(block));
    }

    void popBlock() {
        this->table.pop_front();
    }

    llvm::BasicBlock * topBlock() {
        for (auto it = this->table.begin(); it != this->table.end(); it++) {
            if (it->block) {
                return it->block;
            }
        }
        return this->table.front().block;
    }

    llvm::BasicBlock * bottomBlock() {
        return this->table.back().block;
    }

    void printTable() {
        auto i = this->table.front().localVariables;
        for (auto it = i.begin() ; it != i.end() ; it++) {
            std::cout << it->first;
        }
    }

    void pushBCS(llvm::BasicBlock * breakSt, llvm::BasicBlock * returnSt) {
        this->break_continue_stack.push(std::make_pair(breakSt, returnSt));
    }

    void popBCS() {
        this->break_continue_stack.pop();
    }

    llvm::BasicBlock * getBS() {
        if (!this->break_continue_stack.empty()) {
            return this->break_continue_stack.top().first;
        }
        return NULL;
    }

    llvm::BasicBlock * getCS() {
        if (!this->break_continue_stack.empty()) {
            return this->break_continue_stack.top().second;
        }
        return NULL;
    }

};

#endif
