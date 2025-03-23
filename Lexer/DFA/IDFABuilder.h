#pragma once
#include "DFA.h"
#include "../NFA/NFA.h"

/**
 * @brief Интерфейс построителя DFA из NFA.
 */
class IDFABuilder {
public:
    virtual ~IDFABuilder() = default;

    /**
     * @brief Строит детерминированный автомат (DFA) из недетерминированного (NFA).
     * @param nfa Недетерминированный автомат
     * @return Детерминированный автомат
     */
    virtual DFA buildFromNFA(const NFA &nfa) = 0;
};
