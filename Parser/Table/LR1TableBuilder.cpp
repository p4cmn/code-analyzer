#include "LR1TableBuilder.h"
#include <stdexcept>
#include <queue>
#include <algorithm>
#include <sstream>

LRTable LR1TableBuilder::build(const Grammar &grammar) {
  // Очистка внутренних структур.
  m_states.clear();
  m_stateIndex.clear();
  m_table.action.clear();
  m_table.goTo.clear();
  m_firstNT.clear();
  validateGrammarSymbols(grammar);
  buildAugmentedGrammar(grammar);
  computeAllFirstSets();
  LR1State startState;
  {
    LR1Core core;
    core.left = AUGMENTED_START;
    core.right = { m_augGrammar.startSymbol };
    core.dotPos = 0;
    LR1Item item { core, END_OF_INPUT };
    addToState(startState, item);
  }
  closure(startState);
  bool isNew = false;
  int startId = getStateId(startState, isNew);
  std::queue<int> Q;
  Q.push(startId);
  while (!Q.empty()) {
    int stId = Q.front();
    Q.pop();
    LR1State currentState = m_states[stId];
    std::unordered_set<std::string> symbolsAfterDot;
    for (const auto &item : currentState) {
      if (item.core.dotPos < static_cast<int>(item.core.right.size()))
        symbolsAfterDot.insert(item.core.right[item.core.dotPos]);
    }
    for (const auto &X : symbolsAfterDot) {
      LR1State toState = gotoState(currentState, X);
      if (!toState.empty()) {
        closure(toState);
        bool wasNew = false;
        int id2 = getStateId(toState, wasNew);
        if (isTerminal(X)) {
          auto &row = m_table.action[stId];
          if (row.find(X) != row.end() && row[X].type != LRActionType::ERROR) {
            throw std::runtime_error("SHIFT/REDUCE or SHIFT/SHIFT conflict on symbol: " + X);
          }
          LRAction a;
          a.type = LRActionType::SHIFT;
          a.nextState = id2;
          row[X] = a;
        } else {
          m_table.goTo[stId][X] = id2;
        }
        if (wasNew) {
          Q.push(id2);
        }
      }
    }
    for (const auto &item : currentState) {
      if (item.core.dotPos == static_cast<int>(item.core.right.size())) {
        auto &row = m_table.action[stId];
        if (item.core.left == AUGMENTED_START) {
          LRAction acc;
          acc.type = LRActionType::ACCEPT;
          row[item.lookahead] = acc;
        } else {
          int prodIndex = -1;
          for (int i = 0; i < static_cast<int>(m_augGrammar.productions.size()); i++) {
            const auto &p = m_augGrammar.productions[i];
            if (p.left == item.core.left && p.right == item.core.right) {
              prodIndex = i;
              break;
            }
          }
          if (prodIndex < 0)
            throw std::runtime_error("Cannot find production for reduce: " + item.core.left);
          LRAction r;
          r.type = LRActionType::REDUCE;
          r.productionIndex = prodIndex;
          if (row.find(item.lookahead) == row.end() || row[item.lookahead].type == LRActionType::ERROR) {
            row[item.lookahead] = r;
          }
        }
      }
    }
  }
  return m_table;
}

void LR1TableBuilder::buildAugmentedGrammar(const Grammar &original) {
  m_augGrammar = original;
  Production aug;
  aug.left = AUGMENTED_START;
  aug.right.push_back(original.startSymbol);
  m_augGrammar.productions.insert(m_augGrammar.productions.begin(), aug);
  m_augGrammar.nonterminals.insert(m_augGrammar.nonterminals.begin(), AUGMENTED_START);
}

void LR1TableBuilder::validateGrammarSymbols(const Grammar &g) {
  std::unordered_set<std::string> all;
  for (const auto &t : g.terminals)
    all.insert(t);
  for (const auto &nt : g.nonterminals)
    all.insert(nt);
  if (all.find(g.startSymbol) == all.end())
    throw std::runtime_error("StartSymbol '" + g.startSymbol + "' not found in terminals or nonterminals");
  std::unordered_set<std::string> prodSet;
  for (const auto &prod : g.productions) {
    if (all.find(prod.left) == all.end())
      throw std::runtime_error("Left side of rule '" + prod.left + "' is not in T∪NT");
    for (const auto &sym : prod.right) {
      if (all.find(sym) == all.end())
        throw std::runtime_error("Symbol '" + sym + "' in right side is not in T∪NT");
    }
    std::ostringstream oss;
    oss << prod.left << "->";
    for (const auto &s : prod.right)
      oss << s << " ";
    std::string key = oss.str();
    if (prodSet.find(key) != prodSet.end())
      throw std::runtime_error("Duplicate production: " + key);
    prodSet.insert(key);
  }
}

void LR1TableBuilder::computeAllFirstSets() {
  m_firstNT.clear();
  for (const auto &nt : m_augGrammar.nonterminals)
    m_firstNT[nt] = {};
  bool changed = true;
  while (changed) {
    changed = false;
    for (const auto &prod : m_augGrammar.productions) {
      const std::string &A = prod.left;
      auto &firstA = m_firstNT[A];
      bool allEps = true;
      for (const auto &sym : prod.right) {
        if (isTerminal(sym)) {
          size_t oldSize = firstA.size();
          firstA.insert(sym);
          if (firstA.size() > oldSize)
            changed = true;
          allEps = false;
          break;
        } else if (isNonTerminal(sym)) {
          size_t oldSize = firstA.size();
          const auto &fstB = m_firstNT[sym];
          bool hasEps = (fstB.find("") != fstB.end());
          for (const auto &x : fstB)
            if (!x.empty())
              firstA.insert(x);
          if (firstA.size() > oldSize)
            changed = true;
          if (!hasEps) {
            allEps = false;
            break;
          }
        } else {
          allEps = false;
          break;
        }
      }
      if (allEps) {
        size_t oldSize = firstA.size();
        firstA.insert("");
        if (firstA.size() > oldSize)
          changed = true;
      }
    }
  }
}

std::set<std::string> LR1TableBuilder::firstOfSequence(const std::vector<std::string> &symbols) const {
  std::set<std::string> result;
  if (symbols.empty()) {
    result.insert("");
    return result;
  }
  bool allEps = true;
  for (const auto &sym : symbols) {
    if (isTerminal(sym)) {
      result.insert(sym);
      allEps = false;
      break;
    } else if (isNonTerminal(sym)) {
      const auto &fstB = m_firstNT.at(sym);
      bool hasEps = (fstB.find("") != fstB.end());
      for (const auto &x : fstB)
        if (!x.empty())
          result.insert(x);
      if (!hasEps) {
        allEps = false;
        break;
      }
    } else {
      allEps = false;
      break;
    }
  }
  if (allEps)
    result.insert("");
  return result;
}

void LR1TableBuilder::closure(LR1State &st) {
  std::queue<LR1Item> worklist;
  for (const auto &item : st)
    worklist.push(item);
  while (!worklist.empty()) {
    LR1Item current = worklist.front();
    worklist.pop();
    if (current.core.dotPos < static_cast<int>(current.core.right.size())) {
      std::string B = current.core.right[current.core.dotPos];
      if (isNonTerminal(B)) {
        std::vector<std::string> beta;
        for (int i = current.core.dotPos + 1; i < static_cast<int>(current.core.right.size()); i++)
          beta.push_back(current.core.right[i]);
        auto fstBeta = firstOfSequence(beta);
        std::unordered_set<std::string> newLookaheads;
        bool hasEps = (fstBeta.find("") != fstBeta.end());
        for (const auto &x : fstBeta)
          if (!x.empty())
            newLookaheads.insert(x);
        if (hasEps)
          newLookaheads.insert(current.lookahead);
        for (const auto &p : m_augGrammar.productions) {
          if (p.left == B) {
            LR1Core newCore;
            newCore.left = B;
            newCore.right = p.right;
            newCore.dotPos = 0;
            for (const auto &la : newLookaheads) {
              LR1Item newItem { newCore, la };
              if (addToState(st, newItem)) {
                worklist.push(newItem);
              }
            }
          }
        }
      }
    }
  }
}

LR1State LR1TableBuilder::gotoState(const LR1State &st, const std::string &X) {
  LR1State res;
  for (const auto &item : st) {
    if (item.core.dotPos < static_cast<int>(item.core.right.size()) && item.core.right[item.core.dotPos] == X) {
      LR1Core newCore = item.core;
      newCore.dotPos++;
      LR1Item newItem { newCore, item.lookahead };
      addToState(res, newItem);
    }
  }
  return res;
}

bool LR1TableBuilder::isTerminal(const std::string &sym) const {
  if (sym == END_OF_INPUT)
    return true;
  return std::find(m_augGrammar.terminals.begin(), m_augGrammar.terminals.end(), sym) != m_augGrammar.terminals.end();
}

bool LR1TableBuilder::isNonTerminal(const std::string &sym) const {
  return std::find(m_augGrammar.nonterminals.begin(), m_augGrammar.nonterminals.end(), sym) != m_augGrammar.nonterminals.end();
}

bool LR1TableBuilder::addToState(LR1State &st, const LR1Item &item) {
  if (st.find(item) != st.end())
    return false;
  st.insert(item);
  return true;
}

int LR1TableBuilder::getStateId(const LR1State &st, bool &isNew) {
  std::string key = serializeState(st);
  auto it = m_stateIndex.find(key);
  if (it != m_stateIndex.end()) {
    isNew = false;
    return it->second;
  }
  int newId = static_cast<int>(m_states.size());
  m_states.push_back(st);
  m_stateIndex[key] = newId;
  isNew = true;
  return newId;
}

std::string LR1TableBuilder::serializeState(const LR1State &st) const {
  std::vector<std::string> lines;
  for (const auto &item : st) {
    std::ostringstream oss;
    oss << item.core.left << "->";
    for (int i = 0; i < static_cast<int>(item.core.right.size()); i++) {
      if (i == item.core.dotPos)
        oss << ".";
      oss << item.core.right[i] << " ";
    }
    if (item.core.dotPos == static_cast<int>(item.core.right.size()))
      oss << ".";
    oss << " /{" << item.lookahead << "}";
    lines.push_back(oss.str());
  }
  std::sort(lines.begin(), lines.end());
  std::ostringstream out;
  for (const auto &ln : lines)
    out << ln << "\n";
  return out.str();
}
