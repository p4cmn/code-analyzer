#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include "../../../Lexer/Reader/TwoBufferReader.h"

static std::string writeTempFile(const std::string& content, const std::string& fileName) {
  std::ofstream ofs(fileName);
  ofs << content;
  return fileName;
}

TEST(TwoBufferReaderTest, PeekCharDoesNotAdvance) {
  std::string text = "ABCDE";
  std::string path = writeTempFile(text, "test_peek_char.txt");
  TwoBufferReader reader(path, 2);

  char pc0 = reader.peekChar(0);
  EXPECT_EQ(pc0, 'A');
  char g0 = reader.getChar();
  EXPECT_EQ(g0, 'A');

  char pc1 = reader.peekChar(0);
  EXPECT_EQ(pc1, 'B');
  char g1 = reader.getChar();
  EXPECT_EQ(g1, 'B');

  char pc2 = reader.peekChar(0);
  EXPECT_EQ(pc2, 'C');
  char pc2again = reader.peekChar(0);
  EXPECT_EQ(pc2again, 'C');
  char g2 = reader.getChar();
  EXPECT_EQ(g2, 'C');

  char g3 = reader.getChar();
  EXPECT_EQ(g3, 'D');

  char g4 = reader.getChar();
  EXPECT_EQ(g4, 'E');

  char g5 = reader.getChar();
  EXPECT_EQ(g5, '\0');
  EXPECT_TRUE(reader.isEOF());

  std::remove(path.c_str());
}

TEST(TwoBufferReaderTest, CheckLineColumn) {
  std::string text = "abc\nxyz\n123";
  std::string path = writeTempFile(text, "test_line_col.txt");
  TwoBufferReader reader(path, 4);
  EXPECT_EQ(reader.getLine(), 1);
  EXPECT_EQ(reader.getColumn(), 1);

  char c1 = reader.getChar();
  EXPECT_EQ(c1, 'a');
  EXPECT_EQ(reader.getLine(), 1);
  EXPECT_EQ(reader.getColumn(), 2);

  char c2 = reader.getChar();
  EXPECT_EQ(c2, 'b');
  EXPECT_EQ(reader.getLine(), 1);
  EXPECT_EQ(reader.getColumn(), 3);

  char c3 = reader.getChar();
  EXPECT_EQ(c3, 'c');
  EXPECT_EQ(reader.getLine(), 1);
  EXPECT_EQ(reader.getColumn(), 4);

  char c4 = reader.getChar();
  EXPECT_EQ(c4, '\n');
  EXPECT_EQ(reader.getLine(), 2);
  EXPECT_EQ(reader.getColumn(), 1);

  char c5 = reader.getChar();
  EXPECT_EQ(c5, 'x');
  EXPECT_EQ(reader.getLine(), 2);
  EXPECT_EQ(reader.getColumn(), 2);

  char c6 = reader.getChar();
  EXPECT_EQ(c6, 'y');
  EXPECT_EQ(reader.getLine(), 2);
  EXPECT_EQ(reader.getColumn(), 3);

  char c7 = reader.getChar();
  EXPECT_EQ(c7, 'z');
  EXPECT_EQ(reader.getLine(), 2);
  EXPECT_EQ(reader.getColumn(), 4);

  char c8 = reader.getChar();
  EXPECT_EQ(c8, '\n');
  EXPECT_EQ(reader.getLine(), 3);
  EXPECT_EQ(reader.getColumn(), 1);

  char c9 = reader.getChar();
  EXPECT_EQ(c9, '1');
  EXPECT_EQ(reader.getLine(), 3);
  EXPECT_EQ(reader.getColumn(), 2);

  char c10 = reader.getChar();
  EXPECT_EQ(c10, '2');
  EXPECT_EQ(reader.getLine(), 3);
  EXPECT_EQ(reader.getColumn(), 3);

  char c11 = reader.getChar();
  EXPECT_EQ(c11, '3');
  EXPECT_EQ(reader.getLine(), 3);
  EXPECT_EQ(reader.getColumn(), 4);

  char c12 = reader.getChar();
  EXPECT_EQ(c12, '\0');
  EXPECT_TRUE(reader.isEOF());

  std::remove(path.c_str());
}
