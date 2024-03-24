#pragma once

#include <cassert>
#include <variant>
#include <filesystem>

#include "arena.hpp"
#include "lexer.hpp"

#define yforeach(container) for(int i = 0;i < static_cast<int>(container.size());++i)

struct NodeExprIntLit {
	Token int_lit;
};

struct NodeExprReg {
	Token def;
	std::string name;
};

struct NodeExpr {
	std::variant<NodeExprIntLit*, NodeExprReg*> var;
};

struct NodeStmtPush {
	Token def;
	NodeExpr* expr;
};

struct NodeStmtMov {
	Token def;
	NodeExpr* to;
	NodeExpr* expr;
};

struct NodeStmtPop {
	Token def;
	std::string reg;
};

struct NodeStmtSyscall {
	Token def;
};

struct NodeStmt {
	std::variant<NodeStmtPush*, NodeStmtMov*,
				NodeStmtPop*, NodeStmtSyscall*> var;
};

struct NodeProg {
	std::vector<NodeStmt*> stmts {};
};

bool file_exists(std::string name) {
	if(FILE *file = fopen(name.c_str(), "r")) {
		fclose(file);
		return true;
	} else {
		return false;
	}
}

class Parser {
public:
	explicit Parser(std::vector<Token> tokens)
		: m_tokens(std::move(tokens))
		, m_allocator(1024 * 1024 * 24) // 24 mb
	{
	}

	void ParsingError(const std::string& msg, const int pos = 0) const
	{
		putloc(peek(pos).value());
		std::cout << " ERROR: " << msg << "\n";
		exit(EXIT_FAILURE);
	}

	void error_expected(const std::string& msg) const
	{
		putloc(peek(-1).value());
		if(peek().has_value()) {
			std::cout << " ERROR: excepted " << msg << ", but got " << tok_to_string(peek().value().type) << "\n";
		} else {
			std::cout << " ERROR: excepted " << msg << ", but got nothing\n";
		}
		exit(EXIT_FAILURE);
	}

	std::optional<NodeExpr*> parse_expr() // NOLINT(*-no-recursion)
	{
		if(auto int_lit = try_consume(TokenType::int_lit)) {
			auto expr_int = m_allocator.emplace<NodeExprIntLit>(int_lit.value());
			auto expr = m_allocator.emplace<NodeExpr>(expr_int);
			return expr;
		}
		if(auto _reg = try_consume(TokenType::reg)) {
			auto expr_reg = m_allocator.emplace<NodeExprReg>();
			expr_reg->def = _reg.value();
			expr_reg->name = _reg.value().value.value();
			auto expr = m_allocator.emplace<NodeExpr>(expr_reg);
			return expr;
		}
		if(auto ident = try_consume(TokenType::ident)) {
			assert(false && "unkown identifer");
		}
		return {};
	}

	std::optional<NodeStmt*> parse_stmt() // NOLINT(*-no-recursion)
	{
		if(auto _push = try_consume(TokenType::push)) {
			auto push_stmt = m_allocator.emplace<NodeStmtPush>();
			push_stmt->def = _push.value();
			if(auto expr = parse_expr()) {
				push_stmt->expr = expr.value();
			} else {
				error_expected("expression");
			}
			auto stmt = m_allocator.emplace<NodeStmt>(push_stmt);
			return stmt;
		}

		if(auto _pop = try_consume(TokenType::pop)) {
			auto pop_stmt = m_allocator.emplace<NodeStmtPop>();
			pop_stmt->def = _pop.value();
			pop_stmt->reg = try_consume_err(TokenType::reg).value.value();
			auto stmt = m_allocator.emplace<NodeStmt>(pop_stmt);
			return stmt;
		}

		if(auto _mov = try_consume(TokenType::mov)) {
			auto mov_stmt = m_allocator.emplace<NodeStmtMov>();
			mov_stmt->def = _mov.value();
			if(auto expr = parse_expr()) {
				mov_stmt->to = expr.value();
			} else {
				error_expected("expression");
			}
			try_consume_err(TokenType::comma);
			if(auto expr = parse_expr()) {
				mov_stmt->expr = expr.value();
			} else {
				error_expected("expression");
			}
			auto stmt = m_allocator.emplace<NodeStmt>(mov_stmt);
			return stmt;
		}

		if(auto syscall = try_consume(TokenType::syscall)) {
			auto syscall_stmt = m_allocator.emplace<NodeStmtSyscall>();
			syscall_stmt->def = syscall.value();
			auto stmt = m_allocator.emplace<NodeStmt>(syscall_stmt);
			return stmt;
		}

		// TODO: parse syscall

		return {};
	}

	std::optional<NodeProg> parse_prog()
	{
		NodeProg prog;
		while (peek().has_value()) {
			if (auto stmt = parse_stmt()) {
				prog.stmts.push_back(stmt.value());
			}
			else {
				error_expected("statement");
			}
		}
		return prog;
	}

private:
	[[nodiscard]] std::optional<Token> peek(const int offset = 0) const
	{
		if (m_index + offset >= m_tokens.size()) {
			return {};
		}
		return m_tokens.at(m_index + offset);
	}

	Token consume()
	{
		return m_tokens.at(m_index++);
	}

	Token try_consume_err(const TokenType type)
	{
		if (peek().has_value() && peek().value().type == type) {
			return consume();
		}
		error_expected(tok_to_string(type));
		return {};
	}

	std::optional<Token> try_consume(const TokenType type)
	{
		if (peek().has_value() && peek().value().type == type) {
			return consume();
		}
		return {};
	}

	std::vector<Token> m_tokens;
	size_t m_index = 0;
	ArenaAllocator m_allocator;
};