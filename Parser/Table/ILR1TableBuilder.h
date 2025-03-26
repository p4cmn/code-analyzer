#pragma once
#include "LRTable.h"
#include "../Grammar/Grammar.h"

/**
 * @brief Интерфейс для построения таблицы LR(1) (ACTION/GOTO) на основе грамматики.
 */
class ILR1TableBuilder {
public:
    virtual ~ILR1TableBuilder() = default;

    /**
     * @brief Формирует LR-таблицу (ACTION/GOTO) по заданной грамматике.
     * @param grammar Грамматика (с уже заполненными полями: terminals, nonterminals, productions, startSymbol).
     * @return Объект LRTable, содержащий таблицы ACTION и GOTO.
     * @throws std::runtime_error При возникновении конфликтов или некорректных данных.
     */
    virtual LRTable build(const Grammar& grammar) = 0;
};
