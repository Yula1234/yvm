#pragma once

#include <string>
#include <vector>
#include <cstdio>

enum class TokenType {
    push,
    int_lit,
    ident
};

std::string tok_to_string(const TokenType type)
{
    switch (type) {
    case TokenType::push:
        return "`push`";
    case TokenType::int_lit:
        return "`int literal`";
    case TokenType::ident:
        return "`ident`";
    }
    assert(false);
}

struct Token {
    TokenType type;
    int line;
    int col;
    std::optional<std::string> value {};
    std::string file;
    friend std::ostream& operator<<(std::ostream& out, const Token& tok) {
        out << "Token(.type = " << tok_to_string(tok.type);
        out << ", .line = " << tok.line;
        out << ", .col = " << tok.col;
        if(tok.value.has_value()) {
            out << ", .value = " << tok.value.value();
        }
        out << ")";
        return out;
    }
};

void putloc(Token tok) {
    printf("%s %d:%d", tok.file.c_str(), tok.line, tok.col);
}

std::string loc_of(Token tok) {
    static char buffer[2048];
    sprintf(buffer, "%s %d:%d", tok.file.c_str(), tok.line, tok.col);
    std::string str(buffer);
    return str;
}

bool is_valid_id(char c) {
    switch(c) {
    case '_':
        return true;
    default:
        return false;
    }
    return false;
}

class Lexer {
public:
    explicit Lexer(std::string src)
        : m_src(std::move(src))
    {
    }

    std::vector<Token> lex(std::string file)
    {
        std::vector<Token> tokens;
        std::string buf;
        int line_count = 1;
        while (peek().has_value()) {
            if (std::isalpha(peek().value()) || is_valid_id(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && (std::isalnum(peek().value()) || is_valid_id(peek().value()))) {
                    buf.push_back(consume());
                }
                if (buf == "push") {
                    tokens.push_back({ .type = TokenType::push, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else {
                    tokens.push_back({ .type = TokenType::ident, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .value = buf, .file = file });
                    buf.clear();
                }
            }
            else if (std::isdigit(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && std::isdigit(peek().value())) {
                    buf.push_back(consume());
                }
                tokens.push_back({ .type = TokenType::int_lit, .line = line_count, .col = m_col - static_cast<int>(buf.size()), .value = buf, .file = file });
                buf.clear();
            }
            else if (peek().value() == ';') {
                consume();
                consume();
                while (peek().has_value() && peek().value() != '\n') {
                    consume();
                }
            }
            else if(peek().value() == '\'') {
                consume();
                buf.clear();
                while(peek().has_value() && peek().value() != '\'') {
                    buf.push_back(consume());
                }
                consume();
                for(int i = 0;i < static_cast<int>(buf.size());++i) {
                    if(buf[i] == '\\') {
                        if(buf[i+1] == 'n') {
                            buf.erase(buf.begin()+i);
                            buf[i] = '\n';
                        }
                    }
                }
                tokens.push_back({ .type = TokenType::int_lit, .line = line_count , .col = m_col - static_cast<int>(buf.size()), .value = std::to_string(static_cast<int>(buf[0])), .file = file });
                buf.clear();
            }
            else if (peek().value() == '\n') {
                consume();
                m_col = 1;
                line_count++;
            }
            else if (std::isspace(peek().value())) {
                consume();
            }
            else {
                std::cerr << "Invalid token" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        m_index = 0;
        return tokens;
    }

private:
    [[nodiscard]] std::optional<char> peek(const size_t offset = 0) const
    {
        if (m_index + offset >= m_src.length()) {
            return {};
        }
        return m_src.at(m_index + offset);
    }

    char consume()
    {
        m_col++;
        return m_src.at(m_index++);
    }

    const std::string m_src;
    size_t m_index = 0;
    int m_col = 1;
};