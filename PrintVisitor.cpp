#ifndef _PrintVisitor_CPP
#define _PrintVisitor_CPP

#include <iostream>
#include <vector>
#include "AST.h"
#include "Visitor.h"

class PrintVisitor : public Visitor
{
public:
	void visit(ASTProgram* a) {
		std::cout<<"<program>"<<std::endl;
		if(a->getFdl() == NULL){
			std::cout<<"<field_declarations count = \"0\">"<<std::endl;
		}

		else{
			std::cout<<"<field_declarations count =\" "<<(a->getFdl())->size()<<">"<<std::endl;
			for(auto it = (a->getFdl())->begin() ; it != (a->getFdl())->end(); it++) {
				(*it)->accept(this);
			}				
		}
		std::cout<<"</method_declarations>"<<std::endl;
		if(a->getMdl() == NULL){
			std::cout<<"<method_declarations count = \"0\">"<<std::endl;
		}

		else{
			std::cout<<"<method_declarations count =\" "<<(a->getMdl())->size()<<">"<<std::endl;
			for(auto it = (a->getMdl())->begin() ; it != (a->getMdl())->end(); it++) {
				(*it)->accept(this);
			}				
		}
		std::cout<<"</method_declarations>"<<std::endl;
	}
	void visit(ASTFieldDecl* a) {
		std::cout<<parseDatatype(a->getType())<<" ";
		for(auto it = (a->getVar_id_list())->begin() ; it != (a->getVar_id_list())->end(); it++) {
			(*it)->accept(this);
		}
		for(auto it = (a->getArray_id_list())->begin() ; it != (a->getArray_id_list())->end(); it++) {
			(*it)->accept(this);
		}
	}

	void visit(ASTVarIdentifier* a) {
		std::cout<<" "<<a->getId();
	}

	void visit(ASTArrayIdentifier* a) {
		std::cout<<" "<<a->getId()<<"["<<a->getSize()<<"]";
	}

	void visit(ASTMethodDecl* a) {
		std::cout<<a->getId()<<" "<<parseDatatype(a->getReturnType())<<" ";
		for(auto it = (a->getArguments())->begin() ; it != (a->getArguments())->end(); it++) {
				(*it)->accept(this);
			}
			/* Block statement is called*/
		a->getBlock()->accept(this);
	}
	void visit(ASTTypeIdentifier* a) {
		std::cout<<a->getId()<<" "<<parseDatatype(a->getType())<<std::endl;
	}

	void visit(ASTBlockStatement* a) {
		std::cout<<"{"<<std::endl;
		for(auto it = (a->getStmtlist())->begin() ; it != (a->getStmtlist())->end(); it++) {
				(*it)->accept(this);
			}
		std::cout<<"}"<<std::endl;
	}
	void visit(ASTLocation* a) {
		std::cout<<"";
	}
	void visit(ASTExpression* a) {
		std::cout<<"";
	}
	void visit(ASTAssignmentStatement* a) {
		std::cout<<"";
	}
	void visit(ASTMethodCall* a) {
		std::cout<<"";
	}
	void visit(ASTNormalMethod* a) {
		std::cout<<"";
	}
	void visit(ASTCalloutMethod* a) {
		std::cout<<"";
	}
	void visit(ASTIfStatement* a) {
		std::cout<<"";
	}
	void visit(ASTForStatement* a) {
		std::cout<<"";
	}
	void visit(ASTReturnStatement* a) {
		std::cout<<"";
	}
	void visit(ASTVarLocation* a) {
		std::cout<<"";
	}
	void visit(ASTArrayLocation* a) {
		std::cout<<"";
	}
	void visit(ASTContinueStatement* a) {
		std::cout<<"";
	}
	void visit(ASTBreakStatement* a) {
		std::cout<<"";
	}
	void visit(ASTLiteralExpression* a) {
		std::cout<<"";
	}
	void visit(ASTIntegerLiteralExpression* a) {
		std::cout<<"";
	}
	void visit(ASTCharLiteralExpression* a) {
		std::cout<<"";
	}
	void visit(ASTTrueLiteralExpression* a) {
		std::cout<<"";
	}
	void visit(ASTFalseLiteralExpression* a) {
		std::cout<<"";
	}
	void visit(ASTBinaryOperationExpression* a) {
		std::cout<<"";
	}
	void visit(ASTUnaryOperationExpression* a) {
		std::cout<<"";
	}
};
#ifdef TEST

int main()
{
	ASTProgram *obj = new ASTProgram("yolo",NULL,NULL);
	obj->accept(new PrintVisitor());
	return 0;
}

#endif
#endif
