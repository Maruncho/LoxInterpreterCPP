
export module Scanner;

import <string>;
import <vector>;

import Token;

export class Scanner {
	const std::string source;
	std::vector<Token> tokens;

	int start;
	int current;
	int line;

	inline bool isAtEnd() { return current >= source.size(); }

	inline char advance() { return source[current++]; }

	inline char peek() { if (isAtEnd()) return '\0'; return source[current]; }

	inline char peekNext() { if (current + 1 >= source.size()) return '\0'; return source[current + 1]; }


	void addToken(TokenType type);

	void addToken(TokenType type, std::string lit);

	void addToken(TokenType type, double lit);

	bool match(char expected);

	void string();
	void number();
	void identifier();

	void scanToken();

public:
	Scanner(std::string src);

	std::vector<Token> scanTokens();
};
