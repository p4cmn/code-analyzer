#include "DfaLexer.h"
#include <iostream>
#include <cctype>

DfaLexer::DfaLexer(const DFA &dfa,
                   const std::vector<TokenSpec> &tokenSpecs,
                   IReader &reader,
                   ISymbolTable *symbolTable)
        : m_dfa(dfa), m_tokenSpecs(tokenSpecs), m_reader(reader), m_symbolTable(symbolTable)
{}

TokenType DfaLexer::mapTokenNameToType(const std::string &name) {
  // Здесь вы можете расширить логику маппинга,
  // сейчас, для примера, сделаны простые соответствия:
  if (name == "IDENT")       return TokenType::IDENTIFIER;
  if (name == "NUMBER")      return TokenType::NUMBER;
  if (name == "WHITESPACE")  return TokenType::UNKNOWN; // игнорим, но пусть будет UNKNOWN
  if (name == "KEYWORD")     return TokenType::KEYWORD;
  if (name == "OPERATOR")    return TokenType::OPERATOR;
  if (name == "SEPARATOR")   return TokenType::SEPARATOR;
  if (name == "STRING")      return TokenType::STRING_LITERAL;
  // ... и т.д. на ваше усмотрение
  return TokenType::UNKNOWN;
}

// -------------------------------
//  ВАЖНО: Исправленный getNextToken
// -------------------------------
Token DfaLexer::getNextToken() {
  if (m_reader.isEOF()) {
    Token eofTok;
    eofTok.type   = TokenType::END_OF_FILE;
    eofTok.lexeme = "";
    eofTok.line   = m_reader.getLine();
    eofTok.column = m_reader.getColumn();
    return eofTok;
  }
  int startLine   = m_reader.getLine();
  int startColumn = m_reader.getColumn();
  int currentState = m_dfa.startState;
  int lastAcceptState = -1;
  int lastAcceptTokenIndex = -1;
  std::string lexeme;
  while (!m_reader.isEOF()) {
    char c = m_reader.peekChar(0);
    if (m_reader.isEOF()) {
      break;
    }
    int nextState = m_dfa.states[currentState].transitions[(unsigned char)c];
    if (nextState == -1) {
      // Нет перехода — выходим из цикла
      break;
    }
    lexeme.push_back(m_reader.getChar());
    currentState = nextState;
    if (m_dfa.states[currentState].isAccept) {
      lastAcceptState      = currentState;
      lastAcceptTokenIndex = m_dfa.states[currentState].tokenIndex;
    }
  }
  if (lastAcceptState == -1) {
    char badC = m_reader.getChar();
    if ((badC == '\0' && m_reader.isEOF()) || m_reader.isEOF()) {
      Token eofTok;
      eofTok.type   = TokenType::END_OF_FILE;
      eofTok.lexeme = "";
      eofTok.line   = startLine;
      eofTok.column = startColumn;
      return eofTok;
    }
    Token unknownTok;
    unknownTok.type   = TokenType::UNKNOWN;
    unknownTok.lexeme = std::string(1, badC);
    unknownTok.line   = startLine;
    unknownTok.column = startColumn;
    return unknownTok;
  }
  const TokenSpec &spec = m_tokenSpecs[lastAcceptTokenIndex];
  if (spec.ignore) {
    return getNextToken();
  }
  Token tok;
  tok.type   = mapTokenNameToType(spec.name);
  tok.lexeme = lexeme;
  tok.line   = startLine;
  tok.column = startColumn;
  if (tok.type == TokenType::IDENTIFIER && m_symbolTable) {
    tok.symbolId = m_symbolTable->addSymbol(lexeme);
  }

  return tok;
}
