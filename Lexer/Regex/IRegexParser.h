#pragma once
#include "RegexAST.h"

#include <memory>
#include <string>

/**
 * @brief Интерфейс парсера регулярных выражений, строящего AST.
 */
class IRegexParser {
public:
    virtual ~IRegexParser() = default;

    /**
     * @brief Парсит указанное регулярное выражение и возвращает корень AST.
     * @param pattern Строка с регулярным выражением.
     * @return Указатель на корневой узел AST.
     * @throws std::runtime_error В случае синтаксической ошибки.
     */
    virtual std::shared_ptr<RegexAST> parse(const std::string& pattern) = 0;
};
