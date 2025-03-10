#pragma once
#include "ISymbolTable.h"
#include <string>
#include <unordered_map>

class SymbolTable : public ISymbolTable {
private:
    std::unordered_map<std::string, int> table;
    int nextId;

public:
    SymbolTable();
    ~SymbolTable() override = default;

    int addSymbol(const std::string& symbol) override;
    int lookup(const std::string& symbol) const override;
};
