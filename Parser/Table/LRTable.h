#pragma once
#include <map>
#include <string>

/**
 * @brief Тип действия в ячейке ACTION.
 */
enum class LRActionType {
    SHIFT,
    REDUCE,
    ACCEPT,
    ERROR
};

/**
 * @brief Хранит информацию об одном действии в таблице ACTION.
 *
 * - SHIFT:  type = SHIFT,  nextState = индекс состояния
 * - REDUCE: type = REDUCE, productionIndex = индекс правила в grammar.productions
 * - ACCEPT: type = ACCEPT
 * - ERROR:  type = ERROR
 */
struct LRAction {
    LRActionType type;
    int nextState;       ///< используется при SHIFT
    int productionIndex; ///< используется при REDUCE

    LRAction()
            : type(LRActionType::ERROR)
            , nextState(-1)
            , productionIndex(-1)
    {}
};

/**
 * @brief Структура, содержащая таблицы ACTION и GOTO.
 *
 * - action[state][terminal] -> LRAction
 * - goTo[state][nonterminal] -> int (индекс следующего состояния)
 */
struct LRTable {
    std::map<int, std::map<std::string, LRAction>> action; ///< ACTION
    std::map<int, std::map<std::string, int>> goTo;         ///< GOTO
};
