#pragma once
#include <memory>
#include <string>
#include "RegexAST.h"

class RegexParser {
public:
    explicit RegexParser(const std::string& pattern);
    std::shared_ptr<RegexAST> parse();

private:
    std::string pattern_;
    size_t pos_;
    char peek() const;
    char get();
    bool eof() const;
    bool match(char c);

    std::shared_ptr<RegexAST> parseAlt();
    std::shared_ptr<RegexAST> parseCat();
    std::shared_ptr<RegexAST> parseRep();
    std::shared_ptr<RegexAST> parseBase();
    std::shared_ptr<RegexAST> makeNode(RegexNodeType type, std::shared_ptr<RegexAST> left = nullptr, std::shared_ptr<RegexAST> right = nullptr);
    std::string parseCharClass();
    char parseEscaped();
};
