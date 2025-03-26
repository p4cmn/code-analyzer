#pragma once
#include "INFABuilder.h"

/**
 * @brief Класс-строитель NFA по алгоритму Томпсона.
 *
 * Предоставляет функции для:
 *  - Построения NFA из одного регулярного выражения (AST).
 *  - Объединения нескольких NFA (для разных выражений) в один.
 */
class ThompsonNFABuilder : public INFABuilder {
public:
    ThompsonNFABuilder() = default;
    ~ThompsonNFABuilder() override = default;

    /**
     * @see INFABuilder::buildFromAST
     */
    NFA buildFromAST(const std::shared_ptr<RegexAST> &ast) override;

    /**
     * @see INFABuilder::buildCombinedNFA
     */
    NFA buildCombinedNFA(const std::vector<std::shared_ptr<RegexAST>> &asts,
                         const std::vector<int> &tokenIndices) override;

/**
 * @brief Строит базовый NFA для одного символа c (или ε, если c == '\0').
 *        Результат: 2 состояния: start -> accept.
 * @param c Символ, либо '\0' для эпсилон.
 */
NFA buildBasicNFA(char c);

private:
    /**
     * @brief Рекурсивное построение NFA из AST (одна регулярка).
     * @param ast Корень AST.
     */
    NFA buildFromASTImpl(const std::shared_ptr<RegexAST> &ast);

    /**
     * @brief Создаёт NFA, соответствующий объединению A|B (альтернатива).
     */
    NFA alternateNFA(const NFA &a, const NFA &b);

    /**
     * @brief Создаёт NFA для конкатенации A B.
     */
    NFA concatNFA(const NFA &a, const NFA &b);

    /**
     * @brief Создаёт NFA для звезды Клини (A*).
     */
    NFA starNFA(const NFA &a);

    /**
     * @brief Создаёт NFA для A+ (один или более повторений).
     */
    NFA plusNFA(const NFA &a);

    /**
     * @brief Создаёт NFA для A? (ноль или одно вхождение).
     */
    NFA questionNFA(const NFA &a);

    /**
     * @brief Добавляет новое состояние в NFA, возвращает его индекс.
     */
    int addState(NFA &nfa);

    /**
     * @brief Копирует вектор состояний в другой NFA со смещением индексов.
     */
    std::vector<NFAState> copyStatesWithOffset(const std::vector<NFAState> &states, int offset);

    /**
     * @brief Разворачивает диапазоны в классе символов (например, "a-z" -> "abc...xyz").
     */
    std::string expandCharClass(const std::string &charClassExpr);
};
