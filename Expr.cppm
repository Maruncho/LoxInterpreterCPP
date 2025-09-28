
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
	Expr();
public:
	virtual ~Expr();
	Expr(Expr&) = delete;
	Expr& operator=(Expr&) = delete;

	virtual Object accept(ExprVisitor<Object>* visitor) const = 0;
	virtual void accept(ExprVisitor<void>* visitor) const = 0;
};

export struct Binary : public Expr {
	const Expr* l;
	const Token op;
	const Expr* r;

	Binary(Expr* left, Token oper, Expr* right);
	~Binary();
	Object accept(ExprVisitor<Object>* visitor) const override;
	void accept(ExprVisitor<void>* visitor) const override;
};

export struct Assign : public Expr {
	const Token id;
	const Expr* val;

	Assign(Token name, Expr* value);
	~Assign();
	Object accept(ExprVisitor<Object>* visitor) const override;
	void accept(ExprVisitor<void>* visitor) const override;
};

export struct Call : public Expr {
	const Expr* calleeExpr;
	const Token parenthesis;
	const std::vector<Expr*> args;

	Call(Expr* callee, Token paren, std::vector<Expr*> arguments);
	~Call();
	Object accept(ExprVisitor<Object>* visitor) const override;
	void accept(ExprVisitor<void>* visitor) const override;
};

export struct Get : public Expr {
	const Expr* obj;
	const Token id;

	Get(Expr* object, Token name);
	~Get();
	Object accept(ExprVisitor<Object>* visitor) const override;
	void accept(ExprVisitor<void>* visitor) const override;
};

export struct Grouping : public Expr {
	const Expr* expr;

	Grouping(Expr* expression);
	~Grouping();
	Object accept(ExprVisitor<Object>* visitor) const override;
	void accept(ExprVisitor<void>* visitor) const override;
};

export struct Literal : public Expr {
	const Object val;

	Literal();
	Literal(Object value);
	Object accept(ExprVisitor<Object>* visitor) const override;
	void accept(ExprVisitor<void>* visitor) const override;
};

export struct Logical : public Expr {
	const Expr* l;
	const Token op;
	const Expr* r;

	Logical(Expr* left, Token oper, Expr* right);
	~Logical();
	Object accept(ExprVisitor<Object>* visitor) const override;
	void accept(ExprVisitor<void>* visitor) const override;
};

export struct Set : public Expr {
	const Expr* obj;
	const Token name;
	const Expr* val;

	Set(const Expr* object, Token name, Expr* value);
	~Set();
	Object accept(ExprVisitor<Object>* visitor) const override;
	void accept(ExprVisitor<void>* visitor) const override;
};

export struct Super : public Expr {
	const Token keywrd;
	const Token meth;

	Super(Token keyword, Token method);
	Object accept(ExprVisitor<Object>* visitor) const override;
	void accept(ExprVisitor<void>* visitor) const override;
};

export struct This : public Expr {
	const Token keywrd;

	This(Token keyword);
	Object accept(ExprVisitor<Object>* visitor) const override;
	void accept(ExprVisitor<void>* visitor) const override;
};

export struct Unary : public Expr {
	const Token op;
	const Expr* r;

	Unary(Token oper, Expr* right);
	~Unary();
	Object accept(ExprVisitor<Object>* visitor) const override;
	void accept(ExprVisitor<void>* visitor) const override;
};

export struct Variable : public Expr {
	Token nam;

	Variable(Token name);
	inline Variable copy() { return Variable(nam); }
	Object accept(ExprVisitor<Object>* visitor) const override;
	void accept(ExprVisitor<void>* visitor) const override;
};

