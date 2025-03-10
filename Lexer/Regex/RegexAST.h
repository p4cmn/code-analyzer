#pragma once
#include <memory>
#include <string>

enum class RegexNodeType {
    Literal,
    Epsilon,
    CharClass,
    Concat,
    Alt,
    Star,
    Plus,
    Question
};

struct RegexAST {
    RegexNodeType type;
    char literal;
    std::string charClass;
    std::shared_ptr<RegexAST> left;
    std::shared_ptr<RegexAST> right;
    explicit RegexAST(RegexNodeType type) : type(type), literal('\0') {}
};
