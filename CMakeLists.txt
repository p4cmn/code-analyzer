cmake_minimum_required(VERSION 3.28)
project(Analiz)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(PreprocessorLib
        Preprocessor/GccPreprocessor.cpp
        Preprocessor/GccPreprocessor.h
        Preprocessor/IPreprocessor.h
)

add_library(TokenSpecLib
        Lexer/TokenSpecification/TokenSpecReader.cpp
        Lexer/TokenSpecification/TokenSpecReader.h
        Lexer/TokenSpecification/TokenSpec.h
        Lexer/TokenSpecification/ITokenSpecReader.h
)

add_executable(Analiz main.cpp)
target_link_libraries(Analiz PRIVATE PreprocessorLib TokenSpecLib)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(TokenSpecReaderTests
        test/Lexer/TokenSpecification/TokenSpecReaderTest.cpp
        test/Lexer/TokenSpecification/TokenSpecReaderTest.h
)
target_link_libraries(TokenSpecReaderTests PRIVATE TokenSpecLib PreprocessorLib gtest_main)

include(GoogleTest)
gtest_discover_tests(TokenSpecReaderTests)

add_executable(PreprocessorTests
        test/Preprocessor/PreprocessorTest.cpp
        test/Preprocessor/PreprocessorTest.h
)
target_link_libraries(PreprocessorTests PRIVATE PreprocessorLib gtest_main)

gtest_discover_tests(PreprocessorTests)
