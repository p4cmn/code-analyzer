#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include "../../../Lexer/Regex/RegexAST.h"
#include "../../../Lexer/Regex/RegexParser.h"


static std::string astToString(const std::shared_ptr<RegexAST> &node) {
  if (!node) return "";
  switch (node->type) {
    case RegexNodeType::Epsilon:
      return "ε";
    case RegexNodeType::Literal: {
      std::string s(1, node->literal);
      return s;
    }
    case RegexNodeType::CharClass: {
      return "[" + node->charClass + "]";
    }
    case RegexNodeType::Concat: {
      std::ostringstream oss;
      oss << "(" << astToString(node->left) << "·" << astToString(node->right) << ")";
      return oss.str();
    }
    case RegexNodeType::Alt: {
      std::ostringstream oss;
      oss << "(" << astToString(node->left) << "|" << astToString(node->right) << ")";
      return oss.str();
    }
    case RegexNodeType::Star: {
      std::ostringstream oss;
      oss << "(" << astToString(node->left) << ")*";
      return oss.str();
    }
    case RegexNodeType::Plus: {
      std::ostringstream oss;
      oss << "(" << astToString(node->left) << ")+";
      return oss.str();
    }
    case RegexNodeType::Question: {
      std::ostringstream oss;
      oss << "(" << astToString(node->left) << ")?";
      return oss.str();
    }
    default:
      return "?";
  }
}

TEST(RegexParserTest, SimpleLiteral) {
  std::unique_ptr<IRegexParser> parser = std::make_unique<RegexParser>();

  auto ast = parser->parse("a");

  EXPECT_EQ(ast->type, RegexNodeType::Literal);
  EXPECT_EQ(ast->literal, 'a');
  EXPECT_EQ(astToString(ast), "a");
}

TEST(RegexParserTest, CharClassBasic) {
  std::unique_ptr<IRegexParser> parser = std::make_unique<RegexParser>();

  auto ast = parser->parse("[ab]");

  ASSERT_EQ(ast->type, RegexNodeType::CharClass);
  EXPECT_EQ(ast->charClass, "ab");
  EXPECT_EQ(astToString(ast), "[ab]");
}

TEST(RegexParserTest, GroupWithStar) {
  std::unique_ptr<IRegexParser> parser = std::make_unique<RegexParser>();

  auto ast = parser->parse("(ab)*");

  ASSERT_EQ(ast->type, RegexNodeType::Star);

  auto concatNode = ast->left;

  ASSERT_NE(concatNode, nullptr);
  EXPECT_EQ(concatNode->type, RegexNodeType::Concat);

  auto leftLit = concatNode->left;
  auto rightLit = concatNode->right;

  ASSERT_NE(leftLit, nullptr);
  ASSERT_NE(rightLit, nullptr);
  EXPECT_EQ(leftLit->type, RegexNodeType::Literal);
  EXPECT_EQ(leftLit->literal, 'a');
  EXPECT_EQ(rightLit->type, RegexNodeType::Literal);
  EXPECT_EQ(rightLit->literal, 'b');
  EXPECT_EQ(astToString(ast), "((a·b))*");
}

TEST(RegexParserTest, AlternationWithPlus) {
  std::unique_ptr<IRegexParser> parser = std::make_unique<RegexParser>();

  auto ast = parser->parse("a|b+");

  ASSERT_EQ(ast->type, RegexNodeType::Alt);

  auto leftNode = ast->left;

  ASSERT_NE(leftNode, nullptr);
  EXPECT_EQ(leftNode->type, RegexNodeType::Literal);
  EXPECT_EQ(leftNode->literal, 'a');

  auto rightNode = ast->right;

  ASSERT_NE(rightNode, nullptr);
  EXPECT_EQ(rightNode->type, RegexNodeType::Plus);

  auto plusSubNode = rightNode->left;

  ASSERT_NE(plusSubNode, nullptr);
  EXPECT_EQ(plusSubNode->type, RegexNodeType::Literal);
  EXPECT_EQ(plusSubNode->literal, 'b');
  EXPECT_EQ(astToString(ast), "(a|(b)+)");
}

TEST(RegexParserTest, EscapeSequence) {
  std::unique_ptr<IRegexParser> parser = std::make_unique<RegexParser>();

  auto ast = parser->parse("\\n");

  ASSERT_EQ(ast->type, RegexNodeType::Literal);
  EXPECT_EQ(ast->literal, '\n');
}

TEST(RegexParserTest, UnclosedGroup) {
  std::unique_ptr<IRegexParser> parser = std::make_unique<RegexParser>();

  EXPECT_THROW(parser->parse("(abc"), std::runtime_error);
}

TEST(RegexParserTest, UnclosedCharClass) {
  std::unique_ptr<IRegexParser> parser = std::make_unique<RegexParser>();

  EXPECT_THROW(parser->parse("[abc"), std::runtime_error);
}

TEST(RegexParserTest, UnexpectedCharactersAfterExpression) {
  std::unique_ptr<IRegexParser> parser = std::make_unique<RegexParser>();

  EXPECT_THROW(parser->parse("ab#xyz"), std::runtime_error);
}

TEST(RegexParserTest, EpsilonCase) {
  std::unique_ptr<IRegexParser> parser = std::make_unique<RegexParser>();

  auto ast = parser->parse("|abc");

  ASSERT_EQ(ast->type, RegexNodeType::Alt);
  ASSERT_NE(ast->left, nullptr);
  EXPECT_EQ(ast->left->type, RegexNodeType::Epsilon);
  ASSERT_NE(ast->right, nullptr);
  EXPECT_EQ(ast->right->type, RegexNodeType::Concat);
  EXPECT_EQ(astToString(ast), "(ε|(a·(b·c)))");
}
