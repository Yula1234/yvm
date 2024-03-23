#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "lexer.hpp"

class Parser {
private:
	std::vector<Token> m_tokens {};
	size_t m_offset = 0UL;
	[[nodiscard]] std::optional<Token> peek(const int offset = 0) const {
		if (m_index + offset >= m_tokens.size()) {
			return {};
		}
		return m_tokens.at(m_index + offset);
	}
	Token consume() {
		return m_tokens.at(m_index++);
	}
public:
	Parser(std::vector<Token> _tokens) {
		m_tokens = std::move(_tokens);
	}
	NodeProg* parse() {
		
	}
}