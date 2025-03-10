cmake_minimum_required(VERSION 3.28)
project(Analiz)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Библиотека для препроцессора
add_library(PreprocessorLib
        Preprocessor/GccPreprocessor.cpp
        Preprocessor/GccPreprocessor.h
        Preprocessor/IPreprocessor.h
)

# Библиотека для работы со спецификациями токенов
add_library(TokenSpecLib
        Lexer/TokenSpecification/TokenSpecReader.cpp
        Lexer/TokenSpecification/TokenSpecReader.h
        Lexer/TokenSpecification/TokenSpec.h
        Lexer/TokenSpecification/ITokenSpecReader.h
)

# Библиотека для регулярных выражений (Regex)
add_library(RegexLib
        Lexer/Regex/RegexParser.cpp
        Lexer/Regex/RegexParser.h
        Lexer/Regex/RegexAST.h
)

# Библиотека для построителя NFA (NFABuilder)
add_library(NFALib
        Lexer/NFA/NFABuilder.cpp
        Lexer/NFA/NFABuilder.h
)

# Библиотека для конечных автоматов (DFA)
add_library(DFALib
        Lexer/DFA/DFA.cpp
        Lexer/DFA/DFA.h
)

# Основное приложение – не содержит тестовых файлов!
add_executable(Analiz main.cpp)
target_link_libraries(Analiz PRIVATE PreprocessorLib TokenSpecLib RegexLib NFALib DFALib)

# Подключаем GoogleTest через FetchContent
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Тесты для TokenSpecReader
add_executable(TokenSpecReaderTests
        test/Lexer/TokenSpecification/TokenSpecReaderTest.cpp
        test/Lexer/TokenSpecification/TokenSpecReaderTest.h
)
target_link_libraries(TokenSpecReaderTests PRIVATE TokenSpecLib PreprocessorLib gtest_main)
include(GoogleTest)
gtest_discover_tests(TokenSpecReaderTests)

# Тесты для Preprocessor
add_executable(PreprocessorTests
        test/Preprocessor/PreprocessorTest.cpp
        test/Preprocessor/PreprocessorTest.h
)
target_link_libraries(PreprocessorTests PRIVATE PreprocessorLib gtest_main)
gtest_discover_tests(PreprocessorTests)

# Тесты для RegexParser
add_executable(RegexParserTests
        test/Lexer/Regex/RegexParserTest.cpp
)
target_link_libraries(RegexParserTests PRIVATE RegexLib gtest_main)
gtest_discover_tests(RegexParserTests)

# Тесты для NFABuilder
add_executable(NFABuilderTests
        test/Lexer/NFA/NFABuilderTest.cpp
)
target_link_libraries(NFABuilderTests PRIVATE NFALib RegexLib gtest_main)
gtest_discover_tests(NFABuilderTests)

# Тесты для DFA (DFAConstructionTest)
add_executable(DFATests
        test/Lexer/DFA/DFATest.cpp
)
target_link_libraries(DFATests PRIVATE DFALib NFALib RegexLib gtest_main)
gtest_discover_tests(DFATests)
