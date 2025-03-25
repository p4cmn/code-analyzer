#pragma once
#include <string>
#include <vector>
#include <memory>

/**
 * @brief Узел абстрактного синтаксического дерева (AST).
 *
 * - `symbol` — имя нетерминала или терминала (например, "E", "T", "id", "+").
 * - `isTerminal` — флаг, указывающий, является ли этот узел терминалом.
 * - `children` — вектор дочерних узлов (для нетерминала).
 */
struct ASTNode {
    std::string symbol;
    bool isTerminal;
    std::vector<std::shared_ptr<ASTNode>> children;

    /**
     * @brief Конструктор узла.
     * @param sym Имя символа (нетерминал или терминал).
     * @param terminal Флаг: true, если это терминал; false, если нетерминал.
     */
    ASTNode(const std::string& sym, bool terminal)
            : symbol(sym), isTerminal(terminal) {}
};
