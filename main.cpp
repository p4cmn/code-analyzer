#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <memory>
#include <stdexcept>
#include "Preprocessor/GccPreprocessor.h"
#include "Lexer/TokenSpecification/TokenSpecReader.h"
#include "Lexer/Regex/RegexAST.h"
#include "Lexer/Regex/RegexParser.h"
#include "Lexer/NFA/NFABuilder.h"
#include "Lexer/DFA/DFABuiler.h"
#include "Lexer/Reader/TwoBufferReader.h"
#include "Lexer/DfaLexer.h"
#include "SymbolTable/SymbolTable.h"


static std::string writeTempFile(const std::string &content)
{
  std::string tempFile = "temp_preprocessed.txt";
  std::ofstream ofs(tempFile);
  if (!ofs) {
    throw std::runtime_error("Failed to create temp file: " + tempFile);
  }
  ofs << content;
  ofs.close();
  return tempFile;
}

int main(int argc, char *argv[])
{
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <token_specs.txt> <input_file>\n";
    return 1;
  }
  std::string specsFile = argv[1];
  std::string inputFile = argv[2];

  GccPreprocessor preprocessor;
  std::string preprocessed;
  try {
    preprocessed = preprocessor.preprocessFile(inputFile);
  } catch (const std::exception &e) {
    std::cerr << "Preprocessor error: " << e.what() << std::endl;
    return 1;
  }

  std::string tempFile;
  try {
    tempFile = writeTempFile(preprocessed);
  } catch (const std::exception &e) {
    std::cerr << "Write temp file error: " << e.what() << std::endl;
    return 1;
  }

  TokenSpecReader tsReader;
  std::vector<TokenSpec> specs;
  try {
    specs = tsReader.readTokenSpecs(specsFile);
  } catch (const std::exception &e) {
    std::cerr << "Error reading token specs: " << e.what() << std::endl;
    return 1;
  }

  std::sort(specs.begin(), specs.end(), [](const TokenSpec &a, const TokenSpec &b){
      return a.priority < b.priority;
  });

  std::vector<std::shared_ptr<RegexAST>> asts;
  asts.reserve(specs.size());
  std::vector<int> tokenIndices(specs.size());

  try {
    RegexParser parser;
    for (size_t i = 0; i < specs.size(); i++) {
      auto ast = parser.parse(specs[i].regex);
      asts.push_back(ast);
      tokenIndices[i] = (int)i;
    }
  } catch (const std::exception &e) {
    std::cerr << "Regex parse error: " << e.what() << std::endl;
    return 1;
  }

  ThompsonNFABuilder nfaBuilder;
  NFA combinedNFA;
  try {
    combinedNFA = nfaBuilder.buildCombinedNFA(asts, tokenIndices);
  } catch (const std::exception &e) {
    std::cerr << "Error building combined NFA: " << e.what() << std::endl;
    return 1;
  }

  SubsetConstructionDFABuilder dfaBuilder;
  DFA dfa;
  try {
    dfa = dfaBuilder.buildFromNFA(combinedNFA);
  } catch (const std::exception &e) {
    std::cerr << "Error building DFA: " << e.what() << std::endl;
    return 1;
  }

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
      std::cout << "[UNKNOWN] '" << t.lexeme << "' at line " << t.line
                << ", col " << t.column << std::endl;
    } else {
      std::cout << "[TOKEN " << (int)t.type << "] '"
                << t.lexeme << "' at line " << t.line
                << ", col " << t.column << std::endl;
    }
  }

  if(std::remove(tempFile.c_str()) != 0) {
    std::cerr << "Failed to remove temp file: " << tempFile << std::endl;
  }
  return 0;
}
