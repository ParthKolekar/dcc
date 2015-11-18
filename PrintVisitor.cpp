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
        // std::cout<<"<program>"<<std::endl;
        if(node->getFdl() == NULL){
            // std::cout<<"<field_declarations count = \"0\">"<<std::endl;
        }

        else{
            // std::cout<<"<field_declarations count =\" "<<(node->getFdl())->size()<<"\">"<<std::endl;
            for(auto it = (node->getFdl())->begin() ; it != (node->getFdl())->end(); it++) {
                (*it)->accept(this);
            }               
        }
        // std::cout<<"</field_declarations>"<<std::endl;
        if(node->getMdl() == NULL){
            // std::cout<<"<method_declarations count = \"0\">"<<std::endl;
        }

        else{
            // std::cout<<"<method_declarations count =\" "<<(node->getMdl())->size()<<"\">"<<std::endl;
            for(auto it = (node->getMdl())->begin() ; it != (node->getMdl())->end(); it++) {
                (*it)->accept(this);
            }               
        }
        // std::cout<<"</method_declarations>"<<std::endl;
        // std::cout<<"</program>"<<std::endl;
    }
    void visit(ASTFieldDecl * node) {
        std::cout<<node->parseDatatype(node->getType())<<" ";
        if (node->getVar_id_list()) {
            for(auto it = (node->getVar_id_list())->begin() ; it != (node->getVar_id_list())->end(); it++) {
                (*it)->accept(this);
            }   
        }
        if (node->getArray_id_list()) {
            for(auto it = (node->getArray_id_list())->begin() ; it != (node->getArray_id_list())->end(); it++) {
                (*it)->accept(this);
            }
        }
    }

    void visit(ASTVarIdentifier * node) {
        std::cout<<" "<<node->getId();
    }

    void visit(ASTArrayIdentifier * node) {
        std::cout<<" "<<node->getId()<<"["<<node->getSize()<<"]";
    }

    void visit(ASTMethodDecl * node) {
        std::cout<<node->parseDatatype(node->getReturnType())<<" "<<node->getId()<<" ";
        std::cout<<"(";
        if (node->getArguments()) {
            for(auto it = (node->getArguments())->begin() ; it != (node->getArguments())->end(); it++) {
                (*it)->accept(this);
            }
        }
        std::cout<<")"<<std::endl;
        /* Block statement is called*/
        node->getBlock()->accept(this);
    }
    void visit(ASTTypeIdentifier * node) {
        std::cout<<node->parseDatatype(node->getType())<<" "<<node->getId()<<" ";
    }

    void visit(ASTBlockStatement * node) {
        std::cout<<"{"<<std::endl;
        // std::cout<<"no of statements = ";
        if(node->getStmtlist() && node->getId_list()){
            std::cout<<node->getStmtlist()->size() + node->getId_list()->size()<<std::endl;
        for(auto it = (node->getId_list())->begin() ; it != (node->getId_list())->end(); it++) {
            (*it)->accept(this);
        }
        for(auto it = (node->getStmtlist())->begin() ; it != (node->getStmtlist())->end(); it++) {
            (*it)->accept(this);
        }
        }
        else if(node->getStmtlist()){
            std::cout<<node->getStmtlist()->size()<<std::endl;
            for(auto it = (node->getStmtlist())->begin() ; it != (node->getStmtlist())->end(); it++) {
            (*it)->accept(this);
        }
        }
        else if(node->getId_list()){
            std::cout<<node->getId_list()->size()<<std::endl;
            for(auto it = (node->getId_list())->begin() ; it != (node->getId_list())->end(); it++) {
            (*it)->accept(this);
        }
        }
        
        std::cout<<std::endl;
        
        std::cout<<"}"<<std::endl;
    }

    void visit(ASTAssignmentStatement * node) {
        node->getLocation()->accept(this);
        std::cout<<" "<<node->parseAssignOp(node->getOp())<<" ";
        node->getExpr()->accept(this);
        std::cout<<std::endl;
    }
    void visit(ASTMethodCall * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTNormalMethod * node) {
        std::cout<<node->getId()<<"(";
        if(node->getArguments()){
            for(auto it = (node->getArguments())->begin() ; it != (node->getArguments())->end(); it++) {
              (*it)->accept(this);
            }
        }
        std::cout<<")";
    }
    void visit(ASTCalloutMethod * node) {
        std::cout<<node->getMethod_name()<<"(";
        if(node->getArguments()){
            for(auto it = (node->getArguments())->begin() ; it != (node->getArguments())->end(); it++) {
              (*it)->accept(this);
            }
        }
        std::cout<<")"<<std::endl;
    }
    void visit(ASTCalloutArg * node) {
        
    }
    void visit(ASTStringCalloutArg * node) {
        std::cout<<node->getArgument();
    }
    void visit(ASTExpressionCalloutArg * node) {
        node->getArgument()->accept(this);
    }
    void visit(ASTIfStatement * node) {
        std::cout<<"if(";
        node->getCondition()->accept(this);
        std::cout<<")"<<std::endl;
        node->getIf_block()->accept(this);
        if(node->getElse_block())
            node->getElse_block()->accept(this);
    }
    void visit(ASTForStatement * node) {
        node->getInit_condition()->accept(this);
        node->getEnd_condition()->accept(this);
        node->getBlock()->accept(this);

    }
    void visit(ASTReturnStatement * node) {
        std::cout<<"return ";
        node->getReturn_expr()->accept(this);
    }
    void visit(ASTContinueStatement * node) {
        std::cout<<"continue"<<std::endl;
    }
    void visit(ASTBreakStatement * node) {
        std::cout<<"break"<<std::endl;
    }
    void visit(ASTLocation * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTVarLocation * node) {
        std::cout<<node->getId()<<" ";
    }
    void visit(ASTArrayLocation * node) {
        std::cout<<node->getId()<<" ["<<node->getIndex()<<"]";
    }
    void visit(ASTLiteralExpression * node) {
        std::cout<<""<<std::endl;
    }
    void visit(ASTIntegerLiteralExpression * node) {
        std::cout<<" "<<node->getValue();
    }
    void visit(ASTCharLiteralExpression * node) {
        std::cout<<" "<<node->getValue();
    }
    void visit(ASTTrueLiteralExpression * node) {
        std::cout<<" "<<node->getValue();
    }
    void visit(ASTFalseLiteralExpression * node) {
        std::cout<<" "<<node->getValue();
    }
    void visit(ASTBinaryOperationExpression * node) {
        node->getLeft()->accept(this);
        std::cout<<" "<<node->parseBinOp(node->getOp())<<" ";
        node->getRight()->accept(this);
    }
    void visit(ASTUnaryOperationExpression * node) {
        std::cout<<node->parseUnOp(node->getOp())<<" "<<std::endl;
        node->getExpr()->accept(this);
    }
    void visit(ASTVarDecl * node) {
        std::cout<<node->parseDatatype(node->getType())<<" ";
        for(auto it = (node->getId_list())->begin() ; it != (node->getId_list())->end(); it++) {
              (*it)->accept(this);
            }
        std::cout<<std::endl;
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
