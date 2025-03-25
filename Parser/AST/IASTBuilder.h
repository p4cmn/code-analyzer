#pragma once
#include "ASTNode.h"

#include <memory>
#include <vector>

/**
 * @brief Интерфейс строителя AST, который вызывается при свёртке (reduce) в LR-анализаторе.
 */
class IASTBuilder {
public:
    virtual ~IASTBuilder() = default;

    /**
     * @brief Создаёт (или «склеивает») новый узел AST при свёртке продукции.
     *
     * @param nonTerminal Левая часть продукции (например, "E").
     * @param productionIndex Индекс продукции (или любая другая идентификация правила).
     * @param childNodes Вектор дочерних узлов, соответствующих правой части продукции.
     * @return Новый узел AST (обычно нетерминал).
     */
    virtual std::shared_ptr<ASTNode> createNode(
            const std::string& nonTerminal,
            int productionIndex,
            const std::vector<std::shared_ptr<ASTNode>>& childNodes
    ) = 0;

    /**
     * @brief Создаёт узел-терминал (лист). Обычно вызывается, когда лексер возвращает токен.
     *
     * @param terminalSymbol Имя терминала (например, "id" или "+").
     * @return Узел-терминал.
     */
    virtual std::shared_ptr<ASTNode> createTerminal(const std::string& terminalSymbol) = 0;
};
