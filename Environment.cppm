
export module Environment;

import <map>;
import <string>;

import Object;
import Token;
import Error;

export class Environment {
	Environment* enclosing;
	std::map <std::string, Object> values;

public:
	Environment() : enclosing{nullptr} { }
	Environment(Environment* enclosing) : enclosing{enclosing} { }

	inline void define(std::string name, Object value) {
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
		if (values.contains(name.lexeme)) {
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
