#pragma once

#include <string>
#include <vector>
#include <cstdio>

enum class TokenType {
    push,
    int_lit,
    ident,
    mov,
    syscall,
    pop,
    reg,
    comma,
    double_dot,
    jmp,
    add,
    sub,
    mul,
    div,
    entry,
    ipush,
    bpush,
    spush,
    sjmp,
    call,
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
    case TokenType::mov:
        return "`mov`";
    case TokenType::syscall:
        return "`syscall`";
    case TokenType::pop:
        return "`pop`";
    case TokenType::reg:
        return "`register`";
    case TokenType::comma:
        return "`,`";
    case TokenType::double_dot:
        return "`:`";
    case TokenType::jmp:
        return "`jmp`";
    case TokenType::add:
        return "`add`";
    case TokenType::sub:
        return "`sub`";
    case TokenType::mul:
        return "`mul`";
    case TokenType::div:
        return "`div`";
    case TokenType::entry:
        return "`entry`";
    case TokenType::ipush:
        return "`ipush`";
    case TokenType::spush:
        return "`spush`";
    case TokenType::bpush:
        return "`bpush`";
    case TokenType::sjmp:
        return "`sjmp`";
    case TokenType::call:
        return "`call`";
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
        while(peek().has_value()) {
            if(std::isalpha(peek().value()) || is_valid_id(peek().value())) {
                buf.push_back(consume());
                while(peek().has_value() && (std::isalnum(peek().value()) || is_valid_id(peek().value()))) {
                    buf.push_back(consume());
                }
                if(buf == "push") {
                    tokens.push_back({ .type = TokenType::push, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else if(buf == "pop") {
                    tokens.push_back({ .type = TokenType::pop, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else if(buf == "syscall") {
                    tokens.push_back({ .type = TokenType::syscall, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else if(buf == "mov") {
                    tokens.push_back({ .type = TokenType::mov, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else if(buf == "jmp") {
                    tokens.push_back({ .type = TokenType::jmp, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else if(buf == "add") {
                    tokens.push_back({ .type = TokenType::add, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else if(buf == "sub") {
                    tokens.push_back({ .type = TokenType::sub, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else if(buf == "mul") {
                    tokens.push_back({ .type = TokenType::mul, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else if(buf == "div") {
                    tokens.push_back({ .type = TokenType::div, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else if(buf == "entry") {
                    tokens.push_back({ .type = TokenType::entry, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else if(buf == "ipush") {
                    tokens.push_back({ .type = TokenType::ipush, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else if(buf == "bpush") {
                    tokens.push_back({ .type = TokenType::bpush, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else if(buf == "spush") {
                    tokens.push_back({ .type = TokenType::spush, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else if(buf == "sjmp") {
                    tokens.push_back({ .type = TokenType::sjmp, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else if(buf == "call") {
                    tokens.push_back({ .type = TokenType::call, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .file = file });
                    buf.clear();
                }
                else if(buf == "v0" || buf == "v1") {
                    tokens.push_back({ .type = TokenType::reg, .line =  line_count, .col =  m_col - static_cast<int>(buf.size()), .value = buf, .file = file });
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
            else if (peek().value() == ',') {
                consume();
                tokens.push_back({ .type = TokenType::comma, .line = line_count, .col = m_col - 1, .file = file });
            }
            else if (peek().value() == ':') {
                consume();
                tokens.push_back({ .type = TokenType::double_dot, .line = line_count, .col = m_col - 1, .file = file });
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