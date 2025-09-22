
module Scanner;
import Scanner;

import <string>;
import <vector>;
import <unordered_map>;

import Token;
import Error;

bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

bool isAlpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isAlphaNumeric(char c) {
	return isAlpha(c) || isDigit(c);
}

static const std::unordered_map<std::string, TokenType> keywords = {
	{"and", TokenType::AND},
	{"class", TokenType::CLASS},
	{"else", TokenType::ELSE},
	{"false", TokenType::FALSE},
	{"for", TokenType::FOR},
	{"fun", TokenType::FUN},
	{"if", TokenType::IF},
	{"nil", TokenType::NIL},
	{"or", TokenType::OR},
	{"print", TokenType::PRINT},
	{"return", TokenType::RETURN},
	{"super", TokenType::SUPER},
	{"this", TokenType::THIS},
	{"true", TokenType::TRUE},
	{"var", TokenType::VAR},
	{"while", TokenType::WHILE}
};

void Scanner::addToken(TokenType type) {
	std::string text = source.substr(start, current - start);
	tokens.push_back(Token(type, text, line));
}

void Scanner::addToken(TokenType type, std::string lit) {
		std::string text = source.substr(start, current - start);
		tokens.push_back(Token(type, text, lit, line));
}
void Scanner::addToken(TokenType type, double lit) {
	std::string text = source.substr(start, current - start);
	tokens.push_back(Token(type, text, lit, line));
}

bool Scanner::match(char expected) {
	if (isAtEnd()) return false;
	if (source[current] != expected) return false;

	current++;
	return true;
}

Scanner::Scanner(std::string src) : source{ src }, tokens{ {} }, start{ 0 }, current{ 0 }, line{ 1 } {}

std::vector<Token> Scanner::scanTokens() {
	while (!isAtEnd()) {
		start = current;
		scanToken();
	}

	tokens.push_back(Token(TokenType::Eof, "", line));
	return tokens;
}

void Scanner::string() {
	while (peek() != '"' && !isAtEnd()) {
		if (peek() == '\n') line++;
		advance();
	}

	if (isAtEnd()) {
		Error::error(line, "Unterminated string.");
		return;
	}

	// The closing ".
	advance();

	//Trim the surrounding quotes.
	std::string value = source.substr(start + 1, current - 1 - (start + 1));
	addToken(TokenType::STRING, value);
}

void Scanner::number() {
	while (isDigit(peek())) advance();

	// Look for a fractional part.
	if (peek() == '.' && isDigit(peekNext())) {
		// Consume the "."
		advance();

		while (isDigit(peek())) advance();
	}

	addToken(TokenType::NUMBER, std::stod(source.substr(start, current - start)));
}

void Scanner::identifier() {
	while (isAlphaNumeric(peek())) advance();

	std::string text = source.substr(start, current - start);
	if (auto found = keywords.find(text); found != keywords.end()) {
		addToken(found->second);
	}
	else {
		addToken(TokenType::IDENTIFIER);
	}

}

void Scanner::scanToken() {
	using enum TokenType;

	char c = advance();
	switch (c) {
		case '(': addToken(LEFT_PAREN); break;
		case ')': addToken(RIGHT_PAREN); break;
		case '{': addToken(LEFT_BRACE); break;
		case '}': addToken(RIGHT_BRACE); break;
		case ',': addToken(COMMA); break;
		case '.': addToken(DOT); break;
		case '-': addToken(MINUS); break;
		case '+': addToken(PLUS); break;
		case ';': addToken(SEMICOLON); break;
		case '*': addToken(STAR); break; 

		case '!':
			addToken(match('=') ? BANG_EQUAL : BANG);
			break;
		case '=':
			addToken(match('=') ? EQUAL_EQUAL : EQUAL);
			break;
		case '<':
			addToken(match('=') ? LESS_EQUAL : LESS);
			break;
		case '>':
			addToken(match('=') ? GREATER_EQUAL : GREATER);
			break;

		case '/':
			if (match('/')) {
				while (peek() != '\n' && !isAtEnd()) advance();
			}
			else {
				addToken(SLASH);
			}
			break;

		case ' ': case '\r': case '\t':
			// Ignore whitespace.
			break;
		case '\n':
			line++;
			break;

		case '"': string(); break;

		default:
			if (isDigit(c)) {
				number();
			}
			else if (isAlpha(c)) {
				identifier();
			}
			else {
				Error::error(line, "Unexpected character.");
			}
			break;
	}
}
