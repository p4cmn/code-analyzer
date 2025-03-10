#include "SymbolTable.h"

SymbolTable::SymbolTable() : nextId(0) {}

int SymbolTable::addSymbol(const std::string& symbol) {
  auto it = table.find(symbol);
  if (it != table.end()) {
    return it->second;
  }
  int id = nextId++;
  table[symbol] = id;
  return id;
}

int SymbolTable::lookup(const std::string& symbol) const {
  auto it = table.find(symbol);
  return (it != table.end()) ? it->second : -1;
}
