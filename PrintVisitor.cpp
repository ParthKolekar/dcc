#ifndef _PrintVisitor_CPP
#define _PrintVisitor_CPP

#include <iostream>
#include <vector>
#include "AST.h"
#include "Visitor.h"

class PrintVisitor : public Visitor
{
public:
    void visit(ASTProgram * node) {
        std::cout<<"<program>"<<std::endl;
        if(node->getFdl() == NULL){
            std::cout<<"<field_declarations count = \"0\">"<<std::endl;
        }

        else{
            std::cout<<"<field_declarations count =\" "<<(node->getFdl())->size()<<">"<<std::endl;
            for(auto it = (node->getFdl())->begin() ; it != (node->getFdl())->end(); it++) {
                (*it)->accept(this);
            }               
        }
        std::cout<<"</method_declarations>"<<std::endl;
        if(node->getMdl() == NULL){
            std::cout<<"<method_declarations count = \"0\">"<<std::endl;
        }

        else{
            std::cout<<"<method_declarations count =\" "<<(node->getMdl())->size()<<">"<<std::endl;
            for(auto it = (node->getMdl())->begin() ; it != (node->getMdl())->end(); it++) {
                (*it)->accept(this);
            }               
        }
        std::cout<<"</method_declarations>"<<std::endl;
    }
    void visit(ASTFieldDecl * node) {
        std::cout<<node->parseDatatype(node->getType())<<" ";
        for(auto it = (node->getVar_id_list())->begin() ; it != (node->getVar_id_list())->end(); it++) {
            (*it)->accept(this);
        }
        for(auto it = (node->getArray_id_list())->begin() ; it != (node->getArray_id_list())->end(); it++) {
            (*it)->accept(this);
        }
    }

    void visit(ASTVarIdentifier * node) {
        std::cout<<" "<<node->getId();
    }

    void visit(ASTArrayIdentifier * node) {
        std::cout<<" "<<node->getId()<<"["<<node->getSize()<<"]";
    }

    void visit(ASTMethodDecl * node) {
        std::cout<<node->getId()<<" "<<node->parseDatatype(node->getReturnType())<<" ";
        for(auto it = (node->getArguments())->begin() ; it != (node->getArguments())->end(); it++) {
            (*it)->accept(this);
        }
            /* Block statement is called*/
        node->getBlock()->accept(this);
    }
    void visit(ASTTypeIdentifier * node) {
        std::cout<<node->getId()<<" "<<node->parseDatatype(node->getType())<<std::endl;
    }

    void visit(ASTBlockStatement * node) {
        std::cout<<"{"<<std::endl;
        for(auto it = (node->getStmtlist())->begin() ; it != (node->getStmtlist())->end(); it++) {
            (*it)->accept(this);
        }
        std::cout<<"}"<<std::endl;
    }

    void visit(ASTAssignmentStatement * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTMethodCall * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTNormalMethod * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTCalloutMethod * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTCalloutArg * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTStringCalloutArg * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTExpressionCalloutArg * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTIfStatement * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTForStatement * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTReturnStatement * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTContinueStatement * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTBreakStatement * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTLocation * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTVarLocation * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTArrayLocation * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTLiteralExpression * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTIntegerLiteralExpression * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTCharLiteralExpression * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTTrueLiteralExpression * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTFalseLiteralExpression * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTBinaryOperationExpression * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTUnaryOperationExpression * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTVarDecl * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTIdentifier * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTStatement * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTExpression * node) {
        std::cout<<""<<std::endl;
    }
};
#ifdef TEST

int main()
{
    ASTProgram * obj = new ASTProgram("yolo",NULL,NULL);
    obj->accept(new PrintVisitor());
    return 0;
}

#endif
#endif
