#pragma once
#include "ILexer.h"
#include "DFA/DFA.h"
#include "TokenSpecification/TokenSpec.h"
#include "Reader/IReader.h"
#include "../SymbolTable/ISymbolTable.h"

#include <vector>
#include <string>

/**
 * @brief Лексер, работающий по готовому DFA и списку спецификаций токенов.
 */
class DfaLexer : public ILexer {
public:
    /**
     * @param dfa Сконструированный детерминированный автомат
     * @param tokenSpecs Набор спецификаций токенов
     * @param reader Источник символов
     * @param symbolTable Указатель на таблицу символов (может быть nullptr)
     */
    DfaLexer(const DFA &dfa,
             const std::vector<TokenSpec> &tokenSpecs,
             IReader &reader,
             ISymbolTable *symbolTable);

    /**
     * @see ILexer::getNextToken
     */
    Token getNextToken() override;

private:
    const DFA &m_dfa;
    const std::vector<TokenSpec> &m_tokenSpecs;
    IReader &m_reader;
    ISymbolTable *m_symbolTable;
};
