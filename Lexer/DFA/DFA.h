#pragma once
#include <vector>

/**
 * @brief Состояние детерминированного конечного автомата (DFA).
 */
struct DfaState {
    int transitions[256];
    bool isAccept;
    int tokenIndex;
};

/**
 * @brief Структура детерминированного конечного автомата (DFA).
 */
struct DFA {
    std::vector<DfaState> states;
    int startState;
};
