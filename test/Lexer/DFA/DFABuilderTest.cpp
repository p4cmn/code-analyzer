#include <gtest/gtest.h>
#include "../../../Lexer/DFA/DFA.h"
#include "../../../Lexer/NFA/NFA.h"
#include "../../../Lexer/DFA/DFABuiler.h"
#include "../../../Lexer/NFA/NFABuilder.h"


TEST(DFABuilderTest, EmptyNFA) {
  SubsetConstructionDFABuilder dfaBuilder;
  NFA nfa;

  DFA dfa = dfaBuilder.buildFromNFA(nfa);

  ASSERT_EQ(dfa.states.size(), 1u);
  EXPECT_EQ(dfa.startState, 0);
  EXPECT_FALSE(dfa.states[0].isAccept);
}

TEST(DFABuilderTest, SingleLiteralNFA) {
  ThompsonNFABuilder nfaBuilder;
  auto nfaSingle = nfaBuilder.buildBasicNFA('a');
  SubsetConstructionDFABuilder dfaBuilder;

  DFA dfa = dfaBuilder.buildFromNFA(nfaSingle);

  ASSERT_EQ(dfa.states.size(), 2u);
  EXPECT_EQ(dfa.startState, 0);

  unsigned char c = (unsigned char)('a');

  EXPECT_EQ(dfa.states[0].transitions[c], 1);

  for (int sym = 0; sym < 256; sym++) {
    if (sym != c) {
      EXPECT_EQ(dfa.states[0].transitions[sym], -1);
    }
  }

  EXPECT_TRUE(dfa.states[1].isAccept);
}

TEST(DFABuilderTest, CombinedSimple) {
  ThompsonNFABuilder nfaBuilder;
  std::shared_ptr<RegexAST> litA(new RegexAST(RegexNodeType::Literal));
  litA->literal = 'a';
  std::shared_ptr<RegexAST> litB(new RegexAST(RegexNodeType::Literal));
  litB->literal = 'b';
  std::vector<std::shared_ptr<RegexAST>> asts = {litA, litB};
  std::vector<int> tokens = {10, 20};
  NFA combinedNFA = nfaBuilder.buildCombinedNFA(asts, tokens);

  SubsetConstructionDFABuilder dfaBuilder;
  DFA dfa = dfaBuilder.buildFromNFA(combinedNFA);

  ASSERT_LE(dfa.states.size(), 3u);

  int aState = dfa.states[0].transitions[(unsigned char)'a'];
  int bState = dfa.states[0].transitions[(unsigned char)'b'];

  ASSERT_NE(aState, -1);
  ASSERT_NE(bState, -1);
  EXPECT_TRUE(dfa.states[aState].isAccept);
  EXPECT_EQ(dfa.states[aState].tokenIndex, 10);
  EXPECT_TRUE(dfa.states[bState].isAccept);
  EXPECT_EQ(dfa.states[bState].tokenIndex, 20);
}
