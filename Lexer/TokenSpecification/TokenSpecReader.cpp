#include "TokenSpecReader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <iostream>

std::vector<TokenSpec> TokenSpecReader::readTokenSpecs(const std::string& filePath) {
  std::ifstream ifs(filePath);
  if (!ifs.is_open()) {
    throw std::runtime_error("Не удалось открыть файл спецификаций: " + filePath);
  }
  std::vector<TokenSpec> specs;
  std::string line;
  while (std::getline(ifs, line)) {
    line = trim(line);
    if (line.empty() || line[0] == '#') {
      continue;
    }
    TokenSpec ts = parseLine(line);
    specs.push_back(ts);
  }
  return specs;
}

std::string TokenSpecReader::trim(const std::string& s) {
  return rtrim(trimLeft(s));
}

std::string TokenSpecReader::trimLeft(const std::string& s) {
  size_t pos = 0;
  while (pos < s.size() && std::isspace(static_cast<unsigned char>(s[pos]))) {
    ++pos;
  }
  return s.substr(pos);
}

std::string TokenSpecReader::rtrim(const std::string& s) {
  if (s.empty()) return s;
  int i = static_cast<int>(s.size()) - 1;
  while (i >= 0 && std::isspace(static_cast<unsigned char>(s[i]))) {
    --i;
  }
  return s.substr(0, i + 1);
}

std::pair<std::string, std::string> TokenSpecReader::splitOffLastToken(const std::string& s) {
  std::string tmp = rtrim(s);
  if (tmp.empty()) {
    return {"", ""};
  }
  size_t pos = tmp.find_last_of(" \t");
  if (pos == std::string::npos) {
    return {tmp, ""};
  }
  std::string lastToken = tmp.substr(pos + 1);
  std::string remainder = tmp.substr(0, pos);
  remainder = rtrim(remainder);
  return {lastToken, remainder};
}

bool TokenSpecReader::parseIgnoreFlag(const std::string& str, const std::string& wholeLine) const {
  if (str == "true" || str == "True" || str == "1") {
    return true;
  }
  if (str == "false" || str == "False" || str == "0") {
    return false;
  }
  throw std::runtime_error("Некорректное значение флага ignore: " + str +
                           " (строка: " + wholeLine + ")");
}

int TokenSpecReader::parsePriority(const std::string& str, const std::string& wholeLine) const {
  try {
    return std::stoi(str);
  } catch(...) {
    throw std::runtime_error("Некорректный приоритет токена: " + str +
                             " (строка: " + wholeLine + ")");
  }
}

TokenSpec TokenSpecReader::parseLine(const std::string& line) const {
  std::string trimmed = trim(line);
  if (trimmed.empty()) {
    throw std::runtime_error("Пустая строка спецификации: " + line);
  }
  size_t firstSpacePos = trimmed.find_first_of(" \t");
  if (firstSpacePos == std::string::npos) {
    throw std::runtime_error("Не хватает полей в строке спецификации (нет пробелов): " + line);
  }
  std::string tokenName = trimmed.substr(0, firstSpacePos);
  std::string remainder = trimmed.substr(firstSpacePos);
  remainder = trimLeft(remainder);
  auto [priorityStr, remainder2] = splitOffLastToken(remainder);
  if (priorityStr.empty()) {
    throw std::runtime_error("Не найден приоритет токена (нужен как последнее слово): " + line);
  }
  auto [ignoreFlagStr, regexRaw] = splitOffLastToken(remainder2);
  if (ignoreFlagStr.empty()) {
    throw std::runtime_error("Не найден флаг ignore (предпоследнее слово): " + line);
  }
  bool ignoreFlag = parseIgnoreFlag(ignoreFlagStr, line);
  int priority    = parsePriority(priorityStr, line);
  TokenSpec spec;
  spec.name     = tokenName;
  spec.regex    = regexRaw;
  spec.ignore   = ignoreFlag;
  spec.priority = priority;
  return spec;
}
