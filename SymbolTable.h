#ifndef _SymbolTable_H
#define _SymbolTable_H

#include <llvm/Analysis/Verifier.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
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
public:
	std::stack<SymbolTableNode> table;
	SymbolTable() {

	}
	~SymbolTable() {

	}
	std::map<std::string, llvm::Value *> getLocalVariables() {
		return this->table.top().localVariables;
	}
	bool lookupLocalVariables(std::string name) {
		auto it = this->getLocalVariables();
		return it.find(name) != it.end();
	}
	void declareLocalVariables(std::string name, llvm::Value * value) {
		if (!this->lookupLocalVariables(name)) {
			this->table.top().localVariables.insert(std::pair<std::string, llvm::Value *>(name, value));
		} else {
			std::cerr<<"Variable "<<name<<" already declared";
		}
	}
	llvm::Value * returnLocalVariables(std::string name) {
		if (lookupLocalVariables(name)) {
			auto it = this->getLocalVariables();
			return it.find(name)->second;
		}
		return NULL;
	}
	void pushBlock(llvm::BasicBlock * block) {
		this->table.push(SymbolTableNode(block));
	}
	void popBlock() {
		this->table.pop();
	}
	llvm::BasicBlock * topBlock() {
		return this->table.top().block;
	}
	void printTable() {
		auto i = this->table.top().localVariables;
		for (auto it = i.begin() ; it != i.end() ; it++) {
			std::cout << it->first;
		}
	}
};

#endif