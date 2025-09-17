
export module Parser;

import <vector>;
import <string>;

import Token;
export import Expr;
export import Stmt;
import Error;

export class ParseError {};

export class Parser {
	const std::vector<Token>& tokens;
	int current = 0;

	inline Token peek() {
		return tokens[current];
	}

	inline Token previous() {
		return tokens[current - 1];
	}

	inline bool isAtEnd() {
		return peek().type == TokenType::Eof;
	}

	bool check(TokenType type) {
		if (isAtEnd()) return false;
		return peek().type == type;
	}

	Token advance() {
		if (!isAtEnd()) current++;
		return previous();
	}

	bool match(std::initializer_list<TokenType> types) {
		for (const TokenType type : types) {
			if (check(type)) {
				advance();
				return true;
			}
		}
		return false;
	}

	ParseError error(Token token, std::string message) {
		Error::error(token, message);
		return ParseError();
	}

	Token consume(TokenType type, std::string message) {
		if (check(type)) return advance();

		throw error(peek(), message);
	}

	Expr* expression() {
		return assignment();
	}

	Expr* assignment() {
		Expr* expr = equality();

		if (match({ TokenType::EQUAL })) {
			Token equals = previous();
			Expr* value = assignment();

			if (auto var = dynamic_cast<Variable*>(expr)) {
				Token name = var->nam;
				return new Assign(name, value);
			}

			error(equals, "Invalid assignment target.");
		}

		return expr;
	}

	Expr* equality() {
		Expr* expr = comparison();

		while (match({ TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL })) {
			Token oper = previous();
			Expr* right = comparison();
			expr = new Binary(expr, oper, right);
		}
		return expr;
	}

	Expr* comparison() {
		Expr* expr = term();

		while (match({ TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL })) {
			Token oper = previous();
			Expr* right = term();
			expr = new Binary(expr, oper, right);
		}
		return expr;
	}

	Expr* term() {
		Expr* expr = factor();

		while (match({ TokenType::MINUS, TokenType::PLUS })) {
			Token oper = previous();
			Expr* right = factor();
			expr = new Binary(expr, oper, right);
		}
		return expr;
	}

	Expr* factor() {
		Expr* expr = unary();

		while (match({ TokenType::SLASH, TokenType::STAR })) {
			Token oper = previous();
			Expr* right = unary();
			expr = new Binary(expr, oper, right);
		}
		return expr;
	}

	Expr* unary() {
		if (match({ TokenType::BANG, TokenType::MINUS })) {
			Token oper = previous();
			Expr* right = unary();
			return new Unary(oper, right);
		}

		return primary();
	}

	Expr* primary() {
		if (match({ TokenType::FALSE })) return new Literal(false);
		if (match({ TokenType::TRUE })) return new Literal(true);
		if (match({ TokenType::NIL })) return new Literal();

		if (match({ TokenType::NUMBER, TokenType::STRING })) {
			return new Literal(previous().literal);
		}

		if (match({ TokenType::IDENTIFIER })) {
			return new Variable(previous());
		}

		if (match({ TokenType::LEFT_PAREN })) {
			Expr* expr = expression();
			consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
			return new Grouping(expr);
		}

		throw error(peek(), "Expect expression.");
	}

/////////////// STATEMENTS ////////////////////

	std::vector<Stmt*> block() {
		std::vector<Stmt*> statements;

		while (!check({ TokenType::RIGHT_BRACE }) && !isAtEnd()) {
			statements.emplace_back(declaration());
		}

		consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
		return statements;
	}

	Stmt* declaration() {
		try {
			if (match({ TokenType::VAR })) return varDeclaration();

			return statement();
		}
		catch (ParseError) {
			synchronize();
			return nullptr;
		}
	}

	Stmt* varDeclaration() {
		Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

		Expr* initializer = nullptr;
		if (match({ TokenType::EQUAL })) {
			initializer = expression();
		}

		consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
		return new Var(name, initializer);
	}

	Stmt* statement() {
		if (match({ TokenType::PRINT })) return printStatement();
		if (match({ TokenType::LEFT_BRACE })) return new Block(block());

		return expressionStatement();
	}

	Stmt* printStatement() {
		Expr* value = expression();
		consume(TokenType::SEMICOLON, "Expect ';' after value.");
		return new Print(value);
	}

	Stmt* expressionStatement() {
		Expr* expr = expression();
		consume(TokenType::SEMICOLON, "Expect ';' after expression.");
		return new Expression(expr);
	}

	void synchronize() {
		advance();

		while (!isAtEnd()) {
			if (previous().type == TokenType::SEMICOLON) return;

			switch (peek().type) {
				case TokenType::CLASS:
				case TokenType::FUN:
				case TokenType::VAR:
				case TokenType::FOR:
				case TokenType::IF:
				case TokenType::WHILE:
				case TokenType::PRINT:
				case TokenType::RETURN:
					return;
			}

			advance();
		}
	}

public:
	Parser(std::vector<Token>& tokens) : tokens{tokens} { }

	std::vector<Stmt*> parse() {
		std::vector<Stmt*> statements;
		while (!isAtEnd()) {
			statements.emplace_back(declaration());
		}
		return statements;
	}
};