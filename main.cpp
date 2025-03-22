#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <memory>
#include <cstdio>
#include "Lexer/DfaLexer.h"
#include "Lexer/Regex/RegexParser.h"
#include "Lexer/NFA/NFABuilder.h"
#include "Lexer/DFA/DFA.h"
#include "Lexer/TokenSpecification/TokenSpecReader.h"
#include "Lexer/Reader/TwoBufferReader.h"
#include "SymbolTable/SymbolTable.h"
#include "Lexer/Token/Token.h"
#include "Preprocessor/GccPreprocessor.h"

std::string writeTempFile(const std::string &content) {
  std::string tempFile = "temp_preprocessed.txt";
  std::ofstream ofs(tempFile);
  if (!ofs) {
    throw std::runtime_error("Не удалось создать временный файл: " + tempFile);
  }
  ofs << content;
  ofs.close();
  return tempFile;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <token_specs.txt> <input_file>\n";
    return 1;
  }
  std::string specsFile = argv[1];
  std::string inputFile = argv[2];
  GccPreprocessor preprocessor;
  std::string preprocessedContent;
  try {
    preprocessedContent = preprocessor.preprocessFile(inputFile);
  } catch (const std::exception &e) {
    std::cerr << "Ошибка препроцессора: " << e.what() << std::endl;
    return 1;
  }
  std::string tempFile;
  try {
    tempFile = writeTempFile(preprocessedContent);
  } catch (const std::exception &e) {
    std::cerr << "Ошибка записи временного файла: " << e.what() << std::endl;
    return 1;
  }
  TokenSpecReader tsReader;
  auto specs = tsReader.readTokenSpecs(specsFile);
  std::sort(specs.begin(), specs.end(),
            [](const TokenSpec &a, const TokenSpec &b) {
                return a.priority < b.priority;
            });
  std::vector<std::shared_ptr<RegexAST>> asts;
  asts.reserve(specs.size());
  std::vector<int> tokenIndexes(specs.size());
  for (size_t i = 0; i < specs.size(); i++) {
    RegexParser parser(specs[i].regex);
    auto ast = parser.parse();
    asts.push_back(ast);
    tokenIndexes[i] = (int)i;
  }
  NFA combined = NFABuilder::buildCombinedNFA(asts, tokenIndexes);
  DFA dfa = subsetConstruction(combined);
  TwoBufferReader reader(tempFile);
  SymbolTable symTable;
  DfaLexer lexer(dfa, specs, reader, &symTable);
  while (true) {
    Token t = lexer.getNextToken();
    if (t.type == TokenType::END_OF_FILE) {
      std::cout << "EOF" << std::endl;
      break;
    }
    if (t.type == TokenType::UNKNOWN) {
      std::cout << "UNKNOWN(" << t.lexeme << ") at line "
                << t.line << ", col " << t.column << std::endl;
    } else {
      std::cout << "TOKEN(" << (int)t.type << ")='"
                << t.lexeme << "' at line " << t.line
                << ", col " << t.column << std::endl;
    }
  }
  if(std::remove(tempFile.c_str()) != 0) {
    std::cerr << "Не удалось удалить временный файл: " << tempFile << std::endl;
  }

  return 0;
}
