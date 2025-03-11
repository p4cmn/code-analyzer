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
  if (name == "KEYWORD")   return TokenType::KEYWORD;
  if (name == "IDENT")     return TokenType::IDENTIFIER;
  if (name == "NUMBER")    return TokenType::NUMBER;
  if (name == "STRING")    return TokenType::STRING_LITERAL;
  if (name == "LBRACE")    return TokenType::SEPARATOR;    // или сделайте отдельный TokenType для '{'
  if (name == "RBRACE")    return TokenType::SEPARATOR;    // или свой тип
  if (name == "LPAREN")    return TokenType::SEPARATOR;
  if (name == "RPAREN")    return TokenType::SEPARATOR;
  if (name == "SEMICOLON") return TokenType::SEPARATOR;
  if (name == "COMMA")     return TokenType::SEPARATOR;
  if (name == "OP")        return TokenType::OPERATOR;

  // Если правило называется WHITESPACE с ignore=true,
  // мы его пропустим (не вернём наружу), но если дошли сюда –
  // пусть будет UNKNOWN (или можно вернуть SEPARATOR).
  if (name == "WHITESPACE") return TokenType::UNKNOWN;

  // По умолчанию:
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
