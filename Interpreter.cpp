
module Interpreter;
import Interpreter;

import <iostream>;
import <string>;
import <vector>;
import <unordered_map>;

import Expr;
import Stmt;
import Error;
import Environment;
import NativeFunctions;

ReturnFromLoxFn::ReturnFromLoxFn(Object val) : value{ val } { }

void Interpreter::executeBlock(const std::vector<Stmt*> statements, Environment* environment) {
	auto previous = this->environment;
	try {
		this->environment = environment;

		for (const Stmt* statement : statements) {
			execute(statement);
		}

		this->environment = previous;
	}
	catch (ReturnFromLoxFn ret) {
		this->environment = previous;
		throw;
	}
}

bool Interpreter::isTruthy(Object obj) {
	if (obj.isNil()) return false;
	if (obj.isBool()) return obj.getBool();
	return true;
}

void Interpreter::checkNumberOperand(Token oper, Object operand) {
	if (operand.isDouble()) return;
	throw Error::RuntimeError(oper, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(Token oper, Object left, Object right) {
	if (left.isDouble() && right.isDouble()) return;
	throw Error::RuntimeError(oper, "Operands must be numbers.");
}

Interpreter::Interpreter(GC& gc) : environment{ gc.track(new Environment(&globals, true)) }, gc{ gc } {
	globals.define("clock", new NativeFn(NativeFunction::clock, 0));
}
Interpreter::~Interpreter() {
	for (auto& [_, x] : globals.values) {
		if (x.isCallable()) {
			delete x.getCallablePtr();
		}
	}
}

Object Interpreter::lookUpVariable(Token name, const Expr* expr) {
	auto distance = locals.find(const_cast<Expr*>(expr)); //don't do that, but I don't want to fix it.
	if (distance != locals.end()) {
		return environment->getAt(distance->second, name.lexeme);
	}
	else {
		return environment->get(name);
	}
}

void Interpreter::interpret(std::vector<Stmt*> statements) {
	try {
		for (const Stmt* statement : statements) {
			execute(statement);
			gc.runFromEnv(environment);
		}

		locals.clear();
	}
	catch (Error::RuntimeError& error) {
		Error::runtimeError(error);

		locals.clear();
	}
}

Object Interpreter::visitLiteralExpr(const Literal* expr) {
	return expr->val;
}

Object Interpreter::visitLogicalExpr(const Logical* expr) {
	Object left = evaluate(expr->l);

	if (expr->op.type == TokenType::OR) {
		if (isTruthy(left)) return left;
	}
	else {
		if (!isTruthy(left)) return left;
	}

	return evaluate(expr->r);
}
	
Object Interpreter::visitGroupingExpr(const Grouping* expr) {
	return evaluate(expr->expr);
}

Object Interpreter::visitUnaryExpr(const Unary* expr) {
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

Object Interpreter::visitBinaryExpr(const Binary* expr) {
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

			throw Error::RuntimeError(expr->op, "Operands must be two numbers or two strings.");

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

Object Interpreter::visitCallExpr(const Call* expr) {
	Object callee = evaluate(expr->calleeExpr);

	std::vector<Object> arguments;
	for (Expr* argument : expr->args) {
		arguments.push_back(evaluate(argument));
	}

	if (!callee.isCallable()) {
		throw Error::RuntimeError(expr->parenthesis, "Can only call functions and classes.");
	}

	if (arguments.size() != callee.callableArity()) {
		throw Error::RuntimeError(expr->parenthesis,
			"Expected " + std::to_string(callee.callableArity()) + " arguments but got " +
			std::to_string(arguments.size()) + ".");
	}

	return callee.call(*this, arguments);
}

Object Interpreter::visitVariableExpr(const Variable* expr) {
	return lookUpVariable(expr->nam, expr);
}

Object Interpreter::visitAssignExpr(const Assign* expr) {
	Object value = evaluate(expr->val);

	auto distance = locals.find(const_cast<Assign*>(expr));
	if (distance != locals.end()) {
		environment->assignAt(distance->second, expr->id, value);
	}
	else {
		environment->assign(expr->id, value);
	}

	return value;
}

Object Interpreter::visitGetExpr(const Get* expr) {
	Object object = evaluate(expr->obj);
	if (object.isLoxInstance()) {
		return object.getLoxInstancePtr()->get(expr->id);
	}

	throw Error::RuntimeError(expr->id, "Only instances have properties.");
}

Object Interpreter::visitSetExpr(const Set* expr) {
	Object object = evaluate(expr->obj);

	if (!(object.isLoxInstance())) {
		throw Error::RuntimeError(expr->name, "Only instances have fields.");
	}

	Object value = evaluate(expr->val);
	object.getLoxInstancePtr()->set(expr->name, value);
	return value;
}

Object Interpreter::visitSuperExpr(const Super* expr) { return Object(); }

Object Interpreter::visitThisExpr(const This* expr) {
	return lookUpVariable(expr->keywrd, expr);
}

/////////////////STATEMENTS///////////////////

void Interpreter::visitExpressionStmt(const Expression* stmt) {
	evaluate(stmt->expr);
}

void Interpreter::visitPrintStmt(const Print* stmt) {
	Object value = evaluate(stmt->expr);
	std::cout << value << '\n';
}

void Interpreter::visitVarStmt(const Var* stmt) {
	Object value;
	if (stmt->init != nullptr) {
		value = evaluate(stmt->init);
	}

	environment->define(stmt->id.lexeme, value);
}

void Interpreter::visitBlockStmt(const Block* stmt) {
	auto newEnv = gc.track(new Environment(environment));
	executeBlock(stmt->stmts, newEnv);
}

void Interpreter::visitIfStmt(const If* stmt) {
	if (isTruthy(evaluate(stmt->cond))) {
		execute(stmt->th);
	}
	else if (stmt->el) {
		execute(stmt->el);
	}
}

void Interpreter::visitWhileStmt(const While* stmt) {
	while (isTruthy(evaluate(stmt->cond))) {
		execute(stmt->body);
	}
}

void Interpreter::visitFunctionStmt(Function* stmt) {
	Object function = gc.track(new LoxFn(stmt, environment, *this, false));
	environment->define(stmt->id.lexeme, function);
}

void Interpreter::visitReturnStmt(const Return* stmt) {
	Object value = Object();
	if (stmt->val) value = evaluate(stmt->val);

	throw ReturnFromLoxFn(value);
}

void Interpreter::visitClassStmt(const Class* stmt) {
	environment->define(stmt->nam.lexeme, Object());

	std::unordered_map<std::string, LoxFn*> methods;
	for (Function* method : stmt->meths) {
		bool isInit = method->id.lexeme == "init";
		auto function = gc.track(new LoxFn(method, environment, *this, isInit));
		methods[method->id.lexeme] = function;
	}

	Object klass = gc.track(new LoxClass(stmt->nam.lexeme, methods));
	environment->assign(stmt->nam, klass);
}