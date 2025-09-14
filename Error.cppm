
export module Error;

import <string>;
import <iostream>;

export bool hadError = false;

export void report(int line, std::string where, std::string message) {
	std::cerr << "[line " << line << "] Error" << where << ": " << message << '\n';
	hadError = true;
}

export void error(int line, std::string message) {
	report(line, "", message);
}
