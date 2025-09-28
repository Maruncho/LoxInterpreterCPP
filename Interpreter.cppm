
export module Interpreter;

import <string>;
import <vector>;

import Expr;
import Stmt;
import Error;
import Environment;
import GC;

export struct ReturnFromLoxFn {
	Object value;

	ReturnFromLoxFn(Object val);
};

export class Interpreter : public ExprVisitor<Object>, public StmtVisitor<void> {
public:
	Environment globals;
	Environment* environment;
	GC& gc;
private:

	std::unordered_map<const Expr*, int> locals;

	inline Object evaluate(const Expr* expr) {
		return expr->accept(this);
	}

	inline void execute(const Stmt* stmt) {
		stmt->accept(this);
	}

	bool isTruthy(Object obj);

	inline bool isEqual(Object a, Object b) {
		return a.equals(b);
	}

	inline void checkNumberOperand(Token oper, Object operand) {
		if (operand.isDouble()) return;
		throw Error::RuntimeError(oper, "Operand must be a number.");
	}

	inline void checkNumberOperands(Token oper, Object left, Object right) {
		if (left.isDouble() && right.isDouble()) return;
		throw Error::RuntimeError(oper, "Operands must be numbers.");
	}

	Object lookUpVariable(Token name, const Expr* expr);


public:
	Interpreter(GC&);
	~Interpreter();

	//Function* registerFnRef(Function* stmt);
	//void unregisterFnRef(Function* stmt);

	void interpret(std::vector<Stmt*> statements);

	void executeBlock(const std::vector<Stmt*> statements, Environment* environment);

	inline void resolve(const Expr* expr, int depth) {
		locals[expr] = depth;
	}

	Object visitLiteralExpr(const Literal* expr) override;
	Object visitLogicalExpr(const Logical* expr) override;
	Object visitGroupingExpr(const Grouping* expr) override;
	Object visitUnaryExpr(const Unary* expr) override;
	Object visitBinaryExpr(const Binary* expr) override;
	Object visitCallExpr(const Call* expr) override;
	Object visitVariableExpr(const Variable* expr) override;
	Object visitAssignExpr(const Assign* expr) override;
	Object visitGetExpr(const Get* expr) override;
	Object visitSetExpr(const Set* expr) override;
	Object visitSuperExpr(const Super* expr) override;
	Object visitThisExpr(const This* expr) override;

/////////////////STATEMENTS///////////////////

	void visitExpressionStmt(const Expression* stmt) override;
	void visitPrintStmt(const Print* stmt) override;
	void visitVarStmt(const Var* stmt) override;
	void visitBlockStmt(const Block* stmt) override;
	void visitIfStmt(const If* stmt) override;
	void visitWhileStmt(const While* stmt) override;
	void visitClassStmt(const Class* stmt) override;
	void visitFunctionStmt(Function* stmt) override;
	void visitReturnStmt(const Return* stmt) override;
};
