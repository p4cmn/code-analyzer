#pragma once
#include "IParser.h"
#include "../Table/LRTable.h"
#include "../Grammar/Grammar.h"

#include <stack>
#include <vector>

/**
 * @brief Конкретная реализация LR(1)-анализатора, использующего:
 *   - ILexer (для токенов)
 *   - LRTable (ACTION/GOTO)
 *   - Grammar (чтобы знать, какие продукции REDUCE-им)
 *   - IASTBuilder (для построения AST при свёртке)
 *
 * Алгоритм (упрощённо):
 *   1. Инициализируем стек состояний (push 0).
 *   2. Считываем первый токен (currentToken).
 *   3. Пока не получим ACCEPT:
 *      - смотрим ACTION[ topOfStack, currentToken.type ]
 *      - если SHIFT s:
 *         push s в стек, push createTerminal(...) в AST-стек, currentToken = getNextToken()
 *      - если REDUCE r:
 *         смотрим продукцию grammar.productions[r], пусть это A -> α
 *         снимаем |α| символов/узлов со стеков
 *         создаём новый узел newNode = builder->createNode(A, r, poppedNodes)
 *         смотрим на topOfStack => t
 *         nextState = GOTO[t, A]
 *         push nextState в стек, push newNode в AST-стек
 *      - если ACCEPT:
 *         возвращаем верхний узел AST-стека
 *      - иначе ERROR
 */
class LRParser final : public IParser {
public:
    /**
     * @brief Конструктор LR-парсера.
     * @param lexer Указатель на лексер, который даёт токены.
     * @param table Сформированная LR-таблица (ACTION/GOTO).
     * @param grammar Исходная грамматика (для продукций при REDUCE).
     * @param astBuilder Объект, создающий узлы AST.
     */
    LRParser(ILexer* lexer,
             const LRTable& table,
             const Grammar& grammar,
             IASTBuilder* astBuilder);

    ~LRParser() override = default;

    /**
     * @see IParser::parse
     */
    std::shared_ptr<ASTNode> parse() override;

private:
    ILexer* m_lexer;
    const LRTable& m_table;
    const Grammar& m_grammar;
    IASTBuilder* m_astBuilder;

    // Вспомогательные структуры:
    std::vector<int> m_stateStack;  ///< Стек состояний LR
    std::vector<std::shared_ptr<ASTNode>> m_astStack; ///< Стек узлов AST

    /**
     * @brief Берём очередной токен из лексера, если нужно.
     */
    Token getNextToken();

    /**
     * @brief Выполняем SHIFT-действие: кладём в стек новое состояние,
     *        создаём терминальный узел, кладём в AST-стек.
     */
    void doShift(int nextState, const Token& tok);

    /**
     * @brief Выполняем REDUCE-действие по заданному индексу продукции.
     */
    void doReduce(int productionIndex);

    /**
     * @brief Возвращаем ACTION[state][terminal], либо LRAction{ERROR} если не найдено.
     */
    LRAction getAction(int state, const std::string& terminal) const;

    /**
     * @brief Возвращаем goTo[state][nonTerminal], либо -1 если не найдено.
     */
    int getGoto(int state, const std::string& nonTerminal) const;
};
