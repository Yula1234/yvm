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
	INSTR_JMP = 5,
	INSTR_ADD = 6,
	INSTR_SUB = 7,
	INSTR_MUL = 8,
	INSTR_DIV = 9,
	INSTR_RPUSH = 10,
	INSTR_PUSH_IP = 11,
	INSTR_PUSH_BP = 12,
	INSTR_PUSH_SP = 13,
	INSTR_JMP_ONSTACK = 14,
} InstrType;

typedef struct Instr {
	InstrType type;
	int operand;
} Instr;

typedef struct Yvm_Out_file {
	size_t m_count = 0ULL;
	Instr m_code[65000];
	
	friend Yvm_Out_file& operator<<(Yvm_Out_file& outf, Instr in) {
		outf.m_code[outf.m_count] = in;
		outf.m_count += 1ULL;
		return outf;
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
	struct Label {
		size_t addr;
		std::string name;
	};

	struct UnresolvedSymbol {
		Instr* in;
		std::string symbol;
		Token def;
	};

	std::optional<Label> label_lookup(std::string name) {
		for(int i = 0;i < static_cast<int>(m_labels.size());++i) {
			if(m_labels[i].name == name) {
				return m_labels[i];
			}
		}
		return std::nullopt;
	}

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

	Instr m_compile_ident(NodeExprIdent* ident) {
		std::optional<Label> label = label_lookup(ident->ident.value.value());
		if(!label.has_value()) {
			Instr in = { .type = INSTR_JMP, .operand = 0 };
			m_unresolved_symbols.push_back({ .in = &m_output.m_code[m_output.m_count - 1] , .symbol = ident->ident.value.value(), .def = ident->ident });
			m_has_entry = true;
			return in;
		}
		m_has_entry = true;
		Instr in = { .type = INSTR_JMP, .operand = static_cast<int>(label.value().addr) };
		return in;
	}

	Instr gen_expr(const NodeExpr* expr)
	{
		if(std::holds_alternative<NodeExprIntLit*>(expr->var)) {
			return m_compile_int(std::get<NodeExprIntLit*>(expr->var));
		}
		if(std::holds_alternative<NodeExprIdent*>(expr->var)) {
			return m_compile_ident(std::get<NodeExprIdent*>(expr->var));
		}
		assert(false && "unreacheable");
	}

	void gen_stmt(const NodeStmt* stmt)
	{
		struct StmtVisitor {
			Generator& gen;

			void operator()(const NodeStmtPush* stmt_push) const
			{
				if(std::holds_alternative<NodeExprIntLit*>(stmt_push->expr->var)) {
					gen.m_output << gen.gen_expr(stmt_push->expr);
				}
				if(std::holds_alternative<NodeExprReg*>(stmt_push->expr->var)) {
					NodeExprReg* reg_p = std::get<NodeExprReg*>(stmt_push->expr->var);
					int REG = __reg_to_no(reg_p->name);
					Instr in = { .type = INSTR_RPUSH, .operand = REG };
					gen.m_output << in;
				}
				if(std::holds_alternative<NodeExprIdent*>(stmt_push->expr->var)) {
					gen.m_output << gen.gen_expr(stmt_push->expr);
				}
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

			void operator()(const NodeStmtIpush* stmt_ipush) const
			{
				consume_un(stmt_ipush);
				Instr in = { .type = INSTR_PUSH_IP, .operand = 0 };
				gen.m_output << in;
			}

			void operator()(const NodeStmtSpush* stmt_spush) const
			{
				consume_un(stmt_spush);
				Instr in = { .type = INSTR_PUSH_SP, .operand = 0 };
				gen.m_output << in;
			}

			void operator()(const NodeStmtBpush* stmt_bpush) const
			{
				consume_un(stmt_bpush);
				Instr in = { .type = INSTR_PUSH_BP, .operand = 0 };
				gen.m_output << in;
			}

			void operator()(const NodeStmtSjmp* stmt_sjmp) const
			{
				consume_un(stmt_sjmp);
				Instr in = { .type = INSTR_JMP_ONSTACK, .operand = 0 };
				gen.m_output << in;
			}

			void operator()(const NodeStmtAdd* stmt_add) const
			{
				consume_un(stmt_add);
				Instr in = { .type = INSTR_ADD, .operand = 0 };
				gen.m_output << in;
			}

			void operator()(const NodeStmtSub* stmt_sub) const
			{
				consume_un(stmt_sub);
				Instr in = { .type = INSTR_SUB, .operand = 0 };
				gen.m_output << in;
			}

			void operator()(const NodeStmtMul* stmt_mul) const
			{
				consume_un(stmt_mul);
				Instr in = { .type = INSTR_MUL, .operand = 0 };
				gen.m_output << in;
			}

			void operator()(const NodeStmtDiv* stmt_div) const
			{
				consume_un(stmt_div);
				Instr in = { .type = INSTR_DIV, .operand = 0 };
				gen.m_output << in;
			}

			void operator()(const NodeStmtLabel* stmt_label) const
			{
				gen.m_labels.push_back({ .addr = gen.m_output.m_count , .name = stmt_label->name });
			}

			void operator()(const NodeStmtJmp* stmt_jmp) const
			{
				std::optional<Label> label = gen.label_lookup(stmt_jmp->label);
				if(!label.has_value()) {
					Instr in = { .type = INSTR_JMP, .operand = 0 };
					gen.m_output << in;
					gen.m_unresolved_symbols.push_back({ .in = &gen.m_output.m_code[gen.m_output.m_count - 1] , .symbol = stmt_jmp->label, .def = stmt_jmp->def });
					return;
				}
				Instr in = { .type = INSTR_JMP, .operand = static_cast<int>(label.value().addr) };
				gen.m_output << in;
			}

			void operator()(const NodeStmtEntry* stmt_entry) const
			{
				std::optional<Label> label = gen.label_lookup(stmt_entry->name);
				if(!label.has_value()) {
					Instr in = { .type = INSTR_JMP, .operand = 0 };
					gen.m_output << in;
					gen.m_unresolved_symbols.push_back({ .in = &gen.m_output.m_code[gen.m_output.m_count - 1] , .symbol = stmt_entry->name, .def = stmt_entry->def });
					gen.m_has_entry = true;
					return;
				}
				gen.m_has_entry = true;
				Instr in = { .type = INSTR_JMP, .operand = static_cast<int>(label.value().addr) };
				gen.m_output << in;
			}
		};

		StmtVisitor visitor { .gen = *this };
		std::visit(visitor, stmt->var);
	}

	void gen_prog()
	{
		for(const NodeStmt* stmt : m_prog.stmts) {
			gen_stmt(stmt);
		}
		if(!m_has_entry) {
			std::cerr << "entry not provided!\n";
			exit(1);
		}
		for(int i = 0;i < static_cast<int>(m_unresolved_symbols.size());++i) {
			UnresolvedSymbol us = m_unresolved_symbols[i];
			std::optional<Label> label = label_lookup(us.symbol);
			if(label.has_value()) {
				us.in->operand = static_cast<int>(label.value().addr);
				continue;
			}
			GeneratorError(us.def, "undefined symbol `" + us.symbol + "`");
		}
		m_output.write("out.bin");
	}

private:
	const NodeProg m_prog;
	bool m_has_entry = false;
	std::vector<Label> m_labels;
	std::vector<UnresolvedSymbol> m_unresolved_symbols;
	Yvm_Out_file m_output;
};