#include "GrammarReader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

Grammar GrammarReader::readGrammar(const std::string& filePath) {
  std::ifstream ifs(filePath);
  if (!ifs.is_open()) {
    throw std::runtime_error("Failed to open grammar file: " + filePath);
  }
  Grammar grammar;
  Section currentSection = Section::None;
  std::string line;
  while (std::getline(ifs, line)) {
    line = trim(line);
    if (line.empty() || line[0] == '#') {
      continue;
    }
    if (line == "Terminals:") {
      currentSection = Section::Terminals;
      continue;
    }
    if (line == "NonTerminals:") {
      currentSection = Section::NonTerminals;
      continue;
    }
    if (line == "Start:") {
      currentSection = Section::Start;
      continue;
    }
    if (line == "Productions:") {
      currentSection = Section::Productions;
      continue;
    }
    switch (currentSection) {
      case Section::Terminals:
        parseTerminalsLine(line, grammar);
        break;
      case Section::NonTerminals:
        parseNonTerminalsLine(line, grammar);
        break;
      case Section::Start:
        parseStartLine(line, grammar);
        break;
      case Section::Productions:
        parseProductionLine(line, grammar);
        break;
      default:
        throw std::runtime_error("Unexpected line outside of any section: " + line);
    }
  }
  if (grammar.startSymbol.empty()) {
    throw std::runtime_error("No Start symbol specified in grammar file.");
  }
  if (grammar.productions.empty()) {
    throw std::runtime_error("No productions found in grammar file.");
  }
  return grammar;
}

std::string GrammarReader::trim(const std::string& s) {
  if (s.empty()) return s;
  const char* ws = " \t\r\n";
  size_t start = s.find_first_not_of(ws);
  size_t end   = s.find_last_not_of(ws);
  if (start == std::string::npos) return "";
  return s.substr(start, end - start + 1);
}

void GrammarReader::parseTerminalsLine(const std::string& line, Grammar& grammar) {
  std::istringstream iss(line);
  std::string token;
  while (iss >> token) {
    grammar.terminals.push_back(token);
  }
}

void GrammarReader::parseNonTerminalsLine(const std::string& line, Grammar& grammar) {
  std::istringstream iss(line);
  std::string token;
  while (iss >> token) {
    grammar.nonterminals.push_back(token);
  }
}

void GrammarReader::parseStartLine(const std::string& line, Grammar& grammar) {
  if (line.find(' ') != std::string::npos) {
    throw std::runtime_error("Start line must contain exactly one symbol: " + line);
  }
  grammar.startSymbol = line;
}

void GrammarReader::parseProductionLine(const std::string& line, Grammar& grammar) {
  auto pos = line.find(':');
  if (pos == std::string::npos) {
    throw std::runtime_error("Production must contain ':'. Line: " + line);
  }
  std::string left  = trim(line.substr(0, pos));
  std::string right = trim(line.substr(pos + 1));
  if (left.empty() || right.empty()) {
    throw std::runtime_error("Production line has empty left or right part: " + line);
  }
  Production prod;
  prod.left = left;
  std::istringstream iss(right);
  std::string symbol;
  while (iss >> symbol) {
    prod.right.push_back(symbol);
  }
  grammar.productions.push_back(prod);
}
