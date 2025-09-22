
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
    virtual R visitFunctionStmt(Function* stmt) = 0;
    virtual R visitIfStmt(const If* stmt) = 0;
    virtual R visitPrintStmt(const Print* stmt) = 0;
    virtual R visitReturnStmt(const Return* stmt) = 0;
    virtual R visitVarStmt(const Var* stmt) = 0;
    virtual R visitWhileStmt(const While* stmt) = 0;
};


export class Stmt {
protected:
	Stmt();
public:
	virtual ~Stmt();
	Stmt(Stmt&) = delete;
	Stmt& operator=(Stmt&) = delete;

	virtual void accept(StmtVisitor<void>* visitor) const = 0;
};

export struct Block : public Stmt {
	const std::vector<Stmt*> stmts;

	Block(std::vector<Stmt*> statements);
	~Block();
	void accept(StmtVisitor<void>* visitor) const override;
};

export struct Function : public Stmt {
	const Token id;
	const std::vector<Token> params;
	std::vector<Stmt*> body;

	Function(Token name, std::vector<Token> parameters, std::vector<Stmt*> fnBody);
	~Function();
	Function* release();
	void accept(StmtVisitor<void>* visitor) const override;
};

export struct Class : public Stmt {
	const Token nam;
	const Variable& super;
	const std::vector<Function> meths;

	Class(Token name, Variable& superclass, std::vector<Function> methods);
	void accept(StmtVisitor<void>* visitor) const override;
};

export struct Expression : public Stmt {
	const Expr* expr;

	Expression(Expr* expression);
	~Expression();
	void accept(StmtVisitor<void>* visitor) const override;
};

export struct If : public Stmt {
	const Expr* cond;
	const Stmt* th;
	const Stmt* el;

	If(Expr* condition, Stmt* thenBranch, Stmt* elseBranch);
	~If();
	void accept(StmtVisitor<void>* visitor) const override;
};

export struct Print : public Stmt {
	const Expr* expr;

	Print(Expr* expression);
	~Print();
	void accept(StmtVisitor<void>* visitor) const override;
};

export struct Return : public Stmt {
	const Token keywrd;
	const Expr* val;

	Return(Token keyword, Expr* value);
	~Return();
	void accept(StmtVisitor<void>* visitor) const override;
};

export struct Var : public Stmt {
	const Token id;
	const Expr* init;

	Var(Token name, Expr* initializer);
	~Var();
	void accept(StmtVisitor<void>* visitor) const override;
};

export struct While : public Stmt {
	const Expr* cond;
	const Stmt* body;

	While(Expr* condition, Stmt* whileBody);
	~While();
	void accept(StmtVisitor<void>* visitor) const override;
};

