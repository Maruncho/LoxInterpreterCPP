
export module Object;

import <variant>;
import <string>;
import <vector>;
import <iostream>;
import <functional>;
import <memory>;


export class Interpreter;
export struct Token;

class Object;

export typedef const std::vector<Object>& CallParams;

export class LoxCallable {
protected:
	LoxCallable() {}
public:
	virtual ~LoxCallable() {};
	LoxCallable(LoxCallable&) = delete;
	LoxCallable& operator=(LoxCallable&) = delete;

	virtual int arity() const = 0;
	virtual Object call(Interpreter&, CallParams) = 0;
	virtual std::string toString() const = 0;

};

class LoxClass;

export class LoxInstance {
public:
	std::unordered_map<std::string, Object> fields;
	LoxClass* klass;

	LoxInstance(LoxClass* klass);

	//LoxInstance(const LoxInstance& other);
	//LoxInstance& operator=(const LoxInstance& other);
	inline bool operator==(const LoxInstance& other) const { return this == &other; }

	Object get(Token name);
	void set(Token name, Object value);

	//~LoxInstance();
};

export class Object {
	std::variant<double, bool, std::string, std::monostate, LoxCallable*, LoxInstance*> val;

public:
	Object(double value);
	Object(bool value);
	Object(std::string value);
	Object();
	Object(LoxCallable* fn);
	Object(LoxInstance* value);
	//Object(std::variant<double, bool, std::string, std::monostate, LoxCallable*, LoxInstance*> val);

	//Object(const Object&);
	//Object& operator=(const Object&);

	//Object(Object&& other) = default;
	//Object& operator=(Object&& other) = default;

	inline const double getDouble() const { return std::get<double>(val); }
	inline const bool getBool() const { return std::get<bool>(val); }
	inline const std::string getString() const { return std::get<std::string>(val); }
	inline LoxCallable* getCallablePtr() const { return std::get<LoxCallable*>(val); }
	inline LoxInstance* getLoxInstancePtr() const { return std::get<LoxInstance*>(val); }

	inline const Object call(Interpreter& interpreter, CallParams args) const {
		return std::get<LoxCallable*>(val)->call(interpreter, args); 
	}

	inline int  callableArity() const {
		return std::get<LoxCallable*>(val)->arity();
	}
	
	inline bool isDouble() const { return std::holds_alternative<double>(val); }
	inline bool isBool() const { return std::holds_alternative<bool>(val); }
	inline bool isString() const { return std::holds_alternative<std::string>(val); }
	inline bool isNil() const { return std::holds_alternative<std::monostate>(val); }
	inline bool isCallable() const { return std::holds_alternative<LoxCallable*>(val); }
	bool isClass() const;
	inline bool isLoxInstance() const { return std::holds_alternative<LoxInstance*>(val); }

	inline bool isPointer() const { return std::holds_alternative<LoxCallable*>(val) || std::holds_alternative<LoxInstance*>(val); }
	inline void* getPointer() const { return std::holds_alternative<LoxCallable*>(val) ? (void*)std::get<LoxCallable*>(val) : (void*)std::get<LoxInstance*>(val); }

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
	Interpreter& interpreter;
public:
	Function* function;
	Environment* closure;
	bool isClassInit;

	LoxFn(Function* fn, Environment* env, Interpreter& intr, bool isInit);

	Object bind(LoxInstance* instance);

	int arity() const override;
	Object call(Interpreter& interpreter, CallParams) override;
	std::string toString() const override;
};

export class LoxClass : public LoxCallable {
public:
	std::unordered_map<std::string, LoxFn*> methods;
	const std::string name;
	LoxClass* superclass;
	LoxClass(std::string name, LoxClass* superclass, std::unordered_map<std::string, LoxFn*> methods);

	LoxFn* findMethod(std::string name);

	int arity() const override;
	Object call(Interpreter& interpreter, CallParams) override;
	std::string toString() const override;
};

export std::ostream& operator<<(std::ostream& os, const Object& obj);
