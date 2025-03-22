#include "SymbolTable.h"

SymbolTable::SymbolTable()
        : m_symbolMap()
        , m_nextId(0)
{}

int SymbolTable::addSymbol(const std::string& symbol) {
  auto it = m_symbolMap.find(symbol);
  if (it != m_symbolMap.end()) {
    return it->second;
  }
  int newId = m_nextId++;
  m_symbolMap[symbol] = newId;
  return newId;
}

int SymbolTable::lookup(const std::string& symbol) const {
  auto it = m_symbolMap.find(symbol);
  if (it != m_symbolMap.end()) {
    return it->second;
  }
  return -1;
}
