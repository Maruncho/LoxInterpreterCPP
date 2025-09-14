

#include <iostream>
#include <fstream>
#include <sstream>

import Scanner;
import Token;
import Error;

void run(std::string source) {
	Scanner scanner = Scanner(source);
	auto tokens = scanner.scanTokens();

	for (Token tok : tokens) {
		std::cout << tok << '\n';
	}
}

int runFile(std::string path) {
	std::ifstream input{path};
	std::stringstream buffer;
	buffer << input.rdbuf();
	run(buffer.str());

	if (hadError) return 65;
	return 0;
}

void runPrompt() {
	while (true) {
		std::cout << "> ";

		std::string line;
		getline(std::cin, line);

		if (line.empty()) break;
		run(line);
		hadError = false;
	}
}

int main(int argc, char* argv[]) {
	if (argc > 2) {
		std::cout << "Usage: cpplox [script]\n";
		return 64;
	}
	else if (argc == 2) {
		return runFile(argv[1]);
	}
	else {
		runPrompt();
	}
}
