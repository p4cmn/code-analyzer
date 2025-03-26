#include <fstream>
#include <gtest/gtest.h>
#include "../../../Lexer/TokenSpecification/TokenSpecReader.h"

/**
 * @brief Вспомогательный метод для создания временного файла со строками,
 *        чтобы протестировать чтение спецификаций.
 */
static std::string createTempSpecFile(const std::string& content) {
  const char* fileName = "tmp_token_specs.txt";
  std::ofstream ofs(fileName, std::ios::out | std::ios::trunc);
  ofs << content;
  ofs.close();
  return fileName;
}

TEST(TokenSpecReaderTest, ReadTokenSpecs_ValidData_ReturnsAllSpecs) {
  std::string content =
          "# Это комментарий, эта строка должна игнорироваться\n"
          "IDENT [a-zA-Z_][a-zA-Z0-9_]* false 5\n"
          "NUMBER [0-9]+ false 4\n"
          "WHITESPACE [ \\t\\r\\n]+ true 1\n"
          "KEYWORD (auto|break|case) false 10\n"
          "# ещё комментарий\n";

  std::string fileName = createTempSpecFile(content);

  TokenSpecReader reader;
  auto specs = reader.readTokenSpecs(fileName);

  ASSERT_EQ(4u, specs.size());

  EXPECT_EQ("IDENT",     specs[0].name);
  EXPECT_EQ("[a-zA-Z_][a-zA-Z0-9_]*", specs[0].regex);
  EXPECT_FALSE(specs[0].ignore);
  EXPECT_EQ(5, specs[0].priority);

  EXPECT_EQ("NUMBER",    specs[1].name);
  EXPECT_EQ("[0-9]+",    specs[1].regex);
  EXPECT_FALSE(specs[1].ignore);
  EXPECT_EQ(4, specs[1].priority);

  EXPECT_EQ("WHITESPACE", specs[2].name);
  EXPECT_EQ("[ \\t\\r\\n]+", specs[2].regex);
  EXPECT_TRUE(specs[2].ignore);
  EXPECT_EQ(1, specs[2].priority);

  EXPECT_EQ("KEYWORD",   specs[3].name);
  EXPECT_EQ("(auto|break|case)", specs[3].regex);
  EXPECT_FALSE(specs[3].ignore);
  EXPECT_EQ(10, specs[3].priority);
}

TEST(TokenSpecReaderTest, ReadTokenSpecs_EmptyFile_ReturnsEmpty) {
  std::string fileName = createTempSpecFile("");

  TokenSpecReader reader;
  auto specs = reader.readTokenSpecs(fileName);

  EXPECT_TRUE(specs.empty());
}

TEST(TokenSpecReaderTest, ReadTokenSpecs_NoFile_ThrowsException) {
  TokenSpecReader reader;

  EXPECT_THROW({
                 reader.readTokenSpecs("file_that_does_not_exist.txt");
               }, std::runtime_error);
}

TEST(TokenSpecReaderTest, ReadTokenSpecs_InvalidIgnoreFlag_ThrowsException) {
  std::string content = "IDENT [a-zA-Z_]+ maybe 5\n"; // "maybe" некорректно
  std::string fileName = createTempSpecFile(content);

  TokenSpecReader reader;
  EXPECT_THROW({
                 reader.readTokenSpecs(fileName);
               }, std::runtime_error);
}

TEST(TokenSpecReaderTest, ReadTokenSpecs_InvalidPriority_ThrowsException) {
  std::string content = "IDENT [a-zA-Z_]+ false notANumber\n";
  std::string fileName = createTempSpecFile(content);

  TokenSpecReader reader;
  EXPECT_THROW({
                 reader.readTokenSpecs(fileName);
               }, std::runtime_error);
}

TEST(TokenSpecReaderTest, ReadTokenSpecs_IncompleteLine_ThrowsException) {
  std::string content = "IDENT [a-zA-Z_]+ \n";
  std::string fileName = createTempSpecFile(content);

  TokenSpecReader reader;
  EXPECT_THROW({
                 reader.readTokenSpecs(fileName);
               }, std::runtime_error);
}

TEST(TokenSpecReaderTest, ReadTokenSpecs_PartialRegexWithSpaces) {
  std::string content = "IDENT [a-zA-Z   _] false 5\n";
  std::string fileName = createTempSpecFile(content);

  TokenSpecReader reader;
  auto specs = reader.readTokenSpecs(fileName);
  ASSERT_EQ(1u, specs.size());
  EXPECT_EQ("IDENT", specs[0].name);
  EXPECT_EQ("[a-zA-Z   _]", specs[0].regex);
  EXPECT_FALSE(specs[0].ignore);
  EXPECT_EQ(5, specs[0].priority);
}
