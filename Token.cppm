
export module Token;

import <string>;
import <iostream>;

export import Object;

export enum class TokenType
{
	// Single-character tokens.
	LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
	COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

	// One or two character tokens.
	BANG, BANG_EQUAL,
	EQUAL, EQUAL_EQUAL,
	GREATER, GREATER_EQUAL,
	LESS, LESS_EQUAL,

	// Literals.
	IDENTIFIER, STRING, NUMBER,

	// Keywords.
	AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
	PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

	Eof
};

export struct Token {
	TokenType type;
	std::string lexeme;
	Object literal;
	int line;

	Token(TokenType typ, std::string lex, double lit, int ln);
	Token(TokenType typ, std::string lex, bool lit, int ln);
	Token(TokenType typ, std::string lex, std::string lit, int ln);
	Token(TokenType typ, std::string lex, int ln);
};

export std::ostream& operator<<(std::ostream& os, const Token& t);
