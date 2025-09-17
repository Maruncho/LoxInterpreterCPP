
export module Error;

import <string>;
import <iostream>;

import Token;

export namespace Error {

	struct RuntimeError {
		const std::string message;
		const Token token;

		RuntimeError(Token token, std::string message) : message {message}, token{token} { }
	};

	bool hadError = false;
	bool hadRuntimeError = false;

	void report(int line, std::string where, std::string message) {
		std::cerr << "[line " << line << "] Error" << where << ": " << message << '\n';
		hadError = true;
	}

	void error(int line, std::string message) {
		report(line, "", message);
	}

	void error(Token token, std::string message) {
		if (token.type == TokenType::Eof) {
			report(token.line, " at end", message);
		}
		else {
			report(token.line, " at '" + token.lexeme + "'", message);
		}
	}

	void runtimeError(RuntimeError error) {
		std::cerr << "\n[line " << error.token.line << "]";
		hadRuntimeError = true;
	}
}
