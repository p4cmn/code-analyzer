#include <gtest/gtest.h>
#include "../../../Lexer/NFA/NFA.h"
#include "../../../Lexer/Regex/RegexAST.h"
#include "../../../Lexer/NFA/NFABuilder.h"

/**
 * @brief Вспомогательная функция для создания узла AST типа Literal.
 */
static std::shared_ptr<RegexAST> makeLiteralNode(char c) {
  auto node = std::make_shared<RegexAST>(RegexNodeType::Literal);
  node->literal = c;
  return node;
}

/**
 * @brief Создаёт узел для альтернативы: left|right
 */
static std::shared_ptr<RegexAST> makeAltNode(const std::shared_ptr<RegexAST> &left,
                                             const std::shared_ptr<RegexAST> &right) {
  auto node = std::make_shared<RegexAST>(RegexNodeType::Alt);
  node->left = left;
  node->right = right;
  return node;
}

/**
 * @brief Создаёт узел для конкатенации: left right
 */
static std::shared_ptr<RegexAST> makeConcatNode(const std::shared_ptr<RegexAST> &left,
                                                const std::shared_ptr<RegexAST> &right) {
  auto node = std::make_shared<RegexAST>(RegexNodeType::Concat);
  node->left = left;
  node->right = right;
  return node;
}

TEST(NFATest, SingleLiteral) {
  ThompsonNFABuilder builder;
  auto ast = makeLiteralNode('a');

  NFA nfa = builder.buildFromAST(ast);

  ASSERT_EQ(nfa.states.size(), 2u);
  EXPECT_EQ(nfa.startState, 0);
  EXPECT_EQ(nfa.acceptState, 1);

  const auto &startSt = nfa.states[0];
  const auto &acceptSt = nfa.states[1];

  EXPECT_TRUE(acceptSt.isAccept);

  unsigned char c = static_cast<unsigned char>('a');

  ASSERT_EQ(startSt.transitions[c].size(), 1u);
  EXPECT_EQ(startSt.transitions[c][0], 1);

  for (int sym = 0; sym < 256; sym++) {
    if (sym != c) {
      EXPECT_TRUE(startSt.transitions[sym].empty());
    }
  }

  EXPECT_TRUE(startSt.epsilon.empty());
}

TEST(NFATest, Concatenation) {
  ThompsonNFABuilder builder;
  auto ast = makeConcatNode(makeLiteralNode('a'), makeLiteralNode('b'));

  NFA nfa = builder.buildFromAST(ast);

  EXPECT_EQ(nfa.states.size(), 4u);
  EXPECT_EQ(nfa.startState, 0);
  EXPECT_EQ(nfa.acceptState, 3);
  EXPECT_TRUE(nfa.states[3].isAccept);

  unsigned char bChar = static_cast<unsigned char>('b');
  bool foundTransitionB = false;
  for (int stIndex = 0; stIndex < (int)nfa.states.size(); stIndex++) {
    const auto &st = nfa.states[stIndex];
    if (!st.transitions[bChar].empty())
    {
      foundTransitionB = true;
      EXPECT_EQ(st.transitions[bChar][0], 3);
    }
  }
  EXPECT_TRUE(foundTransitionB);
}

TEST(NFATest, Alternative) {
  ThompsonNFABuilder builder;
  auto ast = makeAltNode(makeLiteralNode('a'), makeLiteralNode('b'));
  NFA nfa = builder.buildFromAST(ast);

  ASSERT_GE(nfa.states.size(), 6u);
  EXPECT_EQ(nfa.startState, static_cast<int>(nfa.states.size()) - 2);
  EXPECT_TRUE(nfa.states.back().isAccept);

  const auto &startSt = nfa.states[nfa.startState];

  ASSERT_GE(startSt.epsilon.size(), 2u); // минимум 2 epsilon-перехода
  EXPECT_TRUE(nfa.states[nfa.states.size() - 1].isAccept);
}

TEST(NFATest, Star) {
  ThompsonNFABuilder builder;
  auto lit = makeLiteralNode('a');
  auto ast = std::make_shared<RegexAST>(RegexNodeType::Star);
  ast->left = lit;

  NFA nfa = builder.buildFromAST(ast);

  EXPECT_GE(nfa.states.size(), 4u);

  const auto &startSt = nfa.states[nfa.startState];
  bool hasEpsToAccept = false;

  for (int e : startSt.epsilon) {
    if (e == nfa.acceptState) {
      hasEpsToAccept = true;
      break;
    }

  }
  EXPECT_TRUE(hasEpsToAccept);
}

TEST(NFATest, CombinedNFA) {
  ThompsonNFABuilder builder;
  auto astA = makeLiteralNode('a');
  auto astBStar = std::make_shared<RegexAST>(RegexNodeType::Star);
  astBStar->left = makeLiteralNode('b');
  std::vector<std::shared_ptr<RegexAST>> asts = {astA, astBStar};
  std::vector<int> tokenIndices = {10, 20};

  NFA combined = builder.buildCombinedNFA(asts, tokenIndices);

  EXPECT_GE(combined.states.size(), 7u);
  EXPECT_EQ(combined.startState, 0);
  EXPECT_EQ(combined.acceptState, -1);
  ASSERT_GE(combined.states[0].epsilon.size(), 2u);

  bool foundTokenIndex10 = false;

  bool foundTokenIndex20 = false;
  for (auto &st : combined.states) {
    if (st.isAccept) {
      if (st.tokenIndex == 10) foundTokenIndex10 = true;
      if (st.tokenIndex == 20) foundTokenIndex20 = true;
    }
  }

  EXPECT_TRUE(foundTokenIndex10);
  EXPECT_TRUE(foundTokenIndex20);
}
