#include "DFABuiler.h"

#include <set>
#include <queue>
#include <limits>
#include <sstream>
#include <unordered_map>

/**
 * @brief Возвращает epsilon-замыкание множества состояний.
 */
static std::set<int> epsilonClosure(const NFA &nfa, const std::set<int> &states) {
  std::set<int> closure = states;
  std::queue<int> q;
  for (int s : states) {
    q.push(s);
  }
  while (!q.empty()) {
    int s = q.front();
    q.pop();
    for (int nxt : nfa.states[s].epsilon) {
      if (closure.insert(nxt).second) {
        q.push(nxt);
      }
    }
  }
  return closure;
}

/**
 * @brief Возвращает множество состояний, достижимых из `states` по одному символу `symbol`.
 */
static std::set<int> move(const NFA &nfa, const std::set<int> &states, unsigned char symbol) {
  std::set<int> result;
  for (int s : states) {
    const auto &vec = nfa.states[s].transitions[symbol];
    for (int nxt : vec) {
      result.insert(nxt);
    }
  }
  return result;
}

DFA SubsetConstructionDFABuilder::buildFromNFA(const NFA &nfa) {
  DFA dfa;
  dfa.states.clear();
  dfa.startState = 0;
  if (nfa.startState < 0 || nfa.states.empty()) {
    DfaState st;
    for (int & transition : st.transitions) transition = -1;
    st.isAccept = false;
    st.tokenIndex = -1;
    dfa.states.push_back(st);
    return dfa;
  }
  auto start = epsilonClosure(nfa, {nfa.startState});
  std::unordered_map<std::string, int> dfaIndex;
  auto setToStr = [](const std::set<int> &stt) {
      std::ostringstream oss;
      for (int x : stt) {
        oss << x << ",";
      }
      return oss.str();
  };
  std::string startKey = setToStr(start);
  dfaIndex[startKey] = 0;
  DfaState st0;
  for (int & transition : st0.transitions) transition = -1;
  st0.isAccept = false;
  st0.tokenIndex = std::numeric_limits<int>::max();
  for (int s : start) {
    if (nfa.states[s].isAccept) {
      st0.isAccept = true;
      if (nfa.states[s].tokenIndex < st0.tokenIndex) {
        st0.tokenIndex = nfa.states[s].tokenIndex;
      }
    }
  }
  if (!st0.isAccept) {
    st0.tokenIndex = -1;
  }
  dfa.states.push_back(st0);
  std::queue<std::set<int>> unmarked;
  unmarked.push(start);
  while (!unmarked.empty()) {
    auto currSet = unmarked.front();
    unmarked.pop();
    int currIndex = dfaIndex[setToStr(currSet)];
    for (int c = 0; c < 256; c++) {
      auto moved = move(nfa, currSet, (unsigned char)c);
      if (!moved.empty()) {
        auto ec = epsilonClosure(nfa, moved);
        if (!ec.empty()) {
          auto key = setToStr(ec);
          if (dfaIndex.find(key) == dfaIndex.end()) {
            int newIndex = (int)dfa.states.size();
            dfaIndex[key] = newIndex;
            DfaState newState;
            for (int & transition : newState.transitions) transition = -1;
            newState.isAccept = false;
            newState.tokenIndex = std::numeric_limits<int>::max();
            for (int s : ec) {
              if (nfa.states[s].isAccept) {
                newState.isAccept = true;
                if (nfa.states[s].tokenIndex < newState.tokenIndex) {
                  newState.tokenIndex = nfa.states[s].tokenIndex;
                }
              }
            }
            if (!newState.isAccept) {
              newState.tokenIndex = -1;
            }
            dfa.states.push_back(newState);
            unmarked.push(ec);
          }
          int targetIndex = dfaIndex[key];
          dfa.states[currIndex].transitions[c] = targetIndex;
        }
      }
    }
  }
  return dfa;
}
