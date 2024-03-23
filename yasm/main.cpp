#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>
#include <cstdio>

#include "generation.hpp"

void usage(std::ostream& stream) {
	stream << "Incorrect usage. Correct usage is..." << std::endl;
	stream << "yasm <input.yasm>" << std::endl;
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		usage(std::cerr);
		return EXIT_FAILURE;
	}

	std::string contents;
	{
		std::stringstream contents_stream;
		std::fstream input(argv[1], std::ios::in);
		contents_stream << input.rdbuf();
		contents = contents_stream.str();
	}

	Lexer lexer(std::move(contents));
	std::vector<Token> tokens = lexer.lex(argv[1]);

	Parser parser(std::move(tokens));
	std::optional<NodeProg> prog = parser.parse_prog();

	if (!prog.has_value()) {
		std::cerr << "Invalid program" << std::endl;
		exit(EXIT_FAILURE);
	}

	Generator generator(prog.value());
	generator.gen_prog();

	return EXIT_SUCCESS;
}