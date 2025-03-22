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

# Библиотека для модуля чтения (TwoBufferReader)
add_library(ReaderLib
        Lexer/Reader/TwoBufferReader.cpp
        Lexer/Reader/TwoBufferReader.h
        Lexer/Reader/IReader.h
)

# Библиотека для таблицы символов
add_library(SymbolTableLib
        SymbolTable/SymbolTable.cpp
        SymbolTable/SymbolTable.h
        SymbolTable/ISymbolTable.h
)

# Новая библиотека для DfaLexer
add_library(DfaLexerLib
        Lexer/DfaLexer.cpp
        Lexer/DfaLexer.h
)

# Основное приложение – не содержит тестовых файлов!
add_executable(Analiz main.cpp)
target_link_libraries(Analiz PRIVATE
        PreprocessorLib
        TokenSpecLib
        RegexLib
        NFALib
        DFALib
        ReaderLib
        SymbolTableLib
        DfaLexerLib
)

# Подключаем GoogleTest через FetchContent
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Тесты для Token (файл test/Lexer/Token/TokenTest.cpp)
add_executable(TokenTests
        test/Lexer/Token/TokenTest.cpp
)
target_link_libraries(TokenTests PRIVATE gtest_main)
include(GoogleTest)
gtest_discover_tests(TokenTests)

add_executable(SymbolTableTests
        test/SymbolTable/SymbolTableTest.cpp
)
target_link_libraries(SymbolTableTests PRIVATE SymbolTableLib gtest_main)
gtest_discover_tests(SymbolTableTests)

# Тесты для TokenSpecReader (файл test/Lexer/TokenSpecification/TokenSpecReaderTest.cpp)
add_executable(TokenSpecReaderTests
        test/Lexer/TokenSpecification/TokenSpecReaderTest.cpp
)
target_link_libraries(TokenSpecReaderTests PRIVATE TokenSpecLib PreprocessorLib gtest_main)
gtest_discover_tests(TokenSpecReaderTests)

# Тесты для Preprocessor (файл test/Preprocessor/PreprocessorTest.cpp)
add_executable(PreprocessorTests
        test/Preprocessor/PreprocessorTest.cpp
)
target_link_libraries(PreprocessorTests PRIVATE PreprocessorLib gtest_main)
gtest_discover_tests(PreprocessorTests)

# Тесты для RegexParser (файл test/Lexer/Regex/RegexParserTest.cpp)
add_executable(RegexParserTests
        test/Lexer/Regex/RegexParserTest.cpp
)
target_link_libraries(RegexParserTests PRIVATE RegexLib gtest_main)
gtest_discover_tests(RegexParserTests)

# Тесты для NFABuilder (файл test/Lexer/NFA/NFABuilderTest.cpp)
add_executable(NFABuilderTests
        test/Lexer/NFA/NFABuilderTest.cpp
)
target_link_libraries(NFABuilderTests PRIVATE NFALib RegexLib gtest_main)
gtest_discover_tests(NFABuilderTests)

# Тесты для DFA (файл test/Lexer/DFA/DFATest.cpp)
add_executable(DFATests
        test/Lexer/DFA/DFATest.cpp
)
target_link_libraries(DFATests PRIVATE DFALib NFALib RegexLib gtest_main)
gtest_discover_tests(DFATests)

# Тесты для TwoBufferReader (файл test/Lexer/Reader/TwoBufferReaderTest.cpp)
add_executable(TwoBufferReaderTests
        test/Lexer/Reader/TwoBufferReaderTest.cpp
)
target_link_libraries(TwoBufferReaderTests PRIVATE ReaderLib gtest_main)
gtest_discover_tests(TwoBufferReaderTests)

# Тесты для DfaLexer (файл test/Lexer/DfaLexerTest.cpp)
add_executable(DfaLexerTests
        test/Lexer/DfaLexerTest.cpp
)
target_link_libraries(DfaLexerTests PRIVATE
        PreprocessorLib
        TokenSpecLib
        RegexLib
        NFALib
        DFALib
        ReaderLib
        SymbolTableLib
        DfaLexerLib
        gtest_main
)
gtest_discover_tests(DfaLexerTests)
