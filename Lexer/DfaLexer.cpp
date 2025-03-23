#include "DfaLexer.h"

DfaLexer::DfaLexer(const DFA &dfa,
                   const std::vector<TokenSpec> &tokenSpecs,
                   IReader &reader,
                   ISymbolTable *symbolTable)
        : m_dfa(dfa),
          m_tokenSpecs(tokenSpecs),
          m_reader(reader),
          m_symbolTable(symbolTable)
{}

Token DfaLexer::getNextToken()
{
  if (m_reader.isEOF()) {
    return {"END_OF_FILE", "", m_reader.getLine(), m_reader.getColumn()};
  }

  int startLine = m_reader.getLine();
  int startCol  = m_reader.getColumn();
  int currentState = m_dfa.startState;
  int lastAcceptState = -1;
  int lastAcceptIndex = -1;
  std::string lexeme;

  while (!m_reader.isEOF()) {
    char c = m_reader.peekChar(0);
    if (m_reader.isEOF()) {
      break;
    }
    int nextState = m_dfa.states[currentState].transitions[(unsigned char)c];
    if (nextState == -1) {
      break;
    }
    lexeme.push_back(m_reader.getChar());
    currentState = nextState;
    if (m_dfa.states[currentState].isAccept) {
      lastAcceptState = currentState;
      lastAcceptIndex = m_dfa.states[currentState].tokenIndex;
    }
  }

  if (lastAcceptState == -1) {
    char bad = m_reader.getChar();
    if (bad == '\0' && m_reader.isEOF()) {
      return {"END_OF_FILE", "", startLine, startCol};
    }
    return {"UNKNOWN", std::string(1, bad), startLine, startCol};
  }

  const auto &spec = m_tokenSpecs[lastAcceptIndex];
  if (spec.ignore) {
    return getNextToken();
  }

  Token tok;
  tok.type = spec.name;
  tok.lexeme = lexeme;
  tok.line = startLine;
  tok.column = startCol;
  if (tok.type == "IDENT" && m_symbolTable) {
    tok.symbolId = m_symbolTable->addSymbol(lexeme);
  }
  return tok;
}
