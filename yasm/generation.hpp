#pragma once

#include <algorithm>
#include <cstdio>
#include <cassert>
#include <string>

#include "parser.hpp"



typedef enum {
	INSTR_PUSH,
} InstrType;

typedef struct Instr {
	InstrType type;
	int operand;
} Instr;

typedef struct Yvm_Out_file {
	size_t m_count = 0ULL;
	Instr m_code[65000];
	
	friend void operator<<(Yvm_Out_file& outf, Instr in) {
		outf.m_code[outf.m_count] = in;
		outf.m_count += 1ULL;
	}

	void write(std::string path) {
		FILE* file = fopen(path.c_str(), "wb");
		fwrite(reinterpret_cast<char*>(m_code), sizeof(Instr), m_count, file);
		fclose(file);
	}
} Yvm_Out_file;

class Generator {
public:
	explicit Generator(NodeProg prog)
		: m_prog(std::move(prog))
	{
	}

	void GeneratorError(Token tok, std::string msg) {
		putloc(tok);
		std::cout << " ERROR: " << msg << "\n";
		exit(EXIT_FAILURE);
	}

	Instr m_compile_int(NodeExprIntLit* expr) {
		return { .type = INSTR_PUSH, .operand = std::stoi(expr->int_lit.value.value()) };
	}

	Instr gen_expr(const NodeExpr* expr)
	{
		if(std::holds_alternative<NodeExprIntLit*>(expr->var)) {
			return m_compile_int(std::get<NodeExprIntLit*>(expr->var));
		}
		assert(false && "unreacheable");
	}

	void gen_stmt(const NodeStmt* stmt)
	{
		struct StmtVisitor {
			Generator& gen;

			void operator()(const NodeStmtPush* stmt_push) const
			{
				gen.m_output << gen.gen_expr(stmt_push->expr);
			}
		};

		StmtVisitor visitor { .gen = *this };
		std::visit(visitor, stmt->var);
	}

	void gen_prog()
	{
		for (const NodeStmt* stmt : m_prog.stmts) {
			gen_stmt(stmt);
		}
		m_output.write("out.bin");
	}

private:

	const NodeProg m_prog;
	Yvm_Out_file m_output;
};