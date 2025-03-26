#include <gtest/gtest.h>
#include <fstream>
#include "../../Lexer/DFA/DFA.h"
#include "../../Lexer/TokenSpecification/TokenSpec.h"
#include "../../Lexer/Regex/RegexAST.h"
#include "../../Lexer/Regex/RegexParser.h"
#include "../../Lexer/NFA/NFABuilder.h"
#include "../../Lexer/DFA/DFABuiler.h"
#include "../../SymbolTable/SymbolTable.h"
#include "../../Lexer/Reader/TwoBufferReader.h"
#include "../../Lexer/DfaLexer.h"

static DFA buildDFAFromSpecs(const std::vector<TokenSpec> &specs) {
  std::vector<std::shared_ptr<RegexAST>> asts;
  asts.reserve(specs.size());
  std::vector<int> tokenIndexes(specs.size());
  RegexParser parser;
  ThompsonNFABuilder nfaBuilder;
  SubsetConstructionDFABuilder dfaBuilder;
  for (size_t i = 0; i < specs.size(); i++) {
    auto ast = parser.parse(specs[i].regex);
    asts.push_back(ast);
    tokenIndexes[i] = static_cast<int>(i);
  }
  NFA combined = nfaBuilder.buildCombinedNFA(asts, tokenIndexes);
  return dfaBuilder.buildFromNFA(combined);
}

TEST(DfaLexerTest, SimpleIdentifiers) {
  std::vector<TokenSpec> specs = {
          {"IDENT", "[a-zA-Z]+", false, 10},
          {"WHITESPACE", "[ \t\r\n]+", true, 1}
  };
  DFA dfa = buildDFAFromSpecs(specs);
  std::string testInput = "Hello World   Foo123";
  SymbolTable symTable;
  {
    std::string fileName = "tmp_lexer_test.txt";
    std::ofstream ofs(fileName);
    ofs << testInput;
    ofs.close();

    TwoBufferReader reader(fileName, 8);
    DfaLexer lexer(dfa, specs, reader, &symTable);

    Token t1 = lexer.getNextToken();
    EXPECT_EQ(t1.type, "IDENT");
    EXPECT_EQ(t1.lexeme, "Hello");

    Token t2 = lexer.getNextToken();
    EXPECT_EQ(t2.type, "IDENT");
    EXPECT_EQ(t2.lexeme, "World");

    Token t3 = lexer.getNextToken();
    EXPECT_EQ(t3.type, "IDENT");
    EXPECT_EQ(t3.lexeme, "Foo");

    Token t4 = lexer.getNextToken();
    EXPECT_EQ(t4.type, "UNKNOWN");
    EXPECT_EQ(t4.lexeme, "1");

    Token t5 = lexer.getNextToken();
    EXPECT_EQ(t5.type, "UNKNOWN");
    EXPECT_EQ(t5.lexeme, "2");

    Token t6 = lexer.getNextToken();
    EXPECT_EQ(t6.type, "UNKNOWN");
    EXPECT_EQ(t6.lexeme, "3");

    Token eof = lexer.getNextToken();
    EXPECT_EQ(eof.type, "END_OF_FILE");
  }
  std::remove("tmp_lexer_test.txt");
}

TEST(DfaLexerTest, IdentAndNumber) {
  std::vector<TokenSpec> specs = {
          {"IDENT", "[a-zA-Z_][a-zA-Z0-9_]*", false, 10},
          {"NUMBER", "[0-9]+", false, 9},
          {"WHITESPACE", "[ \t\r\n]+", true, 1}
  };
  DFA dfa = buildDFAFromSpecs(specs);
  std::string testInput = "x1 234 __foo 99bar";
  SymbolTable symTable;
  {
    std::string fileName = "tmp_lexer_test2.txt";
    std::ofstream ofs(fileName);
    ofs << testInput;
    ofs.close();

    TwoBufferReader reader(fileName, 8);
    DfaLexer lexer(dfa, specs, reader, &symTable);

    Token t1 = lexer.getNextToken();
    EXPECT_EQ(t1.type, "IDENT");
    EXPECT_EQ(t1.lexeme, "x1");

    Token t2 = lexer.getNextToken();
    EXPECT_EQ(t2.type, "NUMBER");
    EXPECT_EQ(t2.lexeme, "234");

    Token t3 = lexer.getNextToken();
    EXPECT_EQ(t3.type, "IDENT");
    EXPECT_EQ(t3.lexeme, "__foo");

    Token t4 = lexer.getNextToken();
    EXPECT_EQ(t4.type, "NUMBER");
    EXPECT_EQ(t4.lexeme, "99");

    Token t5 = lexer.getNextToken();
    EXPECT_EQ(t5.type, "IDENT");
    EXPECT_EQ(t5.lexeme, "bar");

    Token eof = lexer.getNextToken();
    EXPECT_EQ(eof.type, "END_OF_FILE");
  }
  std::remove("tmp_lexer_test2.txt");
}
