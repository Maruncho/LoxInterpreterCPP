
module Resolver;
import Resolver;

import Error;

Resolver::Resolver(Interpreter& interpreter, GC& gc) : interpreter{ interpreter }, gc { gc } {}

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
	resolve(expr->obj);
}

void Resolver::visitSetExpr(const Set* expr) {
	resolve(expr->val);
	resolve(expr->obj);
}

void Resolver::visitSuperExpr(const Super* expr) {
	if (currentClass == ClassType::NONE) {
		Error::error(expr->keywrd, "Can't use 'super' outside of a class.");
	}
	else if (currentClass != ClassType::SUBCLASS) {
		Error::error(expr->keywrd, "Can't use 'super' in a class with no superclass.");
	}

	resolveLocal(expr, expr->keywrd);
}

void Resolver::visitThisExpr(const This* expr) {
	if (currentClass == ClassType::NONE) {
		Error::error(expr->keywrd, "Can't use 'this' outside of a class.");
		return;
	}

	resolveLocal(expr, expr->keywrd);
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
	ClassType enclosingClass = currentClass;
	currentClass = ClassType::CLASS;

	declare(stmt->nam);
	define(stmt->nam);

	if (stmt->super && stmt->nam.lexeme == stmt->super->nam.lexeme) {
		Error::error(stmt->super->nam, "A class can't inherit from itself.");
	}

	if (stmt->super) {
		currentClass = ClassType::SUBCLASS;
		resolve(stmt->super);
	}

	if (stmt->super) {
		beginScope();
		scopes.back()["super"] = true;
	}

	beginScope();
	scopes.back()["this"] = true;

	for (Function* method : stmt->meths) {
		FunctionType declaration = FunctionType::METHOD;
		if (method->id.lexeme == "init") {
			declaration = FunctionType::INITIALIZER;
		}
		resolveFunction(method, declaration);
	}

	endScope();

	if (stmt->super) endScope();

	currentClass = enclosingClass;
}

void Resolver::visitFunctionStmt(Function* stmt) {
	declare(stmt->id);
	define(stmt->id);

	resolveFunction(stmt, FunctionType::FUNCTION);
}

void Resolver::resolveFunction(Function* function, FunctionType type) {
	beginScope();

	gc.track(function);
	if (lastFunction) lastFunction->fns_in_body.push_back(function);

	FunctionType enclosingFunction = currentFunction;
	currentFunction = type;

	for (Token param : function->params) {
		declare(param);
		define(param);
	}

	Function* prevLastFunction = lastFunction;
	lastFunction = function;

	resolve(function->body);

	lastFunction = prevLastFunction;

	endScope();
}

void Resolver::visitReturnStmt(const Return* stmt) {
	if (currentFunction == FunctionType::NONE) {
		Error::error(stmt->keywrd, "Can't return from top-level code.");
	}
	if (stmt->val) {
		if (currentFunction == FunctionType::INITIALIZER) {
			Error::error(stmt->keywrd, "Can't return a value from an initializer.");
		}
		resolve(stmt->val);
	}
}