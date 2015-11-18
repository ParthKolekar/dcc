#ifndef _PrintVisitor_H
#define _PrintVisitor_H

#include <iostream>
#include <vector>
#include "AST.h"
#include "Visitor.h"

class PrintVisitor : public Visitor
{
public:
    void * visit(ASTProgram * node) {
        if (node->getFdl()) {
            for(auto it = (node->getFdl())->begin() ; it != (node->getFdl())->end(); it++) {
                (*it)->accept(this);
            }               
        }
        if (node->getMdl()) {
            for(auto it = (node->getMdl())->begin() ; it != (node->getMdl())->end(); it++) {
                (*it)->accept(this);
            }               
        }
        return NULL;
     }

    void * visit(ASTFieldDecl * node) {
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
        return NULL;
    }

    void * visit(ASTVarIdentifier * node) {
        std::cout<<" "<<node->getId();
        return NULL;
    }

    void * visit(ASTArrayIdentifier * node) {
        std::cout<<" "<<node->getId()<<"["<<node->getSize()<<"]";
        return NULL;
    }

    void * visit(ASTMethodDecl * node) {
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
        return NULL;
    }

    void * visit(ASTTypeIdentifier * node) {
        std::cout<<node->parseDatatype(node->getType())<<" "<<node->getId()<<" ";
        return NULL;
    }

    void * visit(ASTBlockStatement * node) {
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
        return NULL;
    }

    void * visit(ASTAssignmentStatement * node) {
        node->getLocation()->accept(this);
        std::cout<<" "<<node->parseAssignOp(node->getOp())<<" ";
        node->getExpr()->accept(this);
        std::cout<<std::endl;
        return NULL;
    }

    void * visit(ASTMethodCall * node) {
        std::cout<<""<<std::endl;
        return NULL;
    }

    void * visit(ASTNormalMethod * node) {
        std::cout<<node->getId()<<"(";
        if(node->getArguments()){
            for(auto it = (node->getArguments())->begin() ; it != (node->getArguments())->end(); it++) {
              (*it)->accept(this);
            }
        }
        std::cout<<")";
return NULL;
    }

    void * visit(ASTCalloutMethod * node) {
        std::cout<<node->getMethod_name()<<"(";
        if(node->getArguments()){
            for(auto it = (node->getArguments())->begin() ; it != (node->getArguments())->end(); it++) {
              (*it)->accept(this);
            }
        }
        std::cout<<")"<<std::endl;
return NULL;
    }

    void * visit(ASTCalloutArg * node) {
        
        return NULL;
    }

    void * visit(ASTStringCalloutArg * node) {
        std::cout<<node->getArgument();
        return NULL;
    }

    void * visit(ASTExpressionCalloutArg * node) {
        node->getArgument()->accept(this);
        return NULL;
    }

    void * visit(ASTIfStatement * node) {
        std::cout<<"if(";
        node->getCondition()->accept(this);
        std::cout<<")"<<std::endl;
        node->getIf_block()->accept(this);
        if(node->getElse_block())
            node->getElse_block()->accept(this);
        return NULL;
    }

    void * visit(ASTForStatement * node) {
        node->getInit_condition()->accept(this);
        node->getEnd_condition()->accept(this);
        node->getBlock()->accept(this);
        return NULL;
    }

    void * visit(ASTReturnStatement * node) {
        std::cout<<"return ";
        node->getReturn_expr()->accept(this);
        return NULL;
    }

    void * visit(ASTContinueStatement * node) {
        std::cout<<"continue"<<std::endl;
        return NULL;
    }

    void * visit(ASTBreakStatement * node) {
        std::cout<<"break"<<std::endl;
        return NULL;
    }

    void * visit(ASTLocation * node) {
        std::cout<<""<<std::endl;
        return NULL;
    }

    void * visit(ASTVarLocation * node) {
        std::cout<<node->getId()<<" ";
        return NULL;
    }

    void * visit(ASTArrayLocation * node) {
        std::cout<<node->getId()<<" ["<<node->getIndex()<<"]";
        return NULL;
    }

    void * visit(ASTLiteralExpression * node) {
        std::cout<<""<<std::endl;
        return NULL;
    }

    void * visit(ASTIntegerLiteralExpression * node) {
        std::cout<<" "<<node->getValue();
        return NULL;
    }

    void * visit(ASTCharLiteralExpression * node) {
        std::cout<<" "<<node->getValue();
        return NULL;
    }

    void * visit(ASTTrueLiteralExpression * node) {
        std::cout<<" "<<node->getValue();
        return NULL;
    }

    void * visit(ASTFalseLiteralExpression * node) {
        std::cout<<" "<<node->getValue();
        return NULL;
    }

    void * visit(ASTBinaryOperationExpression * node) {
        node->getLeft()->accept(this);
        std::cout<<" "<<node->parseBinOp(node->getOp())<<" ";
        node->getRight()->accept(this);
        return NULL;
    }

    void * visit(ASTUnaryOperationExpression * node) {
        std::cout<<node->parseUnOp(node->getOp())<<" "<<std::endl;
        node->getExpr()->accept(this);
        return NULL;
    }

    void * visit(ASTVarDecl * node) {
        std::cout<<node->parseDatatype(node->getType())<<" ";
        for(auto it = (node->getId_list())->begin() ; it != (node->getId_list())->end(); it++) {
              (*it)->accept(this);
            }
        std::cout<<std::endl;
        return NULL;
    }

    void * visit(ASTIdentifier * node) {
        std::cout<<""<<std::endl;
        return NULL;
    }

    void * visit(ASTStatement * node) {
        std::cout<<""<<std::endl;
        return NULL;
    }

    void * visit(ASTExpression * node) {
        std::cout<<""<<std::endl;
        return NULL;
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
