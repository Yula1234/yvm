#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cstring>

#include "generation.hpp"

void usage(std::ostream& stream) {
	stream << "Incorrect usage. Correct usage is..." << std::endl;
	stream << "yasm <flags> <input.yasm>" << std::endl;
}

enum class Flags {
	run,
};

std::vector<Flags> collect_flags(int argc, char* argv[]) {
	std::vector<Flags> flags;
	for(int i = 1;i < argc && argv[i][0] == '-';++i) {
		if(strcmp(argv[i], "-r") == 0) {
			flags.push_back(Flags::run);
		}
	}
	return flags;
}

bool find_flag(std::vector<Flags> flags, Flags f) {
	return std::find(flags.begin(), flags.end(), f) != flags.end();
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		usage(std::cerr);
		return EXIT_FAILURE;
	}

	std::string contents;
	{
		std::stringstream contents_stream;
		std::fstream input(argv[argc-1], std::ios::in);
		contents_stream << input.rdbuf();
		contents = contents_stream.str();
	}

	std::vector<Flags> flags = collect_flags(argc, argv);

	Lexer lexer(std::move(contents));
	std::vector<Token> tokens = lexer.lex(argv[argc-1]);

	Parser parser(std::move(tokens));
	std::optional<NodeProg> prog = parser.parse_prog();

	if (!prog.has_value()) {
		std::cerr << "Invalid program" << std::endl;
		exit(EXIT_FAILURE);
	}

	Generator generator(prog.value());
	generator.gen_prog();

	if(find_flag(flags, Flags::run)) {
		// run out.bin in YVM
		return system("yvm out.bin");
	}

	return EXIT_SUCCESS;
}
