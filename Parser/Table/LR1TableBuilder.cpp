// LR1TableBuilder.cpp

#include "LR1TableBuilder.h"
#include <stdexcept>
#include <queue>
#include <algorithm>
#include <sstream>
#include <iostream>

LRTable LR1TableBuilder::build(const Grammar &grammar) {
  validateGrammarSymbols(grammar);
  buildAugmentedGrammar(grammar);
  computeAllFirstSets();
  LR1State startState;
  {
    LR1Core core;
    core.left = AUGMENTED_START;
    core.right = { m_augGrammar.startSymbol };
    core.dotPos = 0;
    std::unordered_set<std::string> la;
    la.insert(END_OF_INPUT);
    addToState(startState, core, la);
  }
  closure(startState);
  bool isNew = false;
  int startId = getStateId(startState, isNew);
  std::queue<int> Q;
  if (isNew) {
    Q.push(startId);
  }
  while (!Q.empty()) {
    int stId = Q.front();
    Q.pop();
    const auto &st = m_states[stId];
    std::unordered_set<std::string> symbolsAfterDot;
    for (auto &kv : st) {
      const LR1Core &core = kv.first;
      if (core.dotPos < (int)core.right.size()) {
        symbolsAfterDot.insert(core.right[core.dotPos]);
      }
    }
    for (auto &X : symbolsAfterDot) {
      LR1State toSt = gotoState(st, X);
      if (!toSt.empty()) {
        // closure(...)
        closure(toSt);
        bool wasNew = false;
        int id2 = getStateId(toSt, wasNew);
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
    for (auto &kv : st) {
      const LR1Core &core = kv.first;
      const auto &lookaheads = kv.second;
      if (core.dotPos == (int)core.right.size()) {
        // Это либо S'->S ., либо обычное "E->E + id ."
        if (core.left == AUGMENTED_START) {
          for (auto &la : lookaheads) {
            auto &row = m_table.action[stId];
            if (row.find(la) != row.end() && row[la].type != LRActionType::ERROR) {
              throw std::runtime_error("Conflict ACCEPT / ??? on lookahead: " + la);
            }
            LRAction acc;
            acc.type = LRActionType::ACCEPT;
            row[la] = acc;
          }
        } else {
          int prodIndex = -1;
          for (int i = 0; i < (int)m_augGrammar.productions.size(); i++) {
            auto &p = m_augGrammar.productions[i];
            if (p.left == core.left && p.right == core.right) {
              prodIndex = i;
              break;
            }
          }
          if (prodIndex < 0) {
            throw std::runtime_error("Cannot find production for reduce: " + core.left);
          }
          for (auto &la : lookaheads) {
            auto &row = m_table.action[stId];
            if (row.find(la) != row.end() && row[la].type != LRActionType::ERROR) {
              throw std::runtime_error("REDUCE/SHIFT conflict (symbol: " + la + ")");
            }
            LRAction r;
            r.type = LRActionType::REDUCE;
            r.productionIndex = prodIndex;
            row[la] = r;
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
  aug.left = AUGMENTED_START; // "S'"
  aug.right.push_back(original.startSymbol);
  m_augGrammar.productions.insert(m_augGrammar.productions.begin(), aug);
  m_augGrammar.nonterminals.insert(m_augGrammar.nonterminals.begin(), AUGMENTED_START);
}

void LR1TableBuilder::validateGrammarSymbols(const Grammar &g) {
  std::unordered_set<std::string> all;
  for (auto &t : g.terminals) {
    all.insert(t);
  }
  for (auto &nt : g.nonterminals) {
    all.insert(nt);
  }
  if (all.find(g.startSymbol) == all.end()) {
    throw std::runtime_error(
            "StartSymbol '" + g.startSymbol + "' not found in either terminals or nonterminals"
    );
  }
  for (auto &prod : g.productions) {
    if (all.find(prod.left) == all.end()) {
      throw std::runtime_error("Left side of rule '" + prod.left + "' is not in T∪NT");
    }
    for (auto &sym : prod.right) {
      if (all.find(sym) == all.end()) {
        throw std::runtime_error("Symbol '" + sym + "' in right side is not in T∪NT");
      }
    }
  }
}

void LR1TableBuilder::computeAllFirstSets() {
  m_firstNT.clear();
  for (auto &nt : m_augGrammar.nonterminals) {
    m_firstNT[nt] = {};
  }
  bool changed = true;
  while (changed) {
    changed = false;
    for (auto &prod : m_augGrammar.productions) {
      const auto &A = prod.left;
      auto &firstA = m_firstNT[A];
      bool allEps = true; // если все символы могут дать ε
      for (const auto & sym : prod.right) {
        if (isTerminal(sym)) {
          size_t oldSize = firstA.size();
          firstA.insert(sym);
          if (firstA.size() > oldSize) {
            changed = true;
          }
          allEps = false;
          break;
        }
        else if (isNonTerminal(sym)) {
          size_t oldSize = firstA.size();
          auto &fstB = m_firstNT[sym];
          bool hasEps = false;
          for (auto &x : fstB) {
            if (x.empty()) {
              hasEps = true;
            } else {
              firstA.insert(x);
            }
          }
          if (firstA.size() > oldSize) {
            changed = true;
          }
          if (!hasEps) {
            allEps = false;
            break;
          }
        }
        else {
          allEps = false;
          break;
        }
      }
      if (allEps) {
        size_t oldSize = firstA.size();
        firstA.insert(std::string());
        if (firstA.size() > oldSize) {
          changed = true;
        }
      }
    }
  }
}

std::set<std::string> LR1TableBuilder::firstOfSequence(const std::vector<std::string> &symbols) const {
  std::set<std::string> result;
  if (symbols.empty()) {
    result.insert(std::string()); // ""
    return result;
  }
  bool allEps = true;
  for (auto &sym : symbols) {
    if (isTerminal(sym)) {
      result.insert(sym);
      allEps = false;
      break;
    }
    else if (isNonTerminal(sym)) {
      auto &fstB = m_firstNT.at(sym);
      bool hasEps = false;
      for (auto &x : fstB) {
        if (x.empty()) {
          hasEps = true;
        } else {
          result.insert(x);
        }
      }
      if (!hasEps) {
        allEps = false;
        break;
      }
    }
    else {
      allEps = false;
      break;
    }
  }
  if (allEps) {
    result.insert(std::string()); // ""
  }
  return result;
}

void LR1TableBuilder::closure(LR1State &st) {
  bool changed = true;
  while (changed) {
    changed = false;
    std::vector<std::pair<LR1Core, std::unordered_set<std::string>>> items;
    items.reserve(st.size());
    for (auto &kv : st) {
      items.emplace_back( kv.first, kv.second );
    }
    for (auto &it : items) {
      const LR1Core &core = it.first;
      const auto &lookaheads = it.second;
      if (core.dotPos < (int)core.right.size()) {
        std::string B = core.right[core.dotPos];
        if (isNonTerminal(B)) {
          std::vector<std::string> beta;
          for (int i = core.dotPos + 1; i < (int)core.right.size(); i++) {
            beta.push_back(core.right[i]);
          }
          auto fstBeta = firstOfSequence(beta);
          std::unordered_set<std::string> newLA;
          bool hasEps = (fstBeta.find(std::string()) != fstBeta.end());
          for (auto &x : fstBeta) {
            if (!x.empty()) {
              newLA.insert(x);
            }
          }
          if (hasEps) {
            for (auto &la : lookaheads) {
              newLA.insert(la);
            }
          }
          for (auto &p : m_augGrammar.productions) {
            if (p.left == B) {
              LR1Core c2;
              c2.left = B;
              c2.right = p.right;
              c2.dotPos = 0;
              if (!newLA.empty()) {
                if (addToState(st, c2, newLA)) {
                  changed = true;
                }
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
  for (auto &kv : st) {
    const LR1Core &core = kv.first;
    const auto &lookaheads = kv.second;
    if (core.dotPos < (int)core.right.size()) {
      if (core.right[core.dotPos] == X) {
        LR1Core c2 = core;
        c2.dotPos++;
        addToState(res, c2, lookaheads);
      }
    }
  }
  return res;
}

bool LR1TableBuilder::isTerminal(const std::string &sym) const {
  if (sym == END_OF_INPUT) {
    return true;
  }
  auto &ts = m_augGrammar.terminals;
  return (std::find(ts.begin(), ts.end(), sym) != ts.end());
}

bool LR1TableBuilder::isNonTerminal(const std::string &sym) const {
  auto &nts = m_augGrammar.nonterminals;
  return (std::find(nts.begin(), nts.end(), sym) != nts.end());
}

bool LR1TableBuilder::addToState(LR1State &st, const LR1Core &core, const std::unordered_set<std::string> &lookaheads) {
  auto it = st.find(core);
  if (it == st.end()) {
    st[core] = lookaheads;
    return true;
  } else {
    bool changed = false;
    for (auto &la : lookaheads) {
      auto insRes = it->second.insert(la);
      if (insRes.second) {
        changed = true;
      }
    }
    return changed;
  }
}

int LR1TableBuilder::getStateId(const LR1State &st, bool &isNew) {
  std::string key = serializeState(st);
  auto it = m_stateIndex.find(key);
  if (it != m_stateIndex.end()) {
    isNew = false;
    return it->second;
  }
  int newId = (int)m_states.size();
  m_states.push_back(st);
  m_stateIndex[key] = newId;
  isNew = true;
  return newId;
}

std::string LR1TableBuilder::serializeState(const LR1State &st) const {
  std::vector<std::string> lines;
  lines.reserve(st.size());
  for (auto &kv : st) {
    const LR1Core &core = kv.first;
    const auto &las = kv.second;
    std::ostringstream oss;
    oss << core.left << "->";
    for (int i = 0; i < (int)core.right.size(); i++) {
      if (i == core.dotPos) {
        oss << ".";
      }
      oss << core.right[i];
    }
    if (core.dotPos == (int)core.right.size()) {
      oss << ".";
    }
    std::vector<std::string> laVec(las.begin(), las.end());
    std::sort(laVec.begin(), laVec.end());
    oss << " /{";
    for (size_t i = 0; i < laVec.size(); i++) {
      oss << laVec[i];
      if (i + 1 < laVec.size()) {
        oss << ",";
      }
    }
    oss << "}";

    lines.push_back(oss.str());
  }
  std::sort(lines.begin(), lines.end());
  std::ostringstream out;
  for (auto &ln : lines) {
    out << ln << "\n";
  }
  return out.str();
}
