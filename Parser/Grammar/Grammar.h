#pragma once

#include <string>
#include <vector>

/**
 * @brief Структура, описывающая одну продукцию грамматики, например:
 *        E : E + T
 */
struct Production {
    std::string left;                 ///< Левая часть (нетерминал)
    std::vector<std::string> right;   ///< Правая часть (список терминалов/нетерминалов)
};

/**
 * @brief Структура, описывающая грамматику: терминалы, нетерминалы, стартовый символ, список продукций.
 */
struct Grammar {
    std::vector<std::string> terminals;
    std::vector<std::string> nonterminals;
    std::string startSymbol;
    std::vector<Production> productions;
};
