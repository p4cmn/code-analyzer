#pragma once
#include <string>
#include "TokenType.h"

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    int symbolId;

    Token()
            : type(TokenType::UNKNOWN),
              lexeme(""),
              line(0),
              column(0),
              symbolId(-1)
    {}

    Token(TokenType t, const std::string& lex, int ln, int col, int symId = -1)
            : type(t), lexeme(lex), line(ln), column(col), symbolId(symId)
    {}
};
