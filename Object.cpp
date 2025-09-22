
module Object;
import Object;

import <variant>;
import <string>;
import <vector>;
import <iostream>;
import <functional>;

import Stmt;
import Interpreter;
import Environment;

Object::Object(double value) : val{ value } { }
Object::Object(bool value) : val{ value } {}
Object::Object(std::string value) : val{ value } { }
Object::Object() : val{ std::monostate() } { }

//Must be called with newly heap-allocated LoxCallables. Hopefully I don't forget.
Object::Object(LoxCallable* fn) : val{ fn } { }

Object::Object(const Object& other) : val{ other.val } {
	if (other.isCallable()) {
		std::cout << other.getCallablePtr()->toString() << " " << other.getCallablePtr()->refs << "->" << other.getCallablePtr()->refs + 1 << "\n";
		other.getCallablePtr()->refs++;
	}
}

Object::Object(Object&& other) : val { other.val } {
	other.val = std::monostate();
}

Object& Object::operator=(Object& other) {
	if (other.isCallable()) {
		std::cout << other.getCallablePtr()->toString() << " " << other.getCallablePtr()->refs << "->" << other.getCallablePtr()->refs + 1 << "\n";
		other.getCallablePtr()->refs++;
	}
	val = other.val;
	return *this;
}

Object& Object::operator=(Object&& other) {
	val = other.val;
	other.val = std::monostate();
	return *this;
}

Object::~Object() {
	if (isCallable()) {
		LoxCallable* ptr = getCallablePtr();
		if (ptr->refs == 1) {
			std::cout << ptr->toString() << " got deleted!\n";
			delete ptr;
		}
		else {
			std::cout << ptr->toString() << " " << ptr->refs << "->" << ptr->refs - 1 << "\n";
			ptr->refs--;
		}
	}
}

LoxCallable::LoxCallable() : refs { 1 } { }
LoxCallable::~LoxCallable() {
}

NativeFn::NativeFn(std::function<Object(const std::vector<Object>&)> functionPtr, int functionArity)
	: LoxCallable(), fn{functionPtr}, arit{functionArity} { }

int NativeFn::arity() const {
	return arit;
}

Object NativeFn::call(Interpreter&, const std::vector<Object>& arguments) {
	return fn(arguments);
}

std::string NativeFn::toString() const {
	return "<native fn>";
}


LoxFn::LoxFn(const Function* fn, Environment* env)
	: LoxCallable(), function{ fn }, closure{ env } {}

LoxFn::~LoxFn() {
	if (hasBeenReturned) {
		std::string fnName = this->function->id.lexeme;
		closure->destroyWithCircleDependecy(fnName);
	}
		delete function;
}

int LoxFn::arity() const {
	return (int)(function->params.size());
}
Object LoxFn::call(Interpreter& interpreter, const std::vector<Object>& arguments) {
	Environment* local = new Environment { closure };
	int size = (int)function->params.size();
	for (int i = 0; i < size; i++) {
		local->define(function->params[i].lexeme, arguments[i]);
	}

	try {
		interpreter.executeBlock(function->body, local);
	}
	catch (ReturnFromLoxFn returnValue) {
		return returnValue.value;
	}

	return Object();
}

std::string LoxFn::toString() const {
	return "<fn " + function->id.lexeme + ">";
}

std::ostream& operator<<(std::ostream& os, const Object& obj) {
	if (obj.isDouble()) return os << obj.getDouble();
	if (obj.isBool()) return os << (obj.getBool() ? "true" : "false");
	if (obj.isString()) return os << obj.getString();
	if (obj.isNil()) return os << "nil";
	if (obj.isCallable()) return os << obj.getCallablePtr()->toString();
	else return os; //unreachable
}
