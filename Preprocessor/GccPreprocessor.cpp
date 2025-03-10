#include "GccPreprocessor.h"
#include <cstdio>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

std::string GccPreprocessor::preprocessFile(const std::string& filePath) {
  std::string command = "gcc -E -P " + filePath;
  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe) {
    throw std::runtime_error("Не удалось выполнить команду: " + command);
  }
  std::ostringstream outputStream;
  constexpr size_t bufferSize = 1024; //TODO
  char buffer[bufferSize];
  while (fgets(buffer, bufferSize, pipe) != nullptr) {
    outputStream << buffer;
  }
  int returnCode = pclose(pipe);
  if (returnCode != 0) {
    std::cerr << "[WARN] GccPreprocessor: gcc вернул код " << returnCode << "\n";
  }
  std::string result = outputStream.str();
  std::istringstream iss(result);
  std::ostringstream filteredOutput;
  std::string line;
  while (std::getline(iss, line)) {
    if (!line.empty() && line[0] == '#') {
      continue;
    }
    filteredOutput << line << "\n";
  }
  return filteredOutput.str();
}
