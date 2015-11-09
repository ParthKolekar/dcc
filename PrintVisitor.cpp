#ifndef _PrintVisitor_CPP
#define _PrintVisitor_CPP

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
		for(auto it = a->begin() ; it != a->end(); it++) {
				(*it)->accept(this);
			}
	}
	void visit(ASTMethodDecl*) {
		std::cout<<"";
	}
	void visit(ASTVarIdentifier*) {
		std::cout<<" "<<a->getId();
	}
	void visit(ASTArrayIdentifier*) {
		std::cout<<" "<<a->getId()<<"["<<a->getSize()<<"]";
	}
	void visit(ASTTypeIdentifier*) {
		std::cout<<"";
	}
	void visit(ASTStatement*) {
		std::cout<<"";
	}
	void visit(ASTBlockStatement*) {
		std::cout<<"";
	}
	void visit(ASTLocation*) {
		std::cout<<"";
	}
	void visit(ASTExpression*) {
		std::cout<<"";
	}
	void visit(ASTAssignmentStatement*) {
		std::cout<<"";
	}
	void visit(ASTMethodCall*) {
		std::cout<<"";
	}
	void visit(ASTNormalMethod*) {
		std::cout<<"";
	}
	void visit(ASTCalloutMethod*) {
		std::cout<<"";
	}
	void visit(ASTIfStatement*) {
		std::cout<<"";
	}
	void visit(ASTForStatement*) {
		std::cout<<"";
	}
	void visit(ASTReturnStatement*) {
		std::cout<<"";
	}
	void visit(ASTVarLocation*) {
		std::cout<<"";
	}
	void visit(ASTArrayLocation*) {
		std::cout<<"";
	}
	void visit(ASTContinueStatement*) {
		std::cout<<"";
	}
	void visit(ASTBreakStatement*) {
		std::cout<<"";
	}
	void visit(ASTLiteralExpression*) {
		std::cout<<"";
	}
	void visit(ASTIntegerLiteralExpression*) {
		std::cout<<"";
	}
	void visit(ASTCharLiteralExpression*) {
		std::cout<<"";
	}
	void visit(ASTTrueLiteralExpression*) {
		std::cout<<"";
	}
	void visit(ASTFalseLiteralExpression*) {
		std::cout<<"";
	}
	void visit(ASTBinaryOperationExpression*) {
		std::cout<<"";
	}
	void visit(ASTUnaryOperationExpression*) {
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
