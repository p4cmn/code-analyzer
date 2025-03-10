#pragma once
#include "Token/Token.h"

class ILexer {
public:
    virtual ~ILexer() = default;
    virtual Token getNextToken() = 0;
};
