
export module Object;

import <variant>;
import <string>;
import <vector>;
import <iostream>;
import <functional>;

export class Interpreter;

class Object;

export typedef const std::vector<Object>& CallParams;

export class LoxCallable {
protected:
	LoxCallable();
public:
	int refs;
	bool hasBeenReturned = false;
	virtual ~LoxCallable();
	LoxCallable(LoxCallable&) = delete;
	LoxCallable& operator=(LoxCallable&) = delete;

	virtual int arity() const = 0;
	virtual Object call(Interpreter&, CallParams) = 0;
	virtual std::string toString() const = 0;
};

export class Object {
	std::variant<double, bool, std::string, std::monostate, LoxCallable*> val;

public:
	Object(double value);
	Object(bool value);
	Object(std::string value);
	Object();
	Object(LoxCallable* fn);

	Object(const Object&);
	Object(Object&& other);
	Object& operator=(Object&);
	Object& operator=(Object&& other);
	~Object();

	inline const double getDouble() const { return std::get<double>(val); }
	inline const bool getBool() const { return std::get<bool>(val); }
	inline const std::string getString() const { return std::get<std::string>(val); }
	inline LoxCallable* getCallablePtr() const { return std::get<LoxCallable*>(val); }

	inline const Object call(Interpreter& interpreter, CallParams args) const {
		return std::get<LoxCallable*>(val)->call(interpreter, args);
	}
	inline int  callableArity() const { return std::get<LoxCallable*>(val)->arity(); }
	
	inline bool isDouble() const { return std::holds_alternative<double>(val); }
	inline bool isBool() const { return std::holds_alternative<bool>(val); }
	inline bool isString() const { return std::holds_alternative<std::string>(val); }
	inline bool isNil() const { return std::holds_alternative<std::monostate>(val); }
	inline bool isCallable() const { return std::holds_alternative<LoxCallable*>(val); }

	inline bool equals(const Object other) const {
		return val == other.val;
	}
};

export struct Function;
export class Stmt;
export class NativeFn : public LoxCallable {
	const std::function<Object(CallParams)> fn;
	const int arit;

public:
	NativeFn(std::function<Object(CallParams)> functionPtr, int functionArity);

	int arity() const override;
	Object call(Interpreter& interpreter, CallParams) override;
	std::string toString() const override;
};

export class Environment;

export class LoxFn : public LoxCallable {
	const Function* function;
	Environment* closure;

public:
	LoxFn(const Function* fn, Environment* env);
	~LoxFn();

	int arity() const override;
	Object call(Interpreter& interpreter, CallParams) override;
	std::string toString() const override;
};

export std::ostream& operator<<(std::ostream& os, const Object& obj);
