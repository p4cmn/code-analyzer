#include "TokenSpecReader.h"
#include <cctype>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>

static inline std::string trim(const std::string& s) {
  auto start = s.begin();
  while (start != s.end() && std::isspace(*start)) {
    ++start;
  }
  auto end = s.end();
  do {
    --end;
  } while (std::distance(start, end) > 0 && std::isspace(*end));
  return {start, end + 1};
}

std::vector<TokenSpec> TokenSpecReader::readTokenSpecs(const std::string& filePath) {
  std::ifstream ifs(filePath);
  if (!ifs.is_open()) {
    throw std::runtime_error("Не удалось открыть файл спецификаций: " + filePath);
  }
  std::vector<TokenSpec> specs;
  std::string line;
  while (std::getline(ifs, line)) {
    line = trim(line);
    if (line.empty() || line[0] == '#') continue;
    std::istringstream iss(line);
    std::string tokenName;
    iss >> tokenName;
    if (tokenName.empty()) continue;
    std::vector<std::string> parts;
    std::string part;
    while (iss >> part) {
      parts.push_back(part);
    }
    if (parts.size() < 2) {
      std::cerr << "[WARN] Пропускаем некорректную строку: " << line << std::endl;
      continue;
    }
    std::string ignoreFlag = parts[parts.size() - 2];
    int priority = std::stoi(parts.back());
    std::ostringstream regexStream;
    for (size_t i = 0; i < parts.size() - 2; ++i) {
      if (i > 0) {
        regexStream << " ";
      }
      regexStream << parts[i];
    }
    TokenSpec ts;
    ts.name = tokenName;
    ts.regex = regexStream.str();
    ts.ignore = (ignoreFlag == "true" || ignoreFlag == "1" || ignoreFlag == "True");
    ts.priority = priority;
    specs.push_back(ts);
  }
  return specs;
}
