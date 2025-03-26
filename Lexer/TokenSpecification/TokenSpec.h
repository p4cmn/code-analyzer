#pragma once
#include <string>

/**
 * @brief Описание спецификации одного токена: имя, регулярное выражение, флаг игнорирования и приоритет.
 */
struct TokenSpec {
    std::string name;
    std::string regex;
    bool ignore;
    int priority;
};
