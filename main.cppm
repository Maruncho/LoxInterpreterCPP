

import <iostream>;
import <fstream>;
import <sstream>;
import <vector>;

import Scanner;
import Parser;
import Resolver;
import Interpreter;
import GC;
import Token;
import Error;

void run(std::string source, Interpreter& interpreter, GC& gc) {
	Scanner scanner = Scanner(source);
	auto tokens = scanner.scanTokens();

	Parser parser = Parser(tokens);
	ParseResult parseResult = parser.parse();

	if (Error::hadError) return;

	Resolver resolver = Resolver(interpreter, gc);
	resolver.resolve(parseResult.stmts);

	// Stop if there was a syntax error.
	if (!Error::hadError) {
		interpreter.interpret(parseResult.stmts);
	}

}

int runFile(std::string path, Interpreter& interpreter, GC& gc) {
	std::ifstream input{path};
	if (!input) return 69;
	std::stringstream buffer;
	buffer << input.rdbuf();
	run(buffer.str(), interpreter, gc);

	if (Error::hadError) return 65;
	if (Error::hadRuntimeError) return 70;
	return 0;
}

void runPrompt(Interpreter& interpreter, GC& gc) {
	while (true) {
		std::cout << "> ";

		std::string line;
		getline(std::cin, line);

		if (line.empty()) break;
		run(line, interpreter, gc);
		Error::hadError = false;
	}
}

int main(int argc, char* argv[]) {
	GC gc = GC();
	Interpreter interpreter = Interpreter(gc);

	if (argc > 2) {
		std::cout << "Usage: LoxInterpreterCPP [script]\n";
		return 64;
	}
	else if (argc == 2) {
		return runFile(argv[1], interpreter, gc);
	}
	else {
		runPrompt(interpreter, gc);
	}
	
	gc.deleteAll();
}
