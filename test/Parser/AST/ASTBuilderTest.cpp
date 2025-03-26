#include "../../../Parser/AST/ASTBuilder.h"
#include <stdexcept>
#include <gtest/gtest.h>


/**
 * @brief Тест проверяет, что ASTBuilder корректно «склеивает» узлы при имитации свёрток.
 */
TEST(ASTBuilderTest, BuildSimpleExpressionTree) {
  // Имитируем грамматику:
  // (0) E -> E + T
  // (1) E -> T
  // (2) T -> id

  ASTBuilder builder;

  // 1) Создаём терминал "id" (имитируя SHIFT "id")
  auto idNode1 = builder.createTerminal("id"); // это будет T -> id
  // Свёртка: T -> id (прод.2)
  auto Tnode1 = builder.createNode("T", 2, { idNode1 });
  // Свёртка: E -> T (прод.1)
  auto Enode1 = builder.createNode("E", 1, { Tnode1 });

  // 2) Создаём ещё один терминал "id" (для второго T)
  auto idNode2 = builder.createTerminal("id");
  auto Tnode2 = builder.createNode("T", 2, { idNode2 });
  auto Enode2 = builder.createNode("E", 1, { Tnode2 });

  // 3) Создаём терминал "+"
  auto plusNode = builder.createTerminal("+");

  // 4) Теперь свёртка по правилу E -> E + T (прод.0)
  //    Правая часть: (Enode1) + (Tnode2)
  //    Но имитируем порядок: E, "+", T
  auto bigE = builder.createNode("E", 0, { Enode1, plusNode, Enode2 });

  // Проверяем структуру получившегося дерева:
  // bigE = E
  //   children:
  //     [0] -> E (child of Enode1)
  //        children:
  //           [0] -> T
  //               children:
  //                  [0] -> id (terminal)
  //     [1] -> + (terminal)
  //     [2] -> E (child of Enode2)
  //        children:
  //           [0] -> T
  //               children:
  //                  [0] -> id (terminal)

  ASSERT_EQ(bigE->symbol, "E");
  ASSERT_FALSE(bigE->isTerminal);
  ASSERT_EQ(bigE->children.size(), 3u);

  // Первый дочерний узел — E
  auto leftE = bigE->children[0];
  ASSERT_EQ(leftE->symbol, "E");
  ASSERT_FALSE(leftE->isTerminal);
  ASSERT_EQ(leftE->children.size(), 1u);
  auto leftT = leftE->children[0];
  ASSERT_EQ(leftT->symbol, "T");
  ASSERT_FALSE(leftT->isTerminal);
  ASSERT_EQ(leftT->children.size(), 1u);
  auto id1 = leftT->children[0];
  ASSERT_EQ(id1->symbol, "id");
  ASSERT_TRUE(id1->isTerminal);

  // Второй дочерний узел — "+"
  auto plus = bigE->children[1];
  ASSERT_EQ(plus->symbol, "+");
  ASSERT_TRUE(plus->isTerminal);

  // Третий дочерний узел — E
  auto rightE = bigE->children[2];
  ASSERT_EQ(rightE->symbol, "E");
  ASSERT_FALSE(rightE->isTerminal);
  ASSERT_EQ(rightE->children.size(), 1u);
  auto rightT = rightE->children[0];
  ASSERT_EQ(rightT->symbol, "T");
  ASSERT_FALSE(rightT->isTerminal);
  ASSERT_EQ(rightT->children.size(), 1u);
  auto id2 = rightT->children[0];
  ASSERT_EQ(id2->symbol, "id");
  ASSERT_TRUE(id2->isTerminal);
}

/**
 * @brief Тест проверяет, что мы можем строить «простой» AST с одним узлом.
 */
TEST(ASTBuilderTest, SingleNode) {
  ASTBuilder builder;
  auto terminal = builder.createTerminal("id");
  EXPECT_EQ(terminal->symbol, "id");
  EXPECT_TRUE(terminal->isTerminal);
  EXPECT_TRUE(terminal->children.empty());

  auto nonTerm = builder.createNode("F", 999, { terminal });
  EXPECT_EQ(nonTerm->symbol, "F");
  EXPECT_FALSE(nonTerm->isTerminal);
  ASSERT_EQ(nonTerm->children.size(), 1u);
  EXPECT_EQ(nonTerm->children[0]->symbol, "id");
  EXPECT_TRUE(nonTerm->children[0]->isTerminal);
}

/**
 * @brief Тест «глубокого» дерева: имитируем F -> ( E ), E -> E + T, T -> id, и т.д.
 */
TEST(ASTBuilderTest, NestedTreeTest) {
  ASTBuilder builder;

  // (1) Создаём "id" => T -> id
  auto idNode = builder.createTerminal("id");
  auto Tnode = builder.createNode("T", 2, { idNode });

  // (2) E -> T
  auto Enode = builder.createNode("E", 1, { Tnode });

  // (3) "(" и ")" как терминалы
  auto leftParen = builder.createTerminal("(");
  auto rightParen = builder.createTerminal(")");

  // (4) F -> "(" E ")"
  auto Fnode = builder.createNode("F", 10, { leftParen, Enode, rightParen });

  // Проверяем
  ASSERT_EQ(Fnode->symbol, "F");
  ASSERT_FALSE(Fnode->isTerminal);
  ASSERT_EQ(Fnode->children.size(), 3u);

  EXPECT_EQ(Fnode->children[0]->symbol, "(");
  EXPECT_TRUE(Fnode->children[0]->isTerminal);

  auto midE = Fnode->children[1];
  ASSERT_EQ(midE->symbol, "E");
  ASSERT_FALSE(midE->isTerminal);
  ASSERT_EQ(midE->children.size(), 1u);

  auto midT = midE->children[0];
  ASSERT_EQ(midT->symbol, "T");
  ASSERT_FALSE(midT->isTerminal);
  ASSERT_EQ(midT->children.size(), 1u);

  auto midId = midT->children[0];
  ASSERT_EQ(midId->symbol, "id");
  ASSERT_TRUE(midId->isTerminal);

  EXPECT_EQ(Fnode->children[2]->symbol, ")");
  EXPECT_TRUE(Fnode->children[2]->isTerminal);
}
