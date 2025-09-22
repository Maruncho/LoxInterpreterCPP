
module Parser;
import Parser;

import <vector>;
import <string>;

import Token;
import Expr;
import Stmt;
import Error;


std::string fnType_toString(FnType type) {
	switch (type) {
		case FnType::FUNCTION: return "function";
	}
	return "UNDEFINED";
}

bool Parser::match(std::initializer_list<TokenType> types) {
	for (const TokenType type : types) {
		if (check(type)) {
			advance();
			return true;
		}
	}
	return false;
}

ParseError Parser::error(Token token, std::string message) {
	Error::error(token, message);
	return ParseError();
}

Token Parser::consume(TokenType type, std::string message) {
	if (check(type)) return advance();

	throw error(peek(), message);
}

Expr* Parser::expression() {
	return assignment();
}

Expr* Parser::assignment() {
	Expr* expr = lOr();

	if (match({ TokenType::EQUAL })) {
		Token equals = previous();
		Expr* value = assignment();

		if (auto var = dynamic_cast<Variable*>(expr)) {
			Token name = var->nam;
			delete var; //subtle, but it doesn't get stored in the parsed tree.
			return new Assign(name, value);
		}

		error(equals, "Invalid assignment target.");
	}

	return expr;
}

Expr* Parser::lOr() {
	Expr* expr = lAnd();

	while (match({ TokenType::OR })) {
		Token oper = previous();
		Expr* right = lAnd();
		expr = new Logical(expr, oper, right);
	}

	return expr;
}

Expr* Parser::lAnd() {
	Expr* expr = equality();

	while (match({ TokenType::AND })) {
		Token oper = previous();
		Expr* right = equality();
		expr = new Logical(expr, oper, right);
	}

	return expr;
}

Expr* Parser::equality() {
	Expr* expr = comparison();

	while (match({ TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL })) {
		Token oper = previous();
		Expr* right = comparison();
		expr = new Binary(expr, oper, right);
	}
	return expr;
}

Expr* Parser::comparison() {
	Expr* expr = term();

	while (match({ TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL })) {
		Token oper = previous();
		Expr* right = term();
		expr = new Binary(expr, oper, right);
	}
	return expr;
}

Expr* Parser::term() {
	Expr* expr = factor();

	while (match({ TokenType::MINUS, TokenType::PLUS })) {
		Token oper = previous();
		Expr* right = factor();
		expr = new Binary(expr, oper, right);
	}
	return expr;
}

Expr* Parser::factor() {
	Expr* expr = unary();

	while (match({ TokenType::SLASH, TokenType::STAR })) {
		Token oper = previous();
		Expr* right = unary();
		expr = new Binary(expr, oper, right);
	}
	return expr;
}

Expr* Parser::unary() {
	if (match({ TokenType::BANG, TokenType::MINUS })) {
		Token oper = previous();
		Expr* right = unary();
		return new Unary(oper, right);
	}

	return call();
}

Expr* Parser::call() {
	Expr* expr = primary();

	while (true) {
		if (match({ TokenType::LEFT_PAREN })) {
			expr = finishCall(expr);
		}
		else {
			break;
		}
	}

	return expr;
}

Expr* Parser::finishCall(Expr* callee) {
	std::vector<Expr*> arguments;
	if (!check(TokenType::RIGHT_PAREN)) {
		do {
			if (arguments.size() >= 255) {
				Error::error(peek(), "Can't have more than 255 arguments.");
			}
			arguments.push_back(expression());
		} while (match({ TokenType::COMMA }));
	}

	Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

	return new Call(callee, paren, arguments);
}

Expr* Parser::primary() {
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

std::vector<Stmt*> Parser::block() {
	std::vector<Stmt*> statements;

	while (!check({ TokenType::RIGHT_BRACE }) && !isAtEnd()) {
		statements.push_back(declaration());
	}

	consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
	return statements;
}

Stmt* Parser::declaration() {
	try {
		if (match({ TokenType::FUN })) return function(FnType::FUNCTION);
		if (match({ TokenType::VAR })) return varDeclaration();

		return statement();
	}
	catch (ParseError) {
		synchronize();
		return nullptr;
	}
}

Stmt* Parser::function(FnType fnType) {
	std::string kind = fnType_toString(fnType);
	Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
	consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");

	std::vector<Token> parameters;
	if (!check(TokenType::RIGHT_PAREN)) {
		do {
			if (parameters.size() >= 255) {
				error(peek(), "Can't have more than 255 parameters.");
			}

			parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
		} while (match({ TokenType::COMMA }));
	}
	consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

	consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
	std::vector<Stmt*> body = block();
	return new Function(name, parameters, body);
}

Stmt* Parser::varDeclaration() {
	Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

	Expr* initializer = nullptr;
	if (match({ TokenType::EQUAL })) {
		initializer = expression();
	}

	consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
	return new Var(name, initializer);
}

Stmt* Parser::statement() {
	if (match({ TokenType::FOR })) return forStatement();
	if (match({ TokenType::IF })) return ifStatement();
	if (match({ TokenType::PRINT })) return printStatement();
	if (match({ TokenType::RETURN })) return returnStatement();
	if (match({ TokenType::WHILE })) return whileStatement();
	if (match({ TokenType::LEFT_BRACE })) return new Block(block());

	return expressionStatement();
}

Stmt* Parser::forStatement() {
	consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

	Stmt* initializer;
	if (match({ TokenType::SEMICOLON })) {
		initializer = nullptr;
	}
	else if (match({ TokenType::VAR })) {
		initializer = varDeclaration();
	}
	else {
		initializer = expressionStatement();
	}

	Expr* condition = nullptr;
	if (!check(TokenType::SEMICOLON)) {
		condition = expression();
	}
	consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

	Expr* increment = nullptr;
	if (!check(TokenType::RIGHT_PAREN)) {
		increment = expression();
	}
	consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");
	Stmt* body = statement();

	if (increment) {
		body = new Block({ body, new Expression(increment) });
	}

	if (!condition) {
		condition = new Literal(true);
	}
	body = new While(condition, body);

	if (initializer) {
		body = new Block({ initializer, body });
	}

	return body;
}

Stmt* Parser::ifStatement() {
	consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
	Expr* condition = expression();
	consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

	Stmt* thenBranch = statement();
	Stmt* elseBranch = nullptr;
	if (match({ TokenType::ELSE })) {
		elseBranch = statement();
	}

	return new If(condition, thenBranch, elseBranch);
}

Stmt* Parser::printStatement() {
	Expr* value = expression();
	consume(TokenType::SEMICOLON, "Expect ';' after value.");
	return new Print(value);
}

Stmt* Parser::returnStatement() {
	Token kwrd = previous();
	Expr* value = nullptr;
	if (!check(TokenType::SEMICOLON)) {
		value = expression();
	}

	consume(TokenType::SEMICOLON, "Expect ';' after return value.");
	return new Return(kwrd, value);
}

Stmt* Parser::whileStatement() {
	consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
	Expr* condition = expression();
	consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
	Stmt* body = statement();

	return new While(condition, body);
}

Stmt* Parser::expressionStatement() {
	Expr* expr = expression();
	consume(TokenType::SEMICOLON, "Expect ';' after expression.");
	return new Expression(expr);
}

void Parser::synchronize() {
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

Parser::Parser(std::vector<Token>& tokens) : tokens{ tokens }, current{ 0 } {}

std::vector<Stmt*> Parser::parse() {
	std::vector<Stmt*> statements;
	while (!isAtEnd()) {
		statements.push_back(declaration());
	}
	return statements;
}