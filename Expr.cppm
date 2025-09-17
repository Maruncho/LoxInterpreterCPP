
export module Expr;

import <vector>;
import <string>;

export import Token;

class Expr;
struct Assign;
struct Binary;
struct Call;
struct Get;
struct Grouping;
struct Literal;
struct Logical;
struct Set;
struct Super;
struct This;
struct Unary;
struct Variable;


export template<class R> class ExprVisitor {
public:
	virtual R visitAssignExpr(const Assign* expr) = 0;
	virtual R visitBinaryExpr(const Binary* expr) = 0;
	virtual R visitCallExpr(const Call* expr) = 0;
	virtual R visitGetExpr(const Get* expr) = 0;
	virtual R visitGroupingExpr(const Grouping* expr) = 0;
	virtual R visitLiteralExpr(const Literal* expr) = 0;
	virtual R visitLogicalExpr(const Logical* expr) = 0;
	virtual R visitSetExpr(const Set* expr) = 0;
    virtual R visitSuperExpr(const Super* expr) = 0;
    virtual R visitThisExpr(const This* expr) = 0;
    virtual R visitUnaryExpr(const Unary* expr) = 0;
    virtual R visitVariableExpr(const Variable* expr) = 0;
};


export class Expr {

protected:
	Expr() {}
public:
	virtual ~Expr() {}
	Expr(Expr&) = delete;
	Expr(Expr&&) = delete;

	virtual Object accept(ExprVisitor<Object>* visitor) const = 0;
};

export struct Binary : public Expr {
	const Expr* l;
	const Token op;
	const Expr* r;

	Binary(Expr* left, Token oper, Expr* right)
		: l{left}, op{oper}, r{right} { }

	~Binary() {
		delete l;
		delete r;
	}

	Object accept(ExprVisitor<Object>* visitor) const override { return visitor->visitBinaryExpr(this); }
};

export struct Assign : public Expr {
	const Token id;
	const Expr* val;

	Assign(Token name, Expr* value)
		: id{name}, val{value} { }

	~Assign() {
		delete val;
	}

	Object accept(ExprVisitor<Object>* visitor) const override { return visitor->visitAssignExpr(this); }
};

export struct Call : public Expr {
	const Expr* var;
	const Token parenthesis;
	const std::vector<Expr*> args;

	Call(Expr* callee, Token paren, std::vector<Expr*> arguments)
		: var{callee}, parenthesis{paren}, args{arguments} { }

	~Call() {
		//delete var; var's primary handler will delete it
		for (auto x : args) {
			delete x;
		}
	}

	Object accept(ExprVisitor<Object>* visitor) const override { return visitor->visitCallExpr(this); }
};

export struct Get : public Expr {
	const Expr* obj;
	const Token id;

	Get(Expr* object, Token name)
		: obj{object}, id{name} { }

	~Get() {
		//delete obj; obj's primary handle will delete it
	}

	Object accept(ExprVisitor<Object>* visitor) const override { return visitor->visitGetExpr(this); }
};

export struct Grouping : public Expr {
	const Expr* expr;

	Grouping(Expr* expression)
		: expr{expression} { }

	~Grouping() {
		delete expr;
	}

	Object accept(ExprVisitor<Object>* visitor) const override { return visitor->visitGroupingExpr(this); }
};

export struct Literal : public Expr {
	const Object val;

	//Literal(double num) : val{ num } {}
	//Literal(bool b) : val{ b } {}
	//Literal(std::string text) : val{ text } {}
	Literal() : val{ Object() } {}
	Literal(Object value) : val{value} {}

	Object accept(ExprVisitor<Object>* visitor) const override { return visitor->visitLiteralExpr(this); }
};

export struct Logical : public Expr {
	const Expr* l;
	const Token op;
	const Expr* r;

	Logical(Expr* left, Token oper, Expr* right)
		: l{left}, op{oper}, r{right} { }

	~Logical() {
		delete l;
		delete r;
	}

	Object accept(ExprVisitor<Object>* visitor) const override { return visitor->visitLogicalExpr(this); }
};

export struct Set : public Expr {
	const Expr* obj;
	const Token op;
	const Expr* val;

	Set(Expr* object, Token oper, Expr* value)
		: obj{object}, op{oper}, val{value} { }

	~Set() {
		//delete obj; obj's primary handler will delete it
		delete val;
	}

	Object accept(ExprVisitor<Object>* visitor) const override { return visitor->visitSetExpr(this); }
};

export struct Super : public Expr {
	const Token keywrd;
	const Token meth;

	Super(Token keyword, Token method) : keywrd{keyword}, meth{method} {}
};

export struct This : public Expr {
	const Token keywrd;

	This(Token keyword) : keywrd{keyword} {}

	Object accept(ExprVisitor<Object>* visitor) const override { return visitor->visitThisExpr(this); }
};

export struct Unary : public Expr {
	const Token op;
	const Expr* r;

	Unary(Token oper, Expr* right) : op{ oper }, r{ right } {}

	~Unary() {
		delete r;
	}

	Object accept(ExprVisitor<Object>* visitor) const override { return visitor->visitUnaryExpr(this); }
};

export struct Variable : public Expr {
	const Token nam;

	Variable(Token name) : nam{name} {}

	Variable copy() { return Variable(nam); }

	Object accept(ExprVisitor<Object>* visitor) const override { return visitor->visitVariableExpr(this); }
};

