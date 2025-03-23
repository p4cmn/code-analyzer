#pragma once
#include <vector>

/**
 * @brief Структура состояния недетерминированного конечного автомата (NFA).
 */
struct NFAState {
    bool isAccept = false;
    int tokenIndex = -1;
    std::vector<int> transitions[256];
    std::vector<int> epsilon;
};

/**
 * @brief Структура NFA: набор состояний, индекс стартового состояния и индекс
 *        единственного принимающего состояния (или -1, если их несколько).
 */
struct NFA {
    std::vector<NFAState> states;
    int startState = -1;
    int acceptState = -1;
};
