#include "DfaLexer.h"

DfaLexer::DfaLexer(const DFA &dfa,
                   const std::vector<TokenSpec> &tokenSpecs,
                   IReader &reader,
                   ISymbolTable *symbolTable)
        : m_dfa(dfa),
          m_tokenSpecs(tokenSpecs),
          m_reader(reader),
          m_symbolTable(symbolTable)
{
}

TokenType DfaLexer::mapTokenNameToType(const std::string &name) const
{
  if (name == "KEYWORD")    return TokenType::KEYWORD;
  if (name == "IDENT")      return TokenType::IDENTIFIER;
  if (name == "NUMBER")     return TokenType::NUMBER;
  if (name == "STRING")     return TokenType::STRING_LITERAL;
  if (name == "CHAR")       return TokenType::CHAR_LITERAL;
  if (name == "OP")         return TokenType::OPERATOR;
  if (name == "SEPARATOR")  return TokenType::SEPARATOR;
  if (name == "WHITESPACE") return TokenType::UNKNOWN;
  return TokenType::UNKNOWN;
}

Token DfaLexer::getNextToken()
{
  if (m_reader.isEOF()) {
    Token eofTok;
    eofTok.type = TokenType::END_OF_FILE;
    eofTok.line = m_reader.getLine();
    eofTok.column = m_reader.getColumn();
    return eofTok;
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
      Token eofTok;
      eofTok.type = TokenType::END_OF_FILE;
      eofTok.line = startLine;
      eofTok.column = startCol;
      return eofTok;
    }
    Token unk;
    unk.type = TokenType::UNKNOWN;
    unk.lexeme = std::string(1, bad);
    unk.line = startLine;
    unk.column = startCol;
    return unk;
  }

  const auto &spec = m_tokenSpecs[lastAcceptIndex];
  if (spec.ignore) {
    return getNextToken();
  }

  Token tok;
  tok.type = mapTokenNameToType(spec.name);
  tok.lexeme = lexeme;
  tok.line = startLine;
  tok.column = startCol;
  if (tok.type == TokenType::IDENTIFIER && m_symbolTable) {
    tok.symbolId = m_symbolTable->addSymbol(lexeme);
  }
  return tok;
}
