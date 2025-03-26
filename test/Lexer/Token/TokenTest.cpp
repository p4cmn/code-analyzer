#include "../../../Lexer/Token/Token.h"
#include <gtest/gtest.h>

TEST(TokenTest, DefaultConstructor) {
  Token token;
  EXPECT_EQ(token.type, "UNKNOWN");
  EXPECT_EQ(token.lexeme, "");
  EXPECT_EQ(token.line, 0);
  EXPECT_EQ(token.column, 0);
  EXPECT_EQ(token.symbolId, -1);
}

TEST(TokenTest, ParameterizedConstructor) {
  Token token("IDENT", "var", 1, 5, 10);
  EXPECT_EQ(token.type, "IDENT");
  EXPECT_EQ(token.lexeme, "var");
  EXPECT_EQ(token.line, 1);
  EXPECT_EQ(token.column, 5);
  EXPECT_EQ(token.symbolId, 10);
}
