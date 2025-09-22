
export module Environment;

import <unordered_map>;
import <string>;
import <iostream>;

import Object;
import Token;
import Error;

export class Environment {
	Environment* enclosing;
	std::unordered_map <std::string, Object> values;

	bool isTopLevel = false;
public:
	Environment() : enclosing{ nullptr } {}
	Environment(Environment* enclosing, bool isTopLevel = false) : enclosing{ enclosing }, isTopLevel{ isTopLevel } {
		//global's enclosing is nullptr, so the environment enclosing global is the toplevel
	}
	~Environment() {
		std::cout << "Environment got deleted\n";
	}

	Environment(const Environment& other) = delete;
	Environment(Environment&&) = delete;

	void destroy() {
		if (!isTopLevel) {
			delete this;
		}
	}

	// fn gets deleted -> environment gets deleted -> fn gets deleted (already deleted) -> bad
	void destroyWithCircleDependecy(std::string fnName) {
		values[fnName] = Object();
		if (!isTopLevel) {
			delete this;
		}
	}

	inline void define(std::string name, Object value) {
		if (values.contains(name)) values.erase(name);
		values[name] = value;
	}
	Object get(Token name) {
		if (values.contains(name.lexeme)) {
			return values[name.lexeme];
		}

		if (enclosing) return enclosing->get(name);

		throw Error::RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
	}

	void assign(Token name, Object value) {

		if (auto found = values.find(name.lexeme); found != values.end()) {
			values.erase(found);
			values[name.lexeme] = value;
			return;
		}

		if (enclosing) {
			enclosing->assign(name, value);
			return;
		}

		throw Error::RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
	}
};
