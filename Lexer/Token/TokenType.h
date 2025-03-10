#pragma once

enum class TokenType {
    IDENTIFIER,
    NUMBER,
    KEYWORD,
    OPERATOR,
    SEPARATOR,
    STRING_LITERAL,
    CHAR_LITERAL,
    END_OF_FILE,
    UNKNOWN
};
