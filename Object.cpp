

module Object;
import Object;

constexpr bool debug_show_ref = true;

import <variant>;
import <string>;
import <vector>;
import <iostream>;
import <functional>;

import Stmt;
import Interpreter;
import Environment;
import Token;
import Error;
import GC;

Object::Object(double value) : val{ value } { }
Object::Object(bool value) : val{ value } {}
Object::Object(std::string value) : val{ value } { }
Object::Object() : val{ std::monostate() } { }
Object::Object(LoxCallable* fn) : val{ fn } {}
Object::Object(LoxInstance* inst) : val{ inst } { }
//Object::Object(std::variant<double, bool, std::string, std::monostate, LoxCallable*, LoxInstance*> val) : val{ val } { }

LoxInstance::LoxInstance(LoxClass* klass) : klass{ klass } { }

Object LoxInstance::get(Token name) {
	if (fields.contains(name.lexeme)) {
		return fields[name.lexeme];
	}

	LoxFn* method = klass->findMethod(name.lexeme);
	if (method) return method->bind(this);

	throw Error::RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
}

void LoxInstance::set(Token name, Object value) {
	fields[name.lexeme] = value;
}

NativeFn::NativeFn(std::function<Object(const std::vector<Object>&)> functionPtr, int functionArity)
	: fn{functionPtr}, arit{functionArity} { }

int NativeFn::arity() const {
	return arit;
}

Object NativeFn::call(Interpreter&, const std::vector<Object>& arguments) {
	return fn(arguments);
}

std::string NativeFn::toString() const {
	return "<native fn>";
}


LoxFn::LoxFn(Function* fn, Environment* env, Interpreter& intr, bool isClassInit)
	: function{ fn }, closure { env }, interpreter{ intr }, isClassInit{ isClassInit } { }

Object LoxFn::bind(LoxInstance* instance) {
	Environment* environment = interpreter.gc.track(new Environment(closure));
	environment->define("this", Object(instance));
	return Object(interpreter.gc.track(new LoxFn(function, environment, interpreter, isClassInit)));
}

int LoxFn::arity() const {
	return (int)(function->params.size());
}

Object LoxFn::call(Interpreter& interpreter, const std::vector<Object>& arguments) {
	Environment* local = interpreter.gc.track(new Environment { closure });
	int size = (int)function->params.size();
	for (int i = 0; i < size; i++) {
		local->define(function->params[i].lexeme, arguments[i]);
	}

	try {
		interpreter.executeBlock(function->body, local);
	}
	catch (ReturnFromLoxFn returnValue) {
		if (isClassInit) return closure->getAt(0, "this");

		return returnValue.value;
	}

	if (isClassInit) return closure->getAt(0, "this");
	return Object();
}

std::string LoxFn::toString() const {
	return "<fn " + function->id.lexeme + ">";
}


LoxClass::LoxClass(std::string name, std::unordered_map<std::string, LoxFn*> methods)
	: name{ name }, methods{ methods } { }


LoxFn* LoxClass::findMethod(std::string name) {
	if (methods.contains(name)) {
		return methods[name];
	}

	return nullptr;
}

int LoxClass::arity() const {
	if (auto fn = methods.find("init"); fn != methods.end()) {
		return fn->second->arity();
	}
	return 0;
}

Object LoxClass::call(Interpreter& interpreter, CallParams args) {
	LoxInstance* instance = interpreter.gc.track(new LoxInstance(this));
	LoxFn* initializer = findMethod("init");
	if (initializer) {
		initializer->bind(instance).call(interpreter, args);
	}

	return Object(instance);
}

std::string LoxClass::toString() const {
	return name;
}


std::ostream& operator<<(std::ostream& os, const Object& obj) {
	if (obj.isDouble()) return os << obj.getDouble();
	if (obj.isBool()) return os << (obj.getBool() ? "true" : "false");
	if (obj.isString()) return os << obj.getString();
	if (obj.isNil()) return os << "nil";
	if (obj.isCallable()) return os << obj.getCallablePtr()->toString();
	if (obj.isLoxInstance()) return os << obj.getLoxInstancePtr()->klass->name << " instance";
	else return os; //unreachable
}
