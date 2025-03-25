#pragma once
#include "ASTNode.h"
#include "IASTBuilder.h"

/**
 * @brief Пример конкретной реализации IASTBuilder.
 *
 * Здесь мы просто создаём узлы, в которых:
 *  - symbol = nonTerminal или terminalSymbol,
 *  - isTerminal = (true/false),
 *  - children = те, что переданы (для нетерминала).
 */
class ASTBuilder final : public IASTBuilder {
public:
    ASTBuilder() = default;
    ~ASTBuilder() override = default;

    std::shared_ptr<ASTNode> createNode(
            const std::string& nonTerminal,
            int productionIndex,
            const std::vector<std::shared_ptr<ASTNode>>& childNodes
    ) override;

    std::shared_ptr<ASTNode> createTerminal(const std::string& terminalSymbol) override;
};
