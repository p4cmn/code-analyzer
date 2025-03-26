#include "../../../Parser/Grammar/Grammar.h"
#include "../../../Parser/Table/LR1TableBuilder.h"
#include <gtest/gtest.h>
#include <stdexcept>

// Тест 1: Простая грамматика – проверяем, что для символа '$' устанавливается действие ACCEPT
TEST(LR1TableBuilderTest, SimpleGrammar) {
  // Грамматика:
  // Terminals: {"id", "+", "$"}
  // NonTerminals: {"E"}
  // StartSymbol: "E"
  // Productions:
  //    E -> E + id
  //    E -> id
  Grammar g;
  g.terminals = {"id", "+", "$"};
  g.nonterminals = {"E"};
  g.startSymbol = "E";

  Production prod1;
  prod1.left = "E";
  prod1.right = {"E", "+", "id"};
  g.productions.push_back(prod1);

  Production prod2;
  prod2.left = "E";
  prod2.right = {"id"};
  g.productions.push_back(prod2);

  LR1TableBuilder builder;
  LRTable table;
  EXPECT_NO_THROW({
                    table = builder.build(g);
                  });

  bool acceptFound = false;
  for (const auto &row : table.action) {
    auto it = row.second.find("$");
    if (it != row.second.end() && it->second.type == LRActionType::ACCEPT) {
      acceptFound = true;
      break;
    }
  }
  EXPECT_TRUE(acceptFound) << "No ACCEPT action found in the LR(1) table for '$'.";
}

// Тест 2: Неверный стартовый символ
TEST(LR1TableBuilderTest, InvalidStartSymbolTest) {
  Grammar g;
  g.terminals = {"id", "+", "$"};
  g.nonterminals = {"E"};
  g.startSymbol = "X";

  Production prod;
  prod.left = "E";
  prod.right = {"id"};
  g.productions.push_back(prod);

  LR1TableBuilder builder;
  EXPECT_THROW({
                 builder.build(g);
               }, std::runtime_error);
}

// Тест 3: Неверная левая часть продукции
TEST(LR1TableBuilderTest, InvalidProductionLeftTest) {
  Grammar g;
  g.terminals = {"id", "+", "$"};
  g.nonterminals = {"E"};
  g.startSymbol = "E";

  Production prod;
  prod.left = "F"; // F не объявлен
  prod.right = {"id"};
  g.productions.push_back(prod);

  LR1TableBuilder builder;
  EXPECT_THROW({
                 builder.build(g);
               }, std::runtime_error);
}

// Тест 4: Неверная правая часть продукции
TEST(LR1TableBuilderTest, InvalidProductionRightTest) {
  Grammar g;
  g.terminals = {"id", "+", "$"};
  g.nonterminals = {"E"};
  g.startSymbol = "E";

  Production prod;
  prod.left = "E";
  prod.right = {"id", "*", "id"}; // "*" не объявлен
  g.productions.push_back(prod);

  LR1TableBuilder builder;
  EXPECT_THROW({
                 builder.build(g);
               }, std::runtime_error);
}

// Тест 5: Проверка наличия хотя бы одного SHIFT действия
TEST(LR1TableBuilderTest, ShiftActionTest) {
  Grammar g;
  g.terminals = {"id", "+", "$"};
  g.nonterminals = {"E"};
  g.startSymbol = "E";

  Production prod1;
  prod1.left = "E";
  prod1.right = {"E", "+", "id"};
  g.productions.push_back(prod1);

  Production prod2;
  prod2.left = "E";
  prod2.right = {"id"};
  g.productions.push_back(prod2);

  LR1TableBuilder builder;
  LRTable table;
  EXPECT_NO_THROW({
                    table = builder.build(g);
                  });

  bool shiftFound = false;
  for (const auto &row : table.action) {
    for (const auto &actionPair : row.second) {
      if (actionPair.second.type == LRActionType::SHIFT) {
        shiftFound = true;
        break;
      }
    }
    if (shiftFound)
      break;
  }
  EXPECT_TRUE(shiftFound) << "No SHIFT action found in the LR(1) table.";
}

// Тест 6: Проверка наличия GOTO переходов
TEST(LR1TableBuilderTest, GoToTransitionTest) {
  Grammar g;
  g.terminals = {"id", "+", "$"};
  g.nonterminals = {"E"};
  g.startSymbol = "E";

  Production prod1;
  prod1.left = "E";
  prod1.right = {"E", "+", "id"};
  g.productions.push_back(prod1);

  Production prod2;
  prod2.left = "E";
  prod2.right = {"id"};
  g.productions.push_back(prod2);

  LR1TableBuilder builder;
  LRTable table;
  EXPECT_NO_THROW({
                    table = builder.build(g);
                  });

  bool gotoFound = false;
  for (const auto &row : table.goTo) {
    if (!row.second.empty()) {
      gotoFound = true;
      break;
    }
  }
  EXPECT_TRUE(gotoFound) << "No GOTO transitions found in the LR(1) table.";
}

// Тест 7: Проверка обнаружения конфликта
TEST(LR1TableBuilderTest, ConflictTest) {
  Grammar g;
  g.terminals = {"id", "+", "$"};
  g.nonterminals = {"E"};
  g.startSymbol = "E";

  Production prod1;
  prod1.left = "E";
  prod1.right = {"E", "+", "id"};
  g.productions.push_back(prod1);

  Production prod2;
  prod2.left = "E";
  prod2.right = {"E", "+", "id"};
  g.productions.push_back(prod2);

  Production prod3;
  prod3.left = "E";
  prod3.right = {"id"};
  g.productions.push_back(prod3);

  LR1TableBuilder builder;
  EXPECT_THROW({
                 builder.build(g);
               }, std::runtime_error);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
