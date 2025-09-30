
export module Parser;

import <vector>;
import <string>;

import Token;
export import Expr;
export import Stmt;
import Error;

export struct ParseResult {
	std::vector<Stmt*> stmts;
	ParseResult(std::vector<Stmt*> stmts);
	~ParseResult();
};

export class ParseError {};

export enum class FnType {
	FUNCTION,
	METHOD
};

export class Parser {
	const std::vector<Token>& tokens;
	int current;

	inline Token peek() {
		return tokens[current];
	}

	inline Token previous() {
		return tokens[current - 1];
	}

	inline bool isAtEnd() {
		return peek().type == TokenType::Eof;
	}

	inline bool check(TokenType type) {
		if (isAtEnd()) return false;
		return peek().type == type;
	}

	inline Token advance() {
		if (!isAtEnd()) current++;
		return previous();
	}

	bool match(std::initializer_list<TokenType> types);

	ParseError error(Token token, std::string message);

	Token consume(TokenType type, std::string message);

	Expr* expression();

	Expr* assignment();

	Expr* lOr();

	Expr* lAnd();

	Expr* equality();

	Expr* comparison();

	Expr* term();

	Expr* factor();

	Expr* unary();

	Expr* call();

	Expr* finishCall(Expr* callee);

	Expr* primary();

/////////////// STATEMENTS ////////////////////

	std::vector<Stmt*> block();

	Stmt* declaration();

	Stmt* classDeclaration();

	Function* function(FnType);

	Stmt* varDeclaration();

	Stmt* statement();

	Stmt* forStatement();

	Stmt* ifStatement();

	Stmt* printStatement();

	Stmt* returnStatement();

	Stmt* whileStatement();

	Stmt* expressionStatement();

	void synchronize();

public:
	Parser(std::vector<Token>& tokens);

	ParseResult parse();
};
