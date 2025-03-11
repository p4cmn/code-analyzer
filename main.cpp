#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include "Lexer/DfaLexer.h"
#include "Lexer/Regex/RegexParser.h"
#include "Lexer/NFA/NFABuilder.h"
#include "Lexer/DFA/DFA.h"
#include "Lexer/TokenSpecification/TokenSpecReader.h"
#include "Lexer/Reader/TwoBufferReader.h"
#include "SymbolTable/SymbolTable.h"
#include "Lexer/Token/Token.h"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <token_specs.txt> <input_file>\n";
    return 1;
  }

  std::string specsFile = argv[1];
  std::string inputFile = argv[2];
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
  TwoBufferReader reader(inputFile);
  SymbolTable symTable;
  DfaLexer lexer(dfa, specs, reader, &symTable);
  while (true) {
    Token t = lexer.getNextToken();
    if (t.type == TokenType::END_OF_FILE) {
      std::cout << "EOF" << std::endl;
      break;
    }
    if (t.type == TokenType::UNKNOWN) {
      std::cout << "UNKNOWN(" << t.lexeme << ")"
                << " at line " << t.line << ", col " << t.column
                << std::endl;
    } else {
      std::cout << "TOKEN(" << (int)t.type << ")='"
                << t.lexeme << "' "
                << " at line " << t.line << ", col " << t.column
                << std::endl;
    }
  }
  return 0;
}
