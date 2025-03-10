#pragma once

#include "ILexer.h"
#include "DFA/DFA.h"
#include "../SymbolTable/ISymbolTable.h"
#include "TokenSpecification/TokenSpec.h"
#include "Reader/IReader.h"
#include <vector>
#include <string>


class DfaLexer : public ILexer {
public:
    DfaLexer(const DFA &dfa,
             const std::vector<TokenSpec> &tokenSpecs,
             IReader &reader,
             ISymbolTable *symbolTable);

    Token getNextToken() override;

private:
    TokenType mapTokenNameToType(const std::string &name);

    const DFA &m_dfa;
    const std::vector<TokenSpec> &m_tokenSpecs;
    IReader &m_reader;
    ISymbolTable *m_symbolTable;
};
