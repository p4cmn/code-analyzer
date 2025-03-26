#include <gtest/gtest.h>
#include <fstream>
#include "../../../Parser/Reader/GrammarReader.h"

/**
 * @brief Создаёт временный файл с заданным содержимым и возвращает путь к нему.
 */
static std::string createTempFile(const std::string& content) {
  std::string filename = "temp_grammar_test.txt";
  std::ofstream ofs(filename);
  ofs << content;
  ofs.close();
  return filename;
}

TEST(GrammarReaderTest, BasicGrammarParsing) {
  std::string fileContent = R"(
# Пример грамматики
Terminals:
    + - * / ( ) NUM ID

NonTerminals:
    E T F

Start:
    E

Productions:
    E : E + T
    E : T
    T : T * F
    T : F
    F : ( E )
    F : ID
    F : NUM
)";

  std::string filePath = createTempFile(fileContent);
  GrammarReader reader;
  Grammar grammar = reader.readGrammar(filePath);

  ASSERT_EQ(grammar.terminals.size(), 8u);
  EXPECT_EQ(grammar.terminals[0], "+");
  EXPECT_EQ(grammar.terminals[1], "-");
  EXPECT_EQ(grammar.terminals[2], "*");
  EXPECT_EQ(grammar.terminals[3], "/");
  EXPECT_EQ(grammar.terminals[4], "(");
  EXPECT_EQ(grammar.terminals[5], ")");
  EXPECT_EQ(grammar.terminals[6], "NUM");
  EXPECT_EQ(grammar.terminals[7], "ID");

  ASSERT_EQ(grammar.nonterminals.size(), 3u);
  EXPECT_EQ(grammar.nonterminals[0], "E");
  EXPECT_EQ(grammar.nonterminals[1], "T");
  EXPECT_EQ(grammar.nonterminals[2], "F");

  EXPECT_EQ(grammar.startSymbol, "E");

  ASSERT_EQ(grammar.productions.size(), 7u);

  // 1) E : E + T
  EXPECT_EQ(grammar.productions[0].left, "E");
  ASSERT_EQ(grammar.productions[0].right.size(), 3u);
  EXPECT_EQ(grammar.productions[0].right[0], "E");
  EXPECT_EQ(grammar.productions[0].right[1], "+");
  EXPECT_EQ(grammar.productions[0].right[2], "T");

  // 2) E : T
  EXPECT_EQ(grammar.productions[1].left, "E");
  ASSERT_EQ(grammar.productions[1].right.size(), 1u);
  EXPECT_EQ(grammar.productions[1].right[0], "T");

  // 3) T : T * F
  EXPECT_EQ(grammar.productions[2].left, "T");
  ASSERT_EQ(grammar.productions[2].right.size(), 3u);
  EXPECT_EQ(grammar.productions[2].right[0], "T");
  EXPECT_EQ(grammar.productions[2].right[1], "*");
  EXPECT_EQ(grammar.productions[2].right[2], "F");

  // 4) T : F
  EXPECT_EQ(grammar.productions[3].left, "T");
  ASSERT_EQ(grammar.productions[3].right.size(), 1u);
  EXPECT_EQ(grammar.productions[3].right[0], "F");

  // 5) F : ( E )
  EXPECT_EQ(grammar.productions[4].left, "F");
  ASSERT_EQ(grammar.productions[4].right.size(), 3u);
  EXPECT_EQ(grammar.productions[4].right[0], "(");
  EXPECT_EQ(grammar.productions[4].right[1], "E");
  EXPECT_EQ(grammar.productions[4].right[2], ")");

  // 6) F : ID
  EXPECT_EQ(grammar.productions[5].left, "F");
  ASSERT_EQ(grammar.productions[5].right.size(), 1u);
  EXPECT_EQ(grammar.productions[5].right[0], "ID");

  // 7) F : NUM
  EXPECT_EQ(grammar.productions[6].left, "F");
  ASSERT_EQ(grammar.productions[6].right.size(), 1u);
  EXPECT_EQ(grammar.productions[6].right[0], "NUM");

  std::remove(filePath.c_str());
}

TEST(GrammarReaderTest, MissingSectionsThrowsError) {
  std::string fileContent = R"(
Terminals:
    a b

NonTerminals:
    S

Productions:
    S : a b
)";

  std::string filePath = createTempFile(fileContent);
  GrammarReader reader;

  EXPECT_THROW({
                 Grammar grammar = reader.readGrammar(filePath);
               }, std::runtime_error);

  std::remove(filePath.c_str());
}

TEST(GrammarReaderTest, InvalidProductionThrowsError) {
  std::string fileContent = R"(
Terminals:
    a

NonTerminals:
    S

Start:
    S

Productions:
    S a
)";

  std::string filePath = createTempFile(fileContent);
  GrammarReader reader;

  EXPECT_THROW({
                 Grammar grammar = reader.readGrammar(filePath);
               }, std::runtime_error);

  std::remove(filePath.c_str());
}
