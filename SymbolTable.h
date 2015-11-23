#ifndef _SymbolTable_H
#define _SymbolTable_H

#include <llvm/Analysis/Verifier.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <list>

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
	std::list<SymbolTableNode> table;
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
};

#endif