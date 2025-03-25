cmake_minimum_required(VERSION 3.28)
project(Analiz)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(PreprocessorLib
        Preprocessor/GccPreprocessor.cpp
        Preprocessor/GccPreprocessor.h
        Preprocessor/IPreprocessor.h
)
target_include_directories(PreprocessorLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Preprocessor)

add_library(TokenSpecLib
        Lexer/TokenSpecification/TokenSpecReader.cpp
        Lexer/TokenSpecification/TokenSpecReader.h
        Lexer/TokenSpecification/TokenSpec.h
        Lexer/TokenSpecification/ITokenSpecReader.h
)
target_include_directories(TokenSpecLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/TokenSpecification)

add_library(RegexLib
        Lexer/Regex/RegexParser.cpp
        Lexer/Regex/RegexParser.h
        Lexer/Regex/RegexAST.h
        Lexer/Regex/IRegexParser.h
)
target_include_directories(RegexLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Regex)

add_library(NFALib
        Lexer/NFA/NFABuilder.cpp
        Lexer/NFA/NFABuilder.h
        Lexer/NFA/NFA.h
        Lexer/NFA/INFABuilder.h
)
target_include_directories(NFALib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/NFA)

add_library(DFALib
        Lexer/DFA/DFABuilder.cpp
        Lexer/DFA/DFA.h
        Lexer/DFA/IDFABuilder.h
        Lexer/DFA/DFABuiler.h
)
target_include_directories(DFALib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/DFA)

add_library(ReaderLib
        Lexer/Reader/TwoBufferReader.cpp
        Lexer/Reader/TwoBufferReader.h
        Lexer/Reader/IReader.h
)
target_include_directories(ReaderLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Reader)

add_library(SymbolTableLib
        SymbolTable/SymbolTable.cpp
        SymbolTable/SymbolTable.h
        SymbolTable/ISymbolTable.h
)
target_include_directories(SymbolTableLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/SymbolTable)

add_library(DfaLexerLib
        Lexer/DfaLexer.cpp
        Lexer/DfaLexer.h
)
target_include_directories(DfaLexerLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Lexer)

add_library(GrammarReaderLib
        Parser/Reader/GrammarReader.cpp
        Parser/Reader/IGrammarReader.h
)
target_include_directories(GrammarReaderLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Parser/Reader)
target_link_libraries(GrammarReaderLib PRIVATE
        PreprocessorLib TokenSpecLib RegexLib NFALib DFALib ReaderLib SymbolTableLib DfaLexerLib
)

add_library(LR1TableBuilderLib
        Parser/Table/LR1TableBuilder.cpp
        Parser/Table/LR1TableBuilder.h
        Parser/Table/ILR1TableBuilder.h
        Parser/Table/LRTable.h
)
target_include_directories(LR1TableBuilderLib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Parser/Table
        ${CMAKE_CURRENT_SOURCE_DIR}/Parser
)
target_link_libraries(LR1TableBuilderLib PRIVATE GrammarReaderLib)

add_executable(Analiz
        main.cpp
)
target_link_libraries(Analiz PRIVATE GrammarReaderLib LR1TableBuilderLib)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

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

add_executable(TokenSpecReaderTests
        test/Lexer/TokenSpecification/TokenSpecReaderTest.cpp
)
target_link_libraries(TokenSpecReaderTests PRIVATE TokenSpecLib PreprocessorLib gtest_main)
gtest_discover_tests(TokenSpecReaderTests)

add_executable(PreprocessorTests
        test/Preprocessor/PreprocessorTest.cpp
)
target_link_libraries(PreprocessorTests PRIVATE PreprocessorLib gtest_main)
gtest_discover_tests(PreprocessorTests)

add_executable(RegexParserTests
        test/Lexer/Regex/RegexParserTest.cpp
)
target_link_libraries(RegexParserTests PRIVATE RegexLib gtest_main)
gtest_discover_tests(RegexParserTests)

add_executable(NFABuilderTests
        test/Lexer/NFA/NFABuilderTest.cpp
)
target_link_libraries(NFABuilderTests PRIVATE NFALib RegexLib gtest_main)
gtest_discover_tests(NFABuilderTests)

add_executable(DFATests
        test/Lexer/DFA/DFABuilderTest.cpp
)
target_link_libraries(DFATests PRIVATE DFALib NFALib RegexLib gtest_main)
gtest_discover_tests(DFATests)

add_executable(TwoBufferReaderTests
        test/Lexer/Reader/TwoBufferReaderTest.cpp
)
target_link_libraries(TwoBufferReaderTests PRIVATE ReaderLib gtest_main)
gtest_discover_tests(TwoBufferReaderTests)

add_executable(DfaLexerTests
        test/Lexer/DfaLexerTest.cpp
)
target_link_libraries(DfaLexerTests PRIVATE
        PreprocessorLib TokenSpecLib RegexLib NFALib DFALib ReaderLib SymbolTableLib DfaLexerLib gtest_main
)
gtest_discover_tests(DfaLexerTests)

add_executable(GrammarReaderTests
        test/Parser/Reader/GrammarReaderTest.cpp
)
target_link_libraries(GrammarReaderTests PRIVATE GrammarReaderLib gtest_main)
gtest_discover_tests(GrammarReaderTests)

add_executable(LR1TableBuilderTests
        test/Parser/Table/LR1TableBuilderTest.cpp
)
target_link_libraries(LR1TableBuilderTests PRIVATE LR1TableBuilderLib gtest_main)
gtest_discover_tests(LR1TableBuilderTests)
