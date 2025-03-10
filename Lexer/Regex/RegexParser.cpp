// RegexParser.cpp
#include "RegexParser.h"
#include <stdexcept>
#include <iostream>

RegexParser::RegexParser(const std::string& pattern)
        : pattern_(pattern), pos_(0) {}

char RegexParser::peek() const {
  if (pos_ < pattern_.size())
    return pattern_[pos_];
  return '\0';
}

char RegexParser::get() {
  if (pos_ < pattern_.size())
    return pattern_[pos_++];
  return '\0';
}

bool RegexParser::eof() const {
  return pos_ >= pattern_.size();
}

bool RegexParser::match(char c) {
  if (peek() == c) {
    get();
    return true;
  }
  return false;
}

std::shared_ptr<RegexAST> RegexParser::makeNode(RegexNodeType type,
                                                std::shared_ptr<RegexAST> left,
                                                std::shared_ptr<RegexAST> right) {
  auto node = std::make_shared<RegexAST>(type);
  node->left = left;
  node->right = right;
  return node;
}

char RegexParser::parseEscaped() {
  if (eof())
    throw std::runtime_error("Неожиданный конец шаблона после символа '\\'");
  char c = get();
  switch(c) {
    case 'n': return '\n';
    case 'r': return '\r';
    case 't': return '\t';
    default: return c;
  }
}

std::string RegexParser::parseCharClass() {
  std::string result;
  while (!eof() && peek() != ']') {
    char c = get();
    if (c == '\\') {
      c = parseEscaped();
    }
    result.push_back(c);
  }
  if (!match(']'))
    throw std::runtime_error("Ожидалась закрывающая ']' для класса символов");
  return result;
}

// Базовое правило: обрабатываем группы, классы символов, escape-последовательности и литералы.
std::shared_ptr<RegexAST> RegexParser::parseBase() {
  char c = peek();
  if (c == '(') {
    get();
    auto node = parseAlt();
    if (!match(')'))
      throw std::runtime_error("Ожидалась ')'");
    return node;
  } else if (c == '[') {
    get();
    std::string cc = parseCharClass();
    auto node = makeNode(RegexNodeType::CharClass);
    node->charClass = cc;
    return node;
  } else if (c == '\\') {
    get();
    char escaped = parseEscaped();
    auto node = makeNode(RegexNodeType::Literal);
    node->literal = escaped;
    return node;
  } else if (c == '|' || c == ')' || c == '*' || c == '+' || c == '?' || c == '\0') {
    return makeNode(RegexNodeType::Epsilon);
  } else {
    c = get();
    auto node = makeNode(RegexNodeType::Literal);
    node->literal = c;
    return node;
  }
}

std::shared_ptr<RegexAST> RegexParser::parseRep() {
  auto node = parseBase();
  while (!eof()) {
    char c = peek();
    if (c == '*' || c == '+' || c == '?') {
      get();
      if (c == '*') {
        node = makeNode(RegexNodeType::Star, node);
      } else if (c == '+') {
        node = makeNode(RegexNodeType::Plus, node);
      } else if (c == '?') {
        node = makeNode(RegexNodeType::Question, node);
      }
    } else {
      break;
    }
  }
  return node;
}

std::shared_ptr<RegexAST> RegexParser::parseCat() {
  auto left = parseRep();
  while (!eof()) {
    char c = peek();
    if (c == '|' || c == ')')
      break;
    auto right = parseRep();
    left = makeNode(RegexNodeType::Concat, left, right);
  }
  return left;
}

std::shared_ptr<RegexAST> RegexParser::parseAlt() {
  auto left = parseCat();
  while (match('|')) {
    auto right = parseCat();
    left = makeNode(RegexNodeType::Alt, left, right);
  }
  return left;
}

std::shared_ptr<RegexAST> RegexParser::parse() {
  auto ast = parseAlt();
  if (!eof())
    throw std::runtime_error("Неожиданные символы в конце шаблона");
  return ast;
}
