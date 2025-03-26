#pragma once
#include "ISymbolTable.h"

#include <string>
#include <unordered_map>

/**
 * @brief Реализация интерфейса ISymbolTable, хранящая символы во внутренней
 *        структуре std::unordered_map<string, int>.
 */
class SymbolTable final : public ISymbolTable {
public:
    SymbolTable();
    ~SymbolTable() override = default;

    int addSymbol(const std::string& symbol) override;
    int lookup(const std::string& symbol) const override;

private:
    std::unordered_map<std::string, int> m_symbolMap;
    int m_nextId;
};
