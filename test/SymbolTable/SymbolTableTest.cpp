#include <gtest/gtest.h>
#include "../../SymbolTable/SymbolTable.h"

TEST(SymbolTableTest, AddSymbol_ReturnsConsistentIds) {
  SymbolTable table;

  int id1 = table.addSymbol("alpha");
  int id2 = table.addSymbol("beta");
  int id3 = table.addSymbol("gamma");

  EXPECT_EQ(0, id1);
  EXPECT_EQ(1, id2);
  EXPECT_EQ(2, id3);
}

TEST(SymbolTableTest, AddSymbol_ExistingSymbol_ReturnsSameId) {
  SymbolTable table;

  int id1 = table.addSymbol("alpha");
  int id2 = table.addSymbol("alpha");

  EXPECT_EQ(id1, id2);
}

TEST(SymbolTableTest, Lookup_SymbolExists_ReturnsCorrectId) {
  SymbolTable table;

  int idAlpha = table.addSymbol("alpha");
  int idBeta  = table.addSymbol("beta");

  EXPECT_EQ(idAlpha, table.lookup("alpha"));
  EXPECT_EQ(idBeta,  table.lookup("beta"));
}

TEST(SymbolTableTest, Lookup_SymbolNotExists_ReturnsMinusOne) {
  SymbolTable table;

  table.addSymbol("alpha");

  EXPECT_EQ(-1, table.lookup("beta"));
}

TEST(SymbolTableTest, AddSymbol_EmptyString_ReturnsNewId) {
  SymbolTable table;

  int idEmpty = table.addSymbol("");

  EXPECT_EQ(idEmpty, table.lookup(""));
  EXPECT_NE(-1, idEmpty);
}

TEST(SymbolTableTest, MultipleSymbols_AllDistinct) {
  SymbolTable table;

  int idFoo = table.addSymbol("foo");
  int idBar = table.addSymbol("bar");
  int idBaz = table.addSymbol("baz");

  EXPECT_NE(idFoo, idBar);
  EXPECT_NE(idBar, idBaz);
  EXPECT_NE(idFoo, idBaz);
}
