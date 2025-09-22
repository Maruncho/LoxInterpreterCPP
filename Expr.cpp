
module Expr;
import Expr;

import Token;

import <vector>;
import <string>;

Expr::Expr() {}
Expr::~Expr() {}


Binary::Binary(Expr* left, Token oper, Expr* right)
	: l{left}, op{oper}, r{right} { }
Binary::~Binary() {
	delete l;
	delete r;
}
Object Binary::accept(ExprVisitor<Object>* visitor) const { return visitor->visitBinaryExpr(this); }
void Binary::accept(ExprVisitor<void>* visitor) const { return visitor->visitBinaryExpr(this); }


Assign::Assign(Token name, Expr* value)
	: id{name}, val{value} { }
Assign::~Assign() {
	delete val;
}
Object Assign::accept(ExprVisitor<Object>* visitor) const { return visitor->visitAssignExpr(this); }
void Assign::accept(ExprVisitor<void>* visitor) const { return visitor->visitAssignExpr(this); }


Call::Call(Expr* callee, Token paren, std::vector<Expr*> arguments)
	: calleeExpr{callee}, parenthesis{paren}, args{arguments} { }
Call::~Call() {
	delete calleeExpr;
	for (auto x : args) {
		delete x;
	}
}
Object Call::accept(ExprVisitor<Object>* visitor) const { return visitor->visitCallExpr(this); }
void Call::accept(ExprVisitor<void>* visitor) const { return visitor->visitCallExpr(this); }


Get::Get(Expr* object, Token name)
	: obj{object}, id{name} { }
Get::~Get() {
	//delete obj; obj's primary handle will delete it
}
Object Get::accept(ExprVisitor<Object>* visitor) const { return visitor->visitGetExpr(this); }
void Get::accept(ExprVisitor<void>* visitor) const { return visitor->visitGetExpr(this); }


Grouping::Grouping(Expr* expression)
	: expr{expression} { }
Grouping::~Grouping() {
	delete expr;
}
Object Grouping::accept(ExprVisitor<Object>* visitor) const { return visitor->visitGroupingExpr(this); }
void Grouping::accept(ExprVisitor<void>* visitor) const { return visitor->visitGroupingExpr(this); }


Literal::Literal() : val{ Object() } {}
Literal::Literal(Object value) : val{value} {}
Object Literal::accept(ExprVisitor<Object>* visitor) const { return visitor->visitLiteralExpr(this); }
void Literal::accept(ExprVisitor<void>* visitor) const { return visitor->visitLiteralExpr(this); }


Logical::Logical(Expr* left, Token oper, Expr* right)
	: l{left}, op{oper}, r{right} { }
Logical::~Logical() {
	delete l;
	delete r;
}
Object Logical::accept(ExprVisitor<Object>* visitor) const { return visitor->visitLogicalExpr(this); }
void Logical::accept(ExprVisitor<void>* visitor) const { return visitor->visitLogicalExpr(this); }


Set::Set(Expr* object, Token oper, Expr* value)
	: obj{object}, op{oper}, val{value} { }
Set::~Set() {
	//delete obj; obj's primary handler will delete it
	delete val;
}
Object Set::accept(ExprVisitor<Object>* visitor) const { return visitor->visitSetExpr(this); }
void Set::accept(ExprVisitor<void>* visitor) const { return visitor->visitSetExpr(this); }


Super::Super(Token keyword, Token method) : keywrd{keyword}, meth{method} {}
Object Super::accept(ExprVisitor<Object>* visitor) const { return visitor->visitSuperExpr(this); }
void Super::accept(ExprVisitor<void>* visitor) const { return visitor->visitSuperExpr(this); }


This::This(Token keyword) : keywrd{keyword} {}
Object This::accept(ExprVisitor<Object>* visitor) const { return visitor->visitThisExpr(this); }
void This::accept(ExprVisitor<void>* visitor) const { return visitor->visitThisExpr(this); }


Unary::Unary(Token oper, Expr* right) : op{ oper }, r{ right } {}
Unary::~Unary() {
	delete r;
}
Object Unary::accept(ExprVisitor<Object>* visitor) const { return visitor->visitUnaryExpr(this); }
void Unary::accept(ExprVisitor<void>* visitor) const { return visitor->visitUnaryExpr(this); }


Variable::Variable(Token name) : nam{name} {}
Object Variable::accept(ExprVisitor<Object>* visitor) const { return visitor->visitVariableExpr(this); }
void Variable::accept(ExprVisitor<void>* visitor) const { return visitor->visitVariableExpr(this); }
