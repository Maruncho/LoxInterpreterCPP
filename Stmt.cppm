
export module Stmt;

import <vector>;
import <string>;

import Token;
import Expr;

class Stmt;
struct Block;
struct Class;
struct Expression;
struct Function;
struct If;
struct Print;
struct Return;
struct Var;
struct While;

export template<class R> class StmtVisitor {
public:
	virtual R visitBlockStmt(const Block* stmt) = 0;
    virtual R visitClassStmt(const Class* stmt) = 0;
    virtual R visitExpressionStmt(const Expression* stmt) = 0;
    virtual R visitFunctionStmt(const Function* stmt) = 0;
    virtual R visitIfStmt(const If* stmt) = 0;
    virtual R visitPrintStmt(const Print* stmt) = 0;
    virtual R visitReturnStmt(const Return* stmt) = 0;
    virtual R visitVarStmt(const Var* stmt) = 0;
    virtual R visitWhileStmt(const While* stmt) = 0;
};


export class Stmt {
protected:
	Stmt() {}
public:
	virtual ~Stmt() {}
	Stmt(Stmt&) = delete;
	Stmt(Stmt&&) = delete;

	virtual void accept(StmtVisitor<void>* visitor) const = 0;
};

export struct Block : public Stmt {
	const std::vector<Stmt*> stmts;

	Block(std::vector<Stmt*> statements) : stmts{statements} {}

	~Block() {
		for (Stmt* x : stmts) {
			delete x;
		}
	}

	void accept(StmtVisitor<void>* visitor) const override { return visitor->visitBlockStmt(this); }
};

export struct Function : public Stmt {
	const Token id;
	const std::vector<Token> params;
	const std::vector<Stmt*> body;

	Function(Token name, std::vector<Token> parameters, std::vector<Stmt*> fnBody)
		: id{name}, params{parameters}, body{fnBody} { }

	~Function() {
		for (Stmt* x : body) {
			delete x;
		}
	}

	void accept(StmtVisitor<void>* visitor) const override { return visitor->visitFunctionStmt(this); }
};

export struct Class : public Stmt {
	const Token nam;
	const Variable super;
	const std::vector<Function> meths;

	Class(Token name, Variable superclass, std::vector<Function> methods)
		: nam{name}, super{superclass.copy()}, meths{std::move(methods)} { }
};

export struct Expression : public Stmt {
	const Expr* expr;

	Expression(Expr* expression) : expr{expression} { }

	~Expression() {
		delete expr;
	}

	void accept(StmtVisitor<void>* visitor) const override { return visitor->visitExpressionStmt(this); }
};

export struct If : public Stmt {
	const Expr* cond;
	const Stmt* th;
	const Stmt* el;

	If(Expr* condition, Stmt* thenBranch, Stmt* elseBranch)
		: cond{condition}, th{thenBranch}, el{elseBranch} { }

	~If() {
		delete cond;
		delete th;
		delete el;
	}

	void accept(StmtVisitor<void>* visitor) const override { return visitor->visitIfStmt(this); }
};

export struct Print : public Stmt {
	const Expr* expr;

	Print(Expr* expression) : expr{expression} { }

	~Print() {
		delete expr;
	}

	void accept(StmtVisitor<void>* visitor) const override { return visitor->visitPrintStmt(this); }
};

export struct Return : public Stmt {
	const Token keywrd;
	const Expr* val;

	Return(Token keyword, Expr* value) : keywrd{keyword}, val{value} { }

	~Return() {
		delete val;
	}

	void accept(StmtVisitor<void>* visitor) const override { return visitor->visitReturnStmt(this); }
};

export struct Var : public Stmt {
	const Token id;
	const Expr* init;

	Var(Token name, Expr* initializer) : id{name}, init{initializer} { }

	~Var() {
		if(init) delete init;
	}

	void accept(StmtVisitor<void>* visitor) const override { return visitor->visitVarStmt(this); }
};

export struct While : public Stmt {
	const Expr* cond;
	const Stmt* body;

	While(Expr* condition, Stmt* whileBody)
		: cond{condition}, body{whileBody} { }

	~While() {
		delete cond;
		delete body;
	}

	void accept(StmtVisitor<void>* visitor) const override { visitor->visitWhileStmt(this); }
};

