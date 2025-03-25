#pragma once
#include "../AST/ASTNode.h"
#include "../Lexer/ILexer.h"
#include "../Table/LRTable.h"
#include "../AST/IASTBuilder.h"
#include "../Lexer/Token/Token.h"
#include "../../SymbolTable/ISymbolTable.h"

#include <memory>


/**
 * @brief Интерфейс синтаксического анализатора (LR(1)).
 */
class IParser {
public:
    virtual ~IParser() = default;

    /**
     * @brief Запускает процесс синтаксического анализа, возвращая корень AST.
     * @return Корневой узел AST, если анализ прошёл успешно.
     * @throws std::runtime_error При обнаружении синтаксической ошибки или конфликта.
     */
    virtual std::shared_ptr<ASTNode> parse() = 0;
};
