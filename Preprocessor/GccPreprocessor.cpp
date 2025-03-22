#include "GccPreprocessor.h"

#include <cstdio>        // popen, pclose
#include <stdexcept>     // std::runtime_error
#include <sstream>       // std::ostringstream, std::istringstream
#include <iostream>      // std::cerr

std::string GccPreprocessor::preprocessFile(const std::string& filePath) {
  std::string command = buildCommand(filePath);
  std::string rawOutput = executeCommand(command);
  std::string filtered = filterPreprocessorDirectives(rawOutput);
  return filtered;
}

std::string GccPreprocessor::buildCommand(const std::string& filePath) const {
  return "gcc -E -P " + filePath;
}

std::string GccPreprocessor::executeCommand(const std::string& command) const {
  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    throw std::runtime_error("Не удалось запустить команду: " + command);
  }
  std::ostringstream outputStream;
  const size_t bufferSize = 1024;
  char buffer[bufferSize];
  while (fgets(buffer, bufferSize, pipe) != nullptr) {
    outputStream << buffer;
  }
  int returnCode = pclose(pipe);
  if (returnCode != 0) {
    std::cerr << "[WARN] GccPreprocessor: gcc вернул код " << returnCode << "\n";
    throw std::runtime_error("GCC return code is non-zero: " + std::to_string(returnCode));
  }
  return outputStream.str();
}

std::string GccPreprocessor::filterPreprocessorDirectives(const std::string& input) const {
  std::istringstream iss(input);
  std::ostringstream oss;
  std::string line;
  while (std::getline(iss, line)) {
    if (!line.empty() && line[0] == '#') {
      continue;
    }
    oss << line << "\n";
  }
  return oss.str();
}
