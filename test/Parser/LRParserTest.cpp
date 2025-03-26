#include <gtest/gtest.h>
#include <stdexcept>
#include "../../Lexer/ILexer.h"
#include "../../Parser/LRParser.h"
#include "../../Parser/AST/ASTBuilder.h"
#include "../../Parser/Table/LR1TableBuilder.h"

/**
 * @brief Фейковый лексер, возвращающий заданную последовательность токенов.
 */
class FakeLexer : public ILexer {
public:
    explicit FakeLexer(const std::vector<Token>& tokens)
            : m_tokens(tokens), m_index(0)
    {}

    Token getNextToken() override {
      if (m_index < m_tokens.size()) {
        return m_tokens[m_index++];
      }
      // Возвращаем END_OF_FILE, когда всё
      return Token("END_OF_FILE", "", 0, 0);
    }

private:
    std::vector<Token> m_tokens;
    size_t m_index;
};

static Grammar makeCorrectGrammar() {
  // Грамматика:
  // E -> E + T
  // E -> T
  // T -> id
  // Terminals = { "id", "+", "$" }
  // NonTerminals = { "E", "T" }
  // Start = "E"

  Grammar g;
  g.terminals = {"id", "+", "$"};
  g.nonterminals = {"E", "T"};
  g.startSymbol = "E";

  // 0: E : E + T
  {
    Production p;
    p.left = "E";
    p.right = {"E", "+", "T"};
    g.productions.push_back(p);
  }
  // 1: E : T
  {
    Production p;
    p.left = "E";
    p.right = {"T"};
    g.productions.push_back(p);
  }
  // 2: T : id
  {
    Production p;
    p.left = "T";
    p.right = {"id"};
    g.productions.push_back(p);
  }

  return g;
}

///**
// * @brief Тест 1: один id (т. е. "id $").
// */
//TEST(LRParserCorrectGrammarTest, SingleId) {
//  Grammar g = makeCorrectGrammar();
//
//  LR1TableBuilder builder;
//  LRTable table = builder.build(g);
//
//  // Подаём "id $"
//  std::vector<Token> tokens = {
//          Token("id", "x", 1, 1),
//          Token("$",  "",  1, 2)
//  };
//  FakeLexer lexer(tokens);
//
//  ASTBuilder astBuilder;
//  LRParser parser(&lexer, table, g, &astBuilder);
//
//  // Парсим
//  auto root = parser.parse();
//  ASSERT_TRUE(root != nullptr);
//
//  // Корень — E
//  EXPECT_EQ(root->symbol, "E");
//  EXPECT_FALSE(root->isTerminal);
//  // Правило: E -> T, T -> id => дерево E(T(id))
//  ASSERT_EQ(root->children.size(), 1u);
//  auto childE = root->children[0];
//  EXPECT_EQ(childE->symbol, "T");
//  EXPECT_FALSE(childE->isTerminal);
//  ASSERT_EQ(childE->children.size(), 1u);
//  auto idNode = childE->children[0];
//  EXPECT_EQ(idNode->symbol, "id");
//  EXPECT_TRUE(idNode->isTerminal);
//}
//
///**
// * @brief Тест 2: выражение из двух идентификаторов: "id + id $".
// */
//TEST(LRParserCorrectGrammarTest, ExpressionTwoIds) {
//  Grammar g = makeCorrectGrammar();
//
//  LR1TableBuilder builder;
//  LRTable table = builder.build(g);
//
//  // Подаём "id + id $"
//  std::vector<Token> tokens = {
//          Token("id", "x", 1, 1),
//          Token("+",  "+", 1, 2),
//          Token("id", "y", 1, 3),
//          Token("$",  "",  1, 4)
//  };
//  FakeLexer lexer(tokens);
//
//  ASTBuilder astBuilder;
//  LRParser parser(&lexer, table, g, &astBuilder);
//
//  auto root = parser.parse();
//  ASSERT_TRUE(root != nullptr);
//
//  // Корень — E (по правилу E -> E + T)
//  EXPECT_EQ(root->symbol, "E");
//  EXPECT_FALSE(root->isTerminal);
//  // Дети: [0]=E, [1]="+", [2]=T
//  ASSERT_EQ(root->children.size(), 3u);
//
//  // 1) левый E -> T -> id
//  auto leftE = root->children[0];
//  EXPECT_EQ(leftE->symbol, "E");
//  ASSERT_EQ(leftE->children.size(), 1u);
//  auto childT = leftE->children[0];
//  EXPECT_EQ(childT->symbol, "T");
//  ASSERT_EQ(childT->children.size(), 1u);
//  auto idNode1 = childT->children[0];
//  EXPECT_EQ(idNode1->symbol, "id");
//
//  // 2) "+"
//  auto plusNode = root->children[1];
//  EXPECT_EQ(plusNode->symbol, "+");
//  EXPECT_TRUE(plusNode->isTerminal);
//
//  // 3) T -> id
//  auto rightT = root->children[2];
//  EXPECT_EQ(rightT->symbol, "T");
//  EXPECT_FALSE(rightT->isTerminal);
//  ASSERT_EQ(rightT->children.size(), 1u);
//  auto idNode2 = rightT->children[0];
//  EXPECT_EQ(idNode2->symbol, "id");
//  EXPECT_TRUE(idNode2->isTerminal);
//}
//
///**
// * @brief Тест 3: выражение из трёх идентификаторов: "id + id + id $".
// *        Проверяем лево-рекурсивное свёртывание.
// */
//TEST(LRParserCorrectGrammarTest, ExpressionThreeIds) {
//  Grammar g = makeCorrectGrammar();
//
//  LR1TableBuilder builder;
//  LRTable table = builder.build(g);
//
//  // Подаём "id + id + id $"
//  std::vector<Token> tokens = {
//          Token("id", "a", 1, 1),
//          Token("+",  "+", 1, 2),
//          Token("id", "b", 1, 3),
//          Token("+",  "+", 1, 4),
//          Token("id", "c", 1, 5),
//          Token("$",  "",  1, 6)
//  };
//  FakeLexer lexer(tokens);
//
//  ASTBuilder astBuilder;
//  LRParser parser(&lexer, table, g, &astBuilder);
//
//  auto root = parser.parse();
//  ASSERT_TRUE(root != nullptr);
//
//  // Корень — E
//  EXPECT_EQ(root->symbol, "E");
//  // По логике LR(1) будет сначала E -> E + T свёрнуто для первых двух "id",
//  // затем снова E -> E + T с третьим "id".
//
//
//
//  // Проверим лишь, что это E, 3 ребёнка, левый ребёнок сам E, ...
//  ASSERT_EQ(root->children.size(), 3u);
//  auto leftE = root->children[0];
//  auto plusNode = root->children[1];
//  auto rightT = root->children[2];
//
//  // plusNode -> "+"
//  EXPECT_TRUE(plusNode->isTerminal);
//  EXPECT_EQ(plusNode->symbol, "+");
//
//  // rightT -> T -> id
//  ASSERT_EQ(rightT->symbol, "T");
//  ASSERT_EQ(rightT->children.size(), 1u);
//  EXPECT_EQ(rightT->children[0]->symbol, "id");
//
//  // leftE -> E
//  ASSERT_EQ(leftE->symbol, "E");
//  // leftE должно тоже иметь форму E + T
//  ASSERT_EQ(leftE->children.size(), 3u);
//  auto leftE_2 = leftE->children[0];
//  auto plusNode_2 = leftE->children[1];
//  auto rightT_2 = leftE->children[2];
//
//  EXPECT_EQ(plusNode_2->symbol, "+");
//  ASSERT_EQ(rightT_2->symbol, "T");
//  ASSERT_EQ(rightT_2->children.size(), 1u);
//  EXPECT_EQ(rightT_2->children[0]->symbol, "id");
//
//  // leftE_2 -> E -> T -> id
//  ASSERT_EQ(leftE_2->symbol, "E");
//  ASSERT_EQ(leftE_2->children.size(), 1u);
//  auto midT = leftE_2->children[0];
//  EXPECT_EQ(midT->symbol, "T");
//  ASSERT_EQ(midT->children.size(), 1u);
//  auto idNode = midT->children[0];
//  EXPECT_EQ(idNode->symbol, "id");
//}

/**
 * @brief Тест 4: Неверная строка: "id id $".
 *        Ожидаем синтаксическую ошибку.
 */
TEST(LRParserCorrectGrammarTest, SyntaxErrorTest) {
  Grammar g = makeCorrectGrammar();

  LR1TableBuilder builder;
  LRTable table = builder.build(g);

  // Подаём "id id $"
  std::vector<Token> tokens = {
          Token("id", "x", 1, 1),
          Token("id", "y", 1, 2),
          Token("$",  "",  1, 3)
  };
  FakeLexer lexer(tokens);

  ASTBuilder astBuilder;
  LRParser parser(&lexer, table, g, &astBuilder);

  EXPECT_THROW({
                 auto root = parser.parse();
               }, std::runtime_error);
}
