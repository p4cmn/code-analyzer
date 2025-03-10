#pragma once
#include <vector>
#include "../NFA/NFABuilder.h"

struct DfaState {
    int transitions[256];
    bool isAccept;
    int tokenIndex;
};

struct DFA {
    std::vector<DfaState> states;
    int startState;
};

DFA subsetConstruction(const NFA &nfa);
