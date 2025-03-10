#pragma once
#include <string>

class ISymbolTable {
public:
    virtual ~ISymbolTable() = default;
    virtual int addSymbol(const std::string& symbol) = 0;
    virtual int lookup(const std::string& symbol) const = 0;
};
