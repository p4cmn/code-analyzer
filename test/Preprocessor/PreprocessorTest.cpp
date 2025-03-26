#include <fstream>
#include <gtest/gtest.h>
#include "../../Preprocessor/GccPreprocessor.h"

static std::string createTempFile(const std::string& content) {
  const char* fileName = "test_temp_input.c";
  std::ofstream ofs(fileName);
  ofs << content;
  ofs.close();
  return fileName;
}

TEST(GccPreprocessorTests, EmptyFile_ReturnsEmptyString) {
  GccPreprocessor preprocessor;

  std::string fileName = createTempFile("");
  std::string result = preprocessor.preprocessFile(fileName);

  EXPECT_TRUE(result.empty());
}

TEST(GccPreprocessorTests, LinesWithHash_Removed) {
  GccPreprocessor preprocessor;

  std::string fileName = createTempFile(
          "#include <stdio.h>\n"
          "int main() {\n"
          "    // Комментарий\n"
          "    return 0;\n"
          "}\n"
  );
  std::string result = preprocessor.preprocessFile(fileName);
  std::istringstream iss(result);
  std::string line;

  while (std::getline(iss, line)) {
    if (!line.empty()) {
      EXPECT_NE('#', line[0]);
    }
  }
}

TEST(GccPreprocessorTests, MacroDefinition_ExpandsCorrectly) {
  GccPreprocessor preprocessor;

  std::string fileName = createTempFile(
          "#define MY_CONST 123\n"
          "MY_CONST\n"
  );

  std::string result = preprocessor.preprocessFile(fileName);

  EXPECT_NE(std::string::npos, result.find("123"));
}

TEST(GccPreprocessorTests, NonExistentFile_ThrowsException) {
  GccPreprocessor preprocessor;

  std::string badFile = "DefinitelyNotExists_12345.c";

  EXPECT_THROW({
                 auto out = preprocessor.preprocessFile(badFile);
               }, std::runtime_error);
}
