#pragma once
#include "NFA.h"
#include "../Regex/RegexAST.h"

#include <memory>
#include <vector>

/**
 * @brief Интерфейс строителя NFA (по алгоритму Томпсона).
 */
class INFABuilder
{
public:
    virtual ~INFABuilder() = default;

    /**
     * @brief Строит NFA по одному AST (регулярному выражению).
     * @param ast Корень AST регулярного выражения.
     * @return Построенный NFA (с одним start и одним accept).
     */
    virtual NFA buildFromAST(const std::shared_ptr<RegexAST> &ast) = 0;

    /**
     * @brief Строит общий NFA, объединяя несколько регулярных выражений.
     *
     * Создаёт новое стартовое состояние, из которого epsilon-переходы ведут
     * к стартовым состояниям всех NFA. Каждое принимающее состояние помечается
     * своим tokenIndex из массива tokenIndices.
     *
     * @param asts Массив AST (каждое — отдельное регулярное выражение).
     * @param tokenIndices Массив индексов, по одному для каждого AST.
     * @throws std::runtime_error Если размерность asts и tokenIndices не совпадает.
     */
    virtual NFA buildCombinedNFA(const std::vector<std::shared_ptr<RegexAST>> &asts,
                                 const std::vector<int> &tokenIndices) = 0;
};
