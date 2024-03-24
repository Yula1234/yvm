#pragma once

#include <algorithm>
#include <cstdio>
#include <cassert>
#include <string>

#include "parser.hpp"

void consume_un(...) {

}

#define REG_V0 0
#define REG_V1 1

int __reg_to_no(std::string reg) {
	if(reg == "v0") {
		return REG_V0;
	}
	if(reg == "v1") {
		return REG_V1;
	}
	assert(false && "unkown register");
}

typedef enum {
	INSTR_PUSH = 0,
	INSTR_POP = 1,
	INSTR_SYSCALL = 2,
	INSTR_MOV_V0 = 3,
	INSTR_MOV_V1 = 4,
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

		fwrite("YM", sizeof(char), 2, file);
		fwrite("\0\0\0\0\0\0", sizeof(char), 6, file);
		
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

			void operator()(const NodeStmtPop* stmt_pop) const
			{
				int REG = __reg_to_no(stmt_pop->reg);
				Instr in = { .type = INSTR_POP, .operand = REG };
				gen.m_output << in;
			}

			void operator()(const NodeStmtMov* stmt_mov) const
			{
				NodeExpr* to = stmt_mov->to;
				NodeExpr* expr = stmt_mov->expr;
				if(!std::holds_alternative<NodeExprReg*>(to->var)) {
					gen.GeneratorError(stmt_mov->def, "except register at left");
				}
				if(!std::holds_alternative<NodeExprIntLit*>(expr->var)) {
					gen.GeneratorError(stmt_mov->def, "except int literal at right");
				}
				NodeExprIntLit* lit = std::get<NodeExprIntLit*>(expr->var);
				int REG = __reg_to_no(std::get<NodeExprReg*>(to->var)->name);
				Instr in;
				if(REG == REG_V0) {
					in = { .type = INSTR_MOV_V0 , .operand = std::stoi(lit->int_lit.value.value()) };
				}
				else if(REG == REG_V1) {
					in = { .type = INSTR_MOV_V1 , .operand = std::stoi(lit->int_lit.value.value()) };
				}
				else {
					assert(false && "unreacheable");
				}
				gen.m_output << in;
			}

			void operator()(const NodeStmtSyscall* stmt_syscall) const
			{
				consume_un(stmt_syscall);
				Instr in = { .type = INSTR_SYSCALL, .operand = 0 };
				gen.m_output << in;
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