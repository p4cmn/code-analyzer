#include "NFABuilder.h"
#include <stdexcept>

static std::vector<NFAState> copyStatesWithOffset(const std::vector<NFAState>& states, int offset) {
  std::vector<NFAState> copied;
  copied.resize(states.size());
  for (size_t i = 0; i < states.size(); i++) {
    copied[i] = states[i];
    for (int j = 0; j < 256; j++) {
      for (int &target : copied[i].transitions[j]) {
        target += offset;
      }
    }
    for (int &eps : copied[i].epsilon) {
      eps += offset;
    }
  }
  return copied;
}

static int addState(NFA& nfa) {
  NFAState state;
  nfa.states.push_back(state);
  return nfa.states.size() - 1;
}

static NFA buildBasicNFA(char c) {
  NFA nfa;
  int s0 = addState(nfa);
  int s1 = addState(nfa);
  nfa.startState = s0;
  nfa.acceptState = s1;
  if (c == '\0') {
    nfa.states[s0].epsilon.push_back(s1);
  } else {
    nfa.states[s0].transitions[(unsigned char)c].push_back(s1);
  }
  nfa.states[s1].isAccept = true;
  return nfa;
}

static NFA concatNFA(const NFA& a, const NFA& b) {
  NFA nfa;
  nfa.states = a.states;
  int offset = nfa.states.size();
  auto bCopied = copyStatesWithOffset(b.states, offset);
  nfa.states.insert(nfa.states.end(), bCopied.begin(), bCopied.end());
  nfa.states[a.acceptState].epsilon.push_back(b.startState + offset);
  nfa.states[a.acceptState].isAccept = false;
  nfa.startState = a.startState;
  nfa.acceptState = b.acceptState + offset;
  return nfa;
}

static NFA alternateNFA(const NFA& a, const NFA& b) {
  NFA nfa;
  int s0 = addState(nfa);
  int offsetA = nfa.states.size();
  auto aCopied = copyStatesWithOffset(a.states, offsetA);
  nfa.states.insert(nfa.states.end(), aCopied.begin(), aCopied.end());
  int offsetB = nfa.states.size();
  auto bCopied = copyStatesWithOffset(b.states, offsetB);
  nfa.states.insert(nfa.states.end(), bCopied.begin(), bCopied.end());
  int s1 = addState(nfa);
  nfa.states[s0].epsilon.push_back(a.startState + offsetA);
  nfa.states[s0].epsilon.push_back(b.startState + offsetB);
  nfa.states[a.acceptState + offsetA].isAccept = false;
  nfa.states[a.acceptState + offsetA].epsilon.push_back(s1);
  nfa.states[b.acceptState + offsetB].isAccept = false;
  nfa.states[b.acceptState + offsetB].epsilon.push_back(s1);
  nfa.states[s1].isAccept = true;
  nfa.startState = s0;
  nfa.acceptState = s1;
  return nfa;
}

static NFA starNFA(const NFA& a) {
  NFA nfa;
  int s0 = addState(nfa);
  int offset = nfa.states.size();
  auto aCopied = copyStatesWithOffset(a.states, offset);
  nfa.states.insert(nfa.states.end(), aCopied.begin(), aCopied.end());
  int s1 = addState(nfa);
  nfa.states[s0].epsilon.push_back(a.startState + offset);
  nfa.states[s0].epsilon.push_back(s1);
  nfa.states[a.acceptState + offset].isAccept = false;
  nfa.states[a.acceptState + offset].epsilon.push_back(s1);
  nfa.states[a.acceptState + offset].epsilon.push_back(a.startState + offset);
  nfa.states[s1].isAccept = true;
  nfa.startState = s0;
  nfa.acceptState = s1;
  return nfa;
}

static NFA plusNFA(const NFA& a) {
  NFA star = starNFA(a);
  return concatNFA(a, star);
}

static NFA questionNFA(const NFA& a) {
  NFA epsilon = buildBasicNFA('\0');
  return alternateNFA(a, epsilon);
}

static NFA buildNFAFromAST(const std::shared_ptr<RegexAST>& ast) {
  if (!ast)
    return buildBasicNFA('\0');
  switch (ast->type) {
    case RegexNodeType::Literal:
      return buildBasicNFA(ast->literal);
    case RegexNodeType::Epsilon:
      return buildBasicNFA('\0');
    case RegexNodeType::CharClass: {
      if (ast->charClass.empty())
        throw std::runtime_error("Пустой класс символов");
      NFA result = buildBasicNFA(ast->charClass[0]);
      for (size_t i = 1; i < ast->charClass.size(); ++i) {
        NFA temp = buildBasicNFA(ast->charClass[i]);
        result = alternateNFA(result, temp);
      }
      return result;
    }
    case RegexNodeType::Concat: {
      NFA left = buildNFAFromAST(ast->left);
      NFA right = buildNFAFromAST(ast->right);
      return concatNFA(left, right);
    }
    case RegexNodeType::Alt: {
      NFA left = buildNFAFromAST(ast->left);
      NFA right = buildNFAFromAST(ast->right);
      return alternateNFA(left, right);
    }
    case RegexNodeType::Star: {
      NFA sub = buildNFAFromAST(ast->left);
      return starNFA(sub);
    }
    case RegexNodeType::Plus: {
      NFA sub = buildNFAFromAST(ast->left);
      return plusNFA(sub);
    }
    case RegexNodeType::Question: {
      NFA sub = buildNFAFromAST(ast->left);
      return questionNFA(sub);
    }
    default:
      throw std::runtime_error("Неизвестный тип узла AST");
  }
}

NFA NFABuilder::buildFromAST(const std::shared_ptr<RegexAST>& ast) {
  return buildNFAFromAST(ast);
}

NFA NFABuilder::buildCombinedNFA(const std::vector<std::shared_ptr<RegexAST>>& asts,
                                 const std::vector<int>& tokenIndices) {
  if (asts.size() != tokenIndices.size())
    throw std::runtime_error("Размеры векторов AST и tokenIndices должны совпадать");
  NFA combined;
  combined.states.clear();
  int newStart = addState(combined);
  combined.startState = newStart;
  combined.acceptState = -1;
  for (size_t i = 0; i < asts.size(); ++i) {
    NFA tokenNFA = buildFromAST(asts[i]);
    tokenNFA.states[tokenNFA.acceptState].tokenIndex = tokenIndices[i];
    int offset = combined.states.size();
    auto copied = copyStatesWithOffset(tokenNFA.states, offset);
    combined.states.insert(combined.states.end(), copied.begin(), copied.end());
    combined.states[newStart].epsilon.push_back(tokenNFA.startState + offset);
  }
  return combined;
}
