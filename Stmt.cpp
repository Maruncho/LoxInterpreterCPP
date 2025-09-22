

module Stmt;
import Stmt;

import <vector>;
import <string>;

import Token;
import Expr;


Stmt::Stmt() {}
Stmt::~Stmt() {}


Block::Block(std::vector<Stmt*> statements) : stmts{statements} {}
Block::~Block() {
	for (Stmt* x : stmts) {
		delete x;
	}
}
void Block::accept(StmtVisitor<void>* visitor) const { return visitor->visitBlockStmt(this); }


Function::Function(Token name, std::vector<Token> parameters, std::vector<Stmt*> fnBody)
	: id{name}, params{parameters}, body{fnBody} { }
Function::~Function() {
	for (Stmt* x : body) {
		delete x;
	}
}
Function* Function::release() {
	std::vector<Stmt*> swapped;
	body.swap(swapped);
	return new Function(id, params, swapped);
}
void Function::accept(StmtVisitor<void>* visitor) const { return visitor->visitFunctionStmt(const_cast<Function*>(this)); }


Class::Class(Token name, Variable& superclass, std::vector<Function> methods)
	: nam{name}, super{superclass.copy()}, meths{std::move(methods)} { }
void Class::accept(StmtVisitor<void>* visitor) const { return visitor->visitClassStmt(this); }


Expression::Expression(Expr* expression) : expr{expression} { }
Expression::~Expression() {
	delete expr;
}
void Expression::accept(StmtVisitor<void>* visitor) const { return visitor->visitExpressionStmt(this); }


If::If(Expr* condition, Stmt* thenBranch, Stmt* elseBranch)
	: cond{condition}, th{thenBranch}, el{elseBranch} { }
If::~If() {
	delete cond;
	delete th;
	if(el) delete el;
}
void If::accept(StmtVisitor<void>* visitor) const { return visitor->visitIfStmt(this); }


Print::Print(Expr* expression) : expr{expression} { }
Print::~Print() {
	delete expr;
}
void Print::accept(StmtVisitor<void>* visitor) const { return visitor->visitPrintStmt(this); }


Return::Return(Token keyword, Expr* value) : keywrd{keyword}, val{value} { }
Return::~Return() {
	if(val) delete val;
}
void Return::accept(StmtVisitor<void>* visitor) const { return visitor->visitReturnStmt(this); }


Var::Var(Token name, Expr* initializer) : id{name}, init{initializer} { }
Var::~Var() {
	if(init) delete init;
}
void Var::accept(StmtVisitor<void>* visitor) const { return visitor->visitVarStmt(this); }


While::While(Expr* condition, Stmt* whileBody)
	: cond{condition}, body{whileBody} { }
While::~While() {
	delete cond;
	delete body;
}
void While::accept(StmtVisitor<void>* visitor) const { visitor->visitWhileStmt(this); }
