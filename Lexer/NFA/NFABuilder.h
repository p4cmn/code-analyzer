#pragma once
#include "../Regex/RegexAST.h"
#include <memory>
#include <vector>
#include <string>

struct NFAState {
    bool isAccept = false;
    int tokenIndex = -1;
    std::vector<int> transitions[256];
    std::vector<int> epsilon;
};

struct NFA {
    std::vector<NFAState> states;
    int startState;
    int acceptState;
};

class NFABuilder {
public:
    static NFA buildFromAST(const std::shared_ptr<RegexAST>& ast);
    static NFA buildCombinedNFA(const std::vector<std::shared_ptr<RegexAST>>& asts,
                                const std::vector<int>& tokenIndices);
};

std::string parseCharClass(const std::string &charClassExpr);