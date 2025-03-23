#pragma once
#include <string>

struct Token {
    std::string type;
    std::string lexeme;
    int line;
    int column;
    int symbolId;

    Token()
            : type("UNKNOWN"),
              lexeme(""),
              line(0),
              column(0),
              symbolId(-1)
    {}

    Token(const std::string& t, const std::string& lex, int ln, int col, int symId = -1)
            : type(t), lexeme(lex), line(ln), column(col), symbolId(symId)
    {}
};
