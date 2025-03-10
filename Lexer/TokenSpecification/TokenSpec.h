#pragma once
#include <string>

struct TokenSpec {
    std::string name;
    std::string regex;
    bool ignore;
    int priority;
};
