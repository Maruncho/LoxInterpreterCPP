

import <iostream>;
import <fstream>;
import <sstream>;
import <vector>;

import Scanner;
import Parser;
import Interpreter;
import Token;
import Error;

void run(std::string source, Interpreter& interpreter) {
	Scanner scanner = Scanner(source);
	auto tokens = scanner.scanTokens();

	Parser parser = Parser(tokens);
	std::vector<Stmt*> statements = parser.parse();

	// Stop if there was a syntax error.
	if (Error::hadError) return;

	interpreter.interpret(statements);
}

int runFile(std::string path, Interpreter& interpreter) {
	std::ifstream input{path};
	std::stringstream buffer;
	buffer << input.rdbuf();
	run(buffer.str(), interpreter);

	if (Error::hadError) return 65;
	if (Error::hadRuntimeError) return 70;
	return 0;
}

void runPrompt(Interpreter& interpreter) {
	while (true) {
		std::cout << "> ";

		std::string line;
		getline(std::cin, line);

		if (line.empty()) break;
		run(line, interpreter);
		Error::hadError = false;
	}
}

int main(int argc, char* argv[]) {
	Interpreter interpreter = Interpreter();
	if (argc > 2) {
		std::cout << "Usage: cpplox [script]\n";
		return 64;
	}
	else if (argc == 2) {
		return runFile(argv[1], interpreter);
	}
	else {
		runPrompt(interpreter);
	}
}
