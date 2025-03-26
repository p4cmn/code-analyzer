#include "RegexParser.h"

#include <vector>
#include <sstream>
#include <stdexcept>

std::shared_ptr<RegexAST> RegexParser::parse(const std::string& pattern) {
  m_pattern = pattern;
  m_pos = 0;
  auto astRoot = parseImpl();
  if (!eof()) {
    throw std::runtime_error("Неожиданные символы после конца выражения: '" + m_pattern.substr(m_pos) + "'");
  }
  return astRoot;
}

std::shared_ptr<RegexAST> RegexParser::parseImpl() {
  return parseAlt();
}

char RegexParser::peek() const {
  if (m_pos < m_pattern.size()) {
    return m_pattern[m_pos];
  }
  return '\0';
}

char RegexParser::get() {
  if (m_pos < m_pattern.size()) {
    return m_pattern[m_pos++];
  }
  return '\0';
}

bool RegexParser::eof() const {
  return m_pos >= m_pattern.size();
}

bool RegexParser::match(char c) {
  if (peek() == c) {
    get();
    return true;
  }
  return false;
}

std::shared_ptr<RegexAST> RegexParser::makeNode(
        RegexNodeType type,
        std::shared_ptr<RegexAST> left,
        std::shared_ptr<RegexAST> right
) const {
  auto node = std::make_shared<RegexAST>(type);
  node->left = left;
  node->right = right;
  return node;
}

std::shared_ptr<RegexAST> RegexParser::parseAlt() {
  auto left = parseCat();
  while (match('|')) {
    auto right = parseCat();
    left = makeNode(RegexNodeType::Alt, left, right);
  }
  return left;
}

std::shared_ptr<RegexAST> RegexParser::parseCat() {
  std::vector<std::shared_ptr<RegexAST>> nodes;
  nodes.push_back(parseRep());
  for (;;) {
    char c = peek();
    if (c == '|' || c == ')' || c == '\0') {
      break;
    }
    nodes.push_back(parseRep());
  }
  std::shared_ptr<RegexAST> result = nodes.back();
  for (int i = static_cast<int>(nodes.size()) - 2; i >= 0; i--) {
    result = makeNode(RegexNodeType::Concat, nodes[i], result);
  }
  return result;
}

std::shared_ptr<RegexAST> RegexParser::parseRep() {
  auto node = parseBase();
  while (!eof()) {
    char c = peek();
    if (c == '*') {
      get();
      node = makeNode(RegexNodeType::Star, node);
    } else if (c == '+') {
      get();
      node = makeNode(RegexNodeType::Plus, node);
    } else if (c == '?') {
      get();
      node = makeNode(RegexNodeType::Question, node);
    } else {
      break;
    }
  }
  return node;
}

std::shared_ptr<RegexAST> RegexParser::parseBase() {
  char c = peek();
  if (c == '(') {
    get();
    auto node = parseAlt();
    if (!match(')')) {
      throw std::runtime_error("Ожидалась ')' в группе");
    }
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
    if (!((c >= 'a' && c <= 'z') ||
          (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9'))) {
      throw std::runtime_error(std::string("Недопустимый символ '") + c + "' в шаблоне");
    }
    get();
    auto node = makeNode(RegexNodeType::Literal);
    node->literal = c;
    return node;
  }
}

std::string RegexParser::parseCharClass() {
  std::ostringstream oss;
  while (!eof()) {
    char c = peek();
    if (c == ']') {
      get();
      return oss.str();
    }
    if (c == '\\') {
      get();
      if (eof()) {
        throw std::runtime_error("Ожидалась ']' (класс символов не закрыт)");
      }
      char esc = parseEscaped();
      oss << esc;
    } else {
      oss << get();
    }
  }
  throw std::runtime_error("Ожидалась ']' для класса символов, но конец строки");
}

char RegexParser::parseEscaped() {
  if (eof()) {
    throw std::runtime_error("Неожиданный конец при парсинге экранированного символа");
  }
  char c = get();
  switch (c) {
    case 'n':  return '\n';
    case 'r':  return '\r';
    case 't':  return '\t';
    case '\\': return '\\';
    case '|':  return '|';
    case '*':  return '*';
    case '+':  return '+';
    case '?':  return '?';
    case '(':  return '(';
    case ')':  return ')';
    case '[':  return '[';
    case ']':  return ']';
    default:
      return c;
  }
}
