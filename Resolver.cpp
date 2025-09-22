
module Resolver;
import Resolver;

import Error;

Resolver::Resolver(Interpreter& interpreter) : interpreter{ interpreter } {}

void Resolver::resolveLocal(const Expr* expr, Token name) const {
	for (int i = (int)scopes.size() - 1; i >= 0; i--) {
		if (scopes[i].contains(name.lexeme)) {
			interpreter.resolve(expr, (int)(scopes.size()) - 1 - i);
			return;
		}
	}
}

void Resolver::declare(Token name) {
	if (scopes.empty()) return;
	auto scope = scopes.back();
	if (scope.contains(name.lexeme)) {
		Error::error(name, "Already a variable with this name in this scope.");
	}
	scope[name.lexeme] = false;
}

void Resolver::visitLiteralExpr(const Literal* expr) {
	return;
}

void Resolver::visitLogicalExpr(const Logical* expr) {
	resolve(expr->l);
	resolve(expr->r);
}

void Resolver::visitGroupingExpr(const Grouping* expr) {
	resolve(expr->expr);
}

void Resolver::visitUnaryExpr(const Unary* expr) {
	resolve(expr->r);
}

void Resolver::visitBinaryExpr(const Binary* expr) {
	resolve(expr->l);
	resolve(expr->r);
}

void Resolver::visitCallExpr(const Call* expr) {
	resolve(expr->calleeExpr);

	for (auto arg : expr->args) {
		resolve(arg);
	}
}

void Resolver::visitVariableExpr(const Variable* expr) {
	if (!scopes.empty()) {
		if (auto x = scopes.back().find(expr->nam.lexeme); x != scopes.back().end() && x->second == false) {
			Error::error(expr->nam, "Can't read local variable in its own initializer.");
		}
	}

	resolveLocal(expr, expr->nam);
}

void Resolver::visitAssignExpr(const Assign* expr) {
	resolve(expr->val);
	resolveLocal(expr, expr->id);
}

void Resolver::visitGetExpr(const Get* expr) {

}

void Resolver::visitSetExpr(const Set* expr) {

}

void Resolver::visitSuperExpr(const Super* expr) {

}

void Resolver::visitThisExpr(const This* expr) {

}


void Resolver::visitExpressionStmt(const Expression* stmt) {
	resolve(stmt->expr);
}

void Resolver::visitPrintStmt(const Print* stmt) {
	resolve(stmt->expr);
}

void Resolver::visitVarStmt(const Var* stmt) {
	declare(stmt->id);
	if (stmt->init) {
		resolve(stmt->init);
	}
	define(stmt->id);
}

void Resolver::visitBlockStmt(const Block* stmt) {
	beginScope();
	resolve(stmt->stmts);
	endScope();
}

void Resolver::visitIfStmt(const If* stmt) {
	resolve(stmt->cond);
	resolve(stmt->th);
	if (stmt->el) resolve(stmt->el);
}

void Resolver::visitWhileStmt(const While* stmt) {
	resolve(stmt->cond);
	resolve(stmt->body);
}

void Resolver::visitClassStmt(const Class* stmt) {

}

void Resolver::visitFunctionStmt(Function* stmt) {
	declare(stmt->id);
	define(stmt->id);

	resolveFunction(stmt, FunctionType::FUNCTION);
}

void Resolver::resolveFunction(Function* function, FunctionType type) {
	beginScope();

	FunctionType enclosingFunction = currentFunction;
	currentFunction = type;

	for (Token param : function->params) {
		declare(param);
		define(param);
	}
	resolve(function->body);
	endScope();
	currentFunction = enclosingFunction;
}

void Resolver::visitReturnStmt(const Return* stmt) {
	if (currentFunction == FunctionType::NONE) {
		Error::error(stmt->keywrd, "Can't return from top-level code.");
	}
	if (stmt->val) resolve(stmt->val);
}