#pragma once
#include <memory>
#include <string>

/**
 * @brief Тип узла в AST регулярного выражения.
 */
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

/**
 * @brief Узел AST для регулярного выражения.
 *
 * Содержит:
 *   - тип узла (RegexNodeType)
 *   - literal / charClass / left / right (в зависимости от типа)
 */
struct RegexAST {
    RegexNodeType type;
    char literal;
    std::string charClass;
    std::shared_ptr<RegexAST> left;
    std::shared_ptr<RegexAST> right;

    explicit RegexAST(RegexNodeType t)
            : type(t), literal('\0'), charClass(""), left(nullptr), right(nullptr)
    {}
};
