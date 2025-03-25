#include "LRParser.h"

#include <stdexcept>
#include <algorithm>


LRParser::LRParser(ILexer* lexer,
                   const LRTable& table,
                   const Grammar& grammar,
                   IASTBuilder* astBuilder)
        : m_lexer(lexer),
          m_table(table),
          m_grammar(grammar),
          m_astBuilder(astBuilder) {
  m_stateStack.push_back(0);
}

std::shared_ptr<ASTNode> LRParser::parse() {
  Token currentToken = getNextToken();
  while (true) {
    int state = m_stateStack.back();
    std::string term = currentToken.type;
    LRAction action = getAction(state, term);
    switch (action.type) {
      case LRActionType::SHIFT: {
        doShift(action.nextState, currentToken);
        currentToken = getNextToken();
        break;
      }
      case LRActionType::REDUCE: {
        doReduce(action.productionIndex);
        break;
      }
      case LRActionType::ACCEPT: {
        if (m_astStack.empty()) {
          throw std::runtime_error("ACCEPT with empty AST stack?");
        }
        return m_astStack.back();
      }
      default: {
        std::string msg = "Syntax error at token '" + currentToken.lexeme
                          + "' (type=" + currentToken.type + "), line="
                          + std::to_string(currentToken.line);
        throw std::runtime_error(msg);
      }
    }
  }
}

Token LRParser::getNextToken()
{
  return m_lexer->getNextToken();
}

void LRParser::doShift(int nextState, const Token& tok) {
  m_stateStack.push_back(nextState);
  auto termNode = m_astBuilder->createTerminal(tok.type);
  m_astStack.push_back(termNode);
}

void LRParser::doReduce(int productionIndex) {
  const auto& prod = m_grammar.productions[productionIndex];
  int rightSize = (int)prod.right.size();
  std::vector<std::shared_ptr<ASTNode>> children;
  children.reserve(rightSize);
  for (int i = 0; i < rightSize; i++) {
    m_stateStack.pop_back();
    children.push_back(m_astStack.back());
    m_astStack.pop_back();
  }
  std::reverse(children.begin(), children.end());
  auto newNode = m_astBuilder->createNode(prod.left, productionIndex, children);
  int topState = m_stateStack.back();
  int nextState = getGoto(topState, prod.left);
  if (nextState < 0) {
    throw std::runtime_error("No GOTO for nonterminal '" + prod.left + "' from state " + std::to_string(topState));
  }
  m_stateStack.push_back(nextState);
  m_astStack.push_back(newNode);
}

LRAction LRParser::getAction(int state, const std::string& terminal) const {
  auto rowIt = m_table.action.find(state);
  if (rowIt == m_table.action.end()) {
    return LRAction(); // ERROR
  }
  const auto& row = rowIt->second;
  auto actIt = row.find(terminal);
  if (actIt == row.end()) {
    return LRAction(); // ERROR
  }
  return actIt->second;
}

int LRParser::getGoto(int state, const std::string& nonTerminal) const {
  auto rowIt = m_table.goTo.find(state);
  if (rowIt == m_table.goTo.end()) {
    return -1;
  }
  const auto& row = rowIt->second;
  auto it = row.find(nonTerminal);
  if (it == row.end()) {
    return -1;
  }
  return it->second;
}
