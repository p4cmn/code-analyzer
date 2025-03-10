#include "DFA.h"
#include <set>
#include <queue>
#include <string>
#include <limits>
#include <sstream>
#include <iostream>
#include <unordered_map>

static std::set<int> epsilonClosure(const NFA &nfa, const std::set<int> &states) {
  std::set<int> closure = states;
  std::queue<int> q;
  for (int s : states) {
    q.push(s);
  }
  while (!q.empty()) {
    int s = q.front();
    q.pop();
    for (int next : nfa.states[s].epsilon) {
      if (closure.insert(next).second) {
        q.push(next);
      }
    }
  }
  return closure;
}

static std::set<int> move(const NFA &nfa, const std::set<int> &states, unsigned char symbol) {
  std::set<int> result;
  for (int s : states) {
    const auto &trans = nfa.states[s].transitions[symbol];
    result.insert(trans.begin(), trans.end());
  }
  return result;
}

static std::string setToString(const std::set<int>& s) {
  std::ostringstream oss;
  for (int state : s) {
    oss << state << ",";
  }
  return oss.str();
}

DFA subsetConstruction(const NFA &nfa) {
  DFA dfa;
  dfa.states.clear();
  auto createDfaState = []() -> DfaState {
      DfaState state;
      for (int i = 0; i < 256; i++) {
        state.transitions[i] = -1;
      }
      state.isAccept = false;
      state.tokenIndex = std::numeric_limits<int>::max();
      return state;
  };
  std::set<int> startSet = epsilonClosure(nfa, {nfa.startState});
  std::unordered_map<std::string, int> stateMap;
  std::string startKey = setToString(startSet);
  stateMap[startKey] = 0;
  DfaState startDfaState = createDfaState();
  for (int s : startSet) {
    if (nfa.states[s].isAccept) {
      startDfaState.isAccept = true;
      startDfaState.tokenIndex = std::min(startDfaState.tokenIndex, nfa.states[s].tokenIndex);
    }
  }
  dfa.states.push_back(startDfaState);
  dfa.startState = 0;
  std::queue<std::set<int>> unmarked;
  unmarked.push(startSet);
  while (!unmarked.empty()) {
    std::set<int> currentSet = unmarked.front();
    unmarked.pop();
    std::string currentKey = setToString(currentSet);
    int currentDfaIndex = stateMap[currentKey];
    for (int c = 0; c < 256; c++) {
      std::set<int> moveSet = move(nfa, currentSet, static_cast<unsigned char>(c));
      if (moveSet.empty()) continue;
      std::set<int> newSet = epsilonClosure(nfa, moveSet);
      std::string newKey = setToString(newSet);
      if (stateMap.find(newKey) == stateMap.end()) {
        int newIndex = dfa.states.size();
        stateMap[newKey] = newIndex;
        DfaState newDfaState = createDfaState();
        for (int s : newSet) {
          if (nfa.states[s].isAccept) {
            newDfaState.isAccept = true;
            newDfaState.tokenIndex = std::min(newDfaState.tokenIndex, nfa.states[s].tokenIndex);
          }
        }
        dfa.states.push_back(newDfaState);
        unmarked.push(newSet);
      }
      int targetIndex = stateMap[newKey];
      dfa.states[currentDfaIndex].transitions[c] = targetIndex;
    }
  }
  return dfa;
}
