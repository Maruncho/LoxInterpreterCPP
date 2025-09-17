
export module Interpreter;

import <string>;
import <vector>;

import Expr;
import Stmt;
import Error;
import Environment;

export class Interpreter : public ExprVisitor<Object>, public StmtVisitor<void> {
	Environment environment;

	inline Object evaluate(const Expr* expr) {
		return expr->accept(this);
	}

	inline void execute(const Stmt* stmt) {
		stmt->accept(this);
	}

	inline void executeBlock(const std::vector<Stmt*> statements, Environment environment) {
		Environment previous = this->environment;
		try {
			this->environment = environment;

			for (const Stmt* statement : statements) {
				execute(statement);
			}

			this->environment = previous;
		}
		catch (...) {
			this->environment = previous;
		}
	}

	bool isTruthy(Object obj) {
		if (obj.isNil()) return false;
		if (obj.isBool()) return obj.getBool();
		return true;
	}

	inline bool isEqual(Object a, Object b) {
		return a.equals(b);
	}

	void checkNumberOperand(Token oper, Object operand) {
		if (operand.isDouble()) return;
		throw Error::RuntimeError(oper, "Operand must be a number.");
	}

	void checkNumberOperands(Token oper, Object left, Object right) {
		if (left.isDouble() && right.isDouble()) return;
		throw Error::RuntimeError(oper, "Operands must be numbers.");
	}

public:
	Interpreter() {}

	void interpret(std::vector<Stmt*> statements) {
		try {
			for (const Stmt* statement : statements) {
				execute(statement);
			}
		}
		catch (Error::RuntimeError error) {
			Error::runtimeError(error);
		}
	}

	Object visitLiteralExpr(const Literal* expr) override {
		return expr->val;
	}
	
	Object visitGroupingExpr(const Grouping* expr) override {
		return evaluate(expr->expr);
	}

	Object visitUnaryExpr(const Unary* expr) override {
		Object right = evaluate(expr->r);

		switch (expr->op.type) {
			case TokenType::BANG:
				return !isTruthy(right);
			case TokenType::MINUS:
				checkNumberOperand(expr->op, right);
				return -(right.getDouble());
		}

		// Unreachable.
		return Object();
	}

	Object visitBinaryExpr(const Binary* expr) override {
		Object left = evaluate(expr->l);
		Object right = evaluate(expr->r);

		switch (expr->op.type) {
			case TokenType::GREATER:
				checkNumberOperands(expr->op, left, right);
				return left.getDouble() > right.getDouble();
			case TokenType::GREATER_EQUAL:
				checkNumberOperands(expr->op, left, right);
				return left.getDouble() >= right.getDouble();
			case TokenType::LESS:
				checkNumberOperands(expr->op, left, right);
				return left.getDouble() < right.getDouble();
			case TokenType::LESS_EQUAL:
				checkNumberOperands(expr->op, left, right);
				return left.getDouble() <= right.getDouble();
			case TokenType::MINUS:
				checkNumberOperands(expr->op, left, right);
				return left.getDouble() - right.getDouble();

			case TokenType::PLUS:
				if (left.isDouble() && right.isDouble()) {
					return left.getDouble() + right.getDouble();
				}

				if (left.isString() && right.isString()) {
					return left.getString() + right.getString();
				}

				throw new Error::RuntimeError(expr->op, "Operands must be two numbers or two strings.");

			case TokenType::SLASH:
				checkNumberOperands(expr->op, left, right);
				return left.getDouble() / right.getDouble();
			case TokenType::STAR:
				checkNumberOperands(expr->op, left, right);
				return left.getDouble() * right.getDouble();

			case TokenType::BANG_EQUAL: return !isEqual(left, right);
			case TokenType::EQUAL_EQUAL: return isEqual(left, right);
		}

		// Unreachable.
		return Object();
	}

	Object visitVariableExpr(const Variable* expr) {
		return environment.get(expr->nam);
	}

	Object visitAssignExpr(const Assign* expr) {
		Object value = evaluate(expr->val);
		environment.assign(expr->id, value);
		return value;
	}

	Object visitCallExpr(const Call* expr) { return Object(); }
	Object visitGetExpr(const Get* expr) { return Object(); }
	Object visitLogicalExpr(const Logical* expr) { return Object(); }
	Object visitSetExpr(const Set* expr) { return Object(); }
    Object visitSuperExpr(const Super* expr) { return Object(); }
    Object visitThisExpr(const This* expr) { return Object(); }

/////////////////STATEMENTS///////////////////

	void visitExpressionStmt(const Expression* stmt) override {
		evaluate(stmt->expr);
	}

	void visitPrintStmt(const Print* stmt) override {
		Object value = evaluate(stmt->expr);
		std::cout << value << '\n';
	}

	void visitVarStmt(const Var* stmt) override {
		Object value;
		if (stmt->init != nullptr) {
			value = evaluate(stmt->init);
		}

		environment.define(stmt->id.lexeme, value);
	}

	void visitBlockStmt(const Block* stmt) {
		executeBlock(stmt->stmts, Environment{ environment });
	}

    void visitClassStmt(const Class* stmt) {}
    void visitFunctionStmt(const Function* stmt) {}
    void visitIfStmt(const If* stmt) {}
    void visitReturnStmt(const Return* stmt) {}
    void visitWhileStmt(const While* stmt) {}
};