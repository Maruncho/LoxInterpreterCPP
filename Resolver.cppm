
export module Resolver;

import <vector>;
import <unordered_map>;
import <string>;

import Expr;
import Stmt;
import Interpreter;
import GC;

enum class FunctionType {
	NONE,
	FUNCTION,
	INITIALIZER,
	METHOD
};

enum class ClassType {
	NONE,
	CLASS
};

export class Resolver : public ExprVisitor<void>, public StmtVisitor<void> {
	Interpreter& interpreter;
	GC& gc;
	std::vector<std::unordered_map<std::string, bool>> scopes;
	
	FunctionType currentFunction = FunctionType::NONE;
	ClassType currentClass = ClassType::NONE;

	Function* lastFunction = nullptr;

	void resolveLocal(const Expr* expr, Token name) const;
	void resolveFunction(Function* function, FunctionType type);

	inline void resolve(const Stmt* stmt) {
		stmt->accept(this);
	}

	inline void resolve(const Expr* expr) {
		expr->accept(this);
	}

	inline void beginScope() {
		scopes.push_back(std::unordered_map<std::string, bool>());
	}

	inline void endScope() {
		scopes.pop_back();
	}

	void declare(Token name);

	inline void define(Token name) {
		if (scopes.empty()) return;
		scopes.back()[name.lexeme] = true;
	}

public:
	Resolver(Interpreter& interpreter, GC& gc);

	std::vector<std::pair<Function*, Function*>> functions;

	inline void resolve(const std::vector<Stmt*>& stmts) {
		for (Stmt* stmt : stmts) {
			resolve(stmt);
		}
	}

	void visitLiteralExpr(const Literal* expr) override;
	void visitLogicalExpr(const Logical* expr) override;
	void visitGroupingExpr(const Grouping* expr) override;
	void visitUnaryExpr(const Unary* expr) override;
	void visitBinaryExpr(const Binary* expr) override;
	void visitCallExpr(const Call* expr) override;
	void visitVariableExpr(const Variable* expr) override;
	void visitAssignExpr(const Assign* expr) override;
	void visitGetExpr(const Get* expr) override;
	void visitSetExpr(const Set* expr) override;
	void visitSuperExpr(const Super* expr) override;
	void visitThisExpr(const This* expr) override;

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
