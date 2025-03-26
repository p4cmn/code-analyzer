#include "NFABuilder.h"
#include <stdexcept>
#include <sstream>

/**
 * @brief Добавляет новое состояние (пустое) в NFA.
 */
int ThompsonNFABuilder::addState(NFA &nfa) {
  NFAState st;
  nfa.states.push_back(st);
  return static_cast<int>(nfa.states.size()) - 1;
}

/**
 * @brief Копирует состояния в новый массив со смещением индексов переходов и epsilon.
 */
std::vector<NFAState> ThompsonNFABuilder::copyStatesWithOffset(const std::vector<NFAState> &states, int offset) {
  std::vector<NFAState> result = states;
  for (auto &st : result) {
    for (auto &vec : st.transitions) {
      for (int &t : vec) {
        t += offset;
      }
    }
    // Смещаем все epsilon-переходы
    for (int &e : st.epsilon) {
      e += offset;
    }
  }
  return result;
}

/**
 * @brief Разворачивает диапазоны вида "a-z" в "abc...xyz".
 */
std::string ThompsonNFABuilder::expandCharClass(const std::string &charClassExpr)
{
  std::string expanded;
  size_t i = 0;
  while (i < charClassExpr.size()) {
    // Если впереди как минимум три символа и средний — '-'
    if (i + 2 < charClassExpr.size() && charClassExpr[i + 1] == '-') {
      char start = charClassExpr[i];
      char end   = charClassExpr[i + 2];
      if (start > end) {
        throw std::runtime_error("Bad range in char class: " + charClassExpr);
      }
      for (char c = start; c <= end; ++c) {
        expanded.push_back(c);
      }
      i += 3;
    } else {
      expanded.push_back(charClassExpr[i]);
      ++i;
    }
  }
  return expanded;
}

/**
 * @brief Строит NFA из одного символа (или эпсилон, если c == '\0').
 */
NFA ThompsonNFABuilder::buildBasicNFA(char c) {
  NFA nfa;
  int s0 = addState(nfa);
  int s1 = addState(nfa);
  nfa.startState  = s0;
  nfa.acceptState = s1;
  nfa.states[s1].isAccept = true;
  if (c == '\0') {
    nfa.states[s0].epsilon.push_back(s1);
  }
  else {
    unsigned char uc = static_cast<unsigned char>(c);
    nfa.states[s0].transitions[uc].push_back(s1);
  }
  return nfa;
}

/**
 * @brief Альтернатива A|B: новое стартовое, новое конечное, epsilon-ребра.
 */
NFA ThompsonNFABuilder::alternateNFA(const NFA &a, const NFA &b) {
  NFA result;
  int offsetA = static_cast<int>(result.states.size()); {
    auto copyA = copyStatesWithOffset(a.states, offsetA);
    result.states.insert(result.states.end(), copyA.begin(), copyA.end());
  }
  int offsetB = static_cast<int>(result.states.size()); {
    auto copyB = copyStatesWithOffset(b.states, offsetB);
    result.states.insert(result.states.end(), copyB.begin(), copyB.end());
  }
  int newStart = addState(result);
  int newAccept = addState(result);
  for (auto &st : result.states) {
    st.isAccept = false;
  }
  result.states[newStart].epsilon.push_back(a.startState + offsetA);
  result.states[newStart].epsilon.push_back(b.startState + offsetB);
  result.states[a.acceptState + offsetA].epsilon.push_back(newAccept);
  result.states[b.acceptState + offsetB].epsilon.push_back(newAccept);
  result.states[a.acceptState + offsetA].tokenIndex = a.states[a.acceptState].tokenIndex;
  result.states[b.acceptState + offsetB].tokenIndex = b.states[b.acceptState].tokenIndex;
  result.states[newAccept].isAccept = true;
  result.startState  = newStart;
  result.acceptState = newAccept;
  return result;
}

/**
 * @brief Конкатенация A B:
 *  - копируем состояния A,
 *  - копируем состояния B,
 *  - соединяем accept(A) -> start(B) эпсилон-переходом
 */
NFA ThompsonNFABuilder::concatNFA(const NFA &a, const NFA &b) {
  NFA result;
  int offsetA = static_cast<int>(result.states.size()); {
    auto copyA = copyStatesWithOffset(a.states, offsetA);
    result.states.insert(result.states.end(), copyA.begin(), copyA.end());
  }
  int offsetB = static_cast<int>(result.states.size()); {
    auto copyB = copyStatesWithOffset(b.states, offsetB);
    result.states.insert(result.states.end(), copyB.begin(), copyB.end());
  }
  int acceptA = a.acceptState + offsetA;
  result.states[acceptA].isAccept = false;
  result.states[acceptA].epsilon.push_back(b.startState + offsetB);
  result.startState  = a.startState + offsetA;
  result.acceptState = b.acceptState + offsetB;
  result.states[result.acceptState].isAccept    = true;
  result.states[result.acceptState].tokenIndex  = b.states[b.acceptState].tokenIndex;
  return result;
}

/**
 * @brief Звезда Клини A*: новое start, newAccept.
 *  - epsilon(newStart -> A.start, newStart -> newAccept)
 *  - epsilon(A.accept -> A.start, A.accept -> newAccept)
 */
NFA ThompsonNFABuilder::starNFA(const NFA &a) {
  NFA result;
  int offsetA = static_cast<int>(result.states.size()); {
    auto copyA = copyStatesWithOffset(a.states, offsetA);
    result.states.insert(result.states.end(), copyA.begin(), copyA.end());
  }
  int newStart = addState(result);
  int newAccept = addState(result);
  for (auto &st : result.states) {
    st.isAccept = false;
  }
  result.states[newStart].epsilon.push_back(a.startState + offsetA);
  result.states[newStart].epsilon.push_back(newAccept);
  int acceptA = a.acceptState + offsetA;
  result.states[acceptA].epsilon.push_back(a.startState + offsetA);
  result.states[acceptA].epsilon.push_back(newAccept);
  result.states[newAccept].isAccept = true;
  result.startState  = newStart;
  result.acceptState = newAccept;
  return result;
}

/**
 * @brief A+ = A concat (A*)
 */
NFA ThompsonNFABuilder::plusNFA(const NFA &a)
{
  // Cначала A*, потом конкатенируем: A (A*)
  NFA aStar = starNFA(a);
  return concatNFA(a, aStar);
}

/**
 * @brief A? = (ε | A)
 */
NFA ThompsonNFABuilder::questionNFA(const NFA &a) {
  NFA eps = buildBasicNFA('\0');
  return alternateNFA(eps, a);
}

/**
 * @brief Рекурсивная функция для построения NFA из AST.
 */
NFA ThompsonNFABuilder::buildFromASTImpl(const std::shared_ptr<RegexAST> &ast) {
  if (!ast) {
    return buildBasicNFA('\0');
  }
  switch (ast->type) {
    case RegexNodeType::Literal:
      return buildBasicNFA(ast->literal);
    case RegexNodeType::Epsilon:
      return buildBasicNFA('\0');
    case RegexNodeType::CharClass: {
      std::string expanded = expandCharClass(ast->charClass);
      if (expanded.empty()) {
        throw std::runtime_error("Пустой класс символов (CharClass) в регулярном выражении.");
      }
      NFA result = buildBasicNFA(expanded[0]);
      for (size_t i = 1; i < expanded.size(); i++) {
        NFA tmp = buildBasicNFA(expanded[i]);
        result = alternateNFA(result, tmp);
      }
      return result;
    }
    case RegexNodeType::Concat: {
      NFA leftNFA  = buildFromASTImpl(ast->left);
      NFA rightNFA = buildFromASTImpl(ast->right);
      return concatNFA(leftNFA, rightNFA);
    }
    case RegexNodeType::Alt: {
      NFA leftNFA  = buildFromASTImpl(ast->left);
      NFA rightNFA = buildFromASTImpl(ast->right);
      return alternateNFA(leftNFA, rightNFA);
    }
    case RegexNodeType::Star: {
      NFA sub = buildFromASTImpl(ast->left);
      return starNFA(sub);
    }
    case RegexNodeType::Plus: {
      NFA sub = buildFromASTImpl(ast->left);
      return plusNFA(sub);
    }
    case RegexNodeType::Question: {
      NFA sub = buildFromASTImpl(ast->left);
      return questionNFA(sub);
    }
    default:
      throw std::runtime_error("Неизвестный тип узла RegexAST при построении NFA.");
  }
}

/**
 * @brief Публичный метод: строит NFA по одному AST.
 */
NFA ThompsonNFABuilder::buildFromAST(const std::shared_ptr<RegexAST> &ast) {
  NFA result = buildFromASTImpl(ast);
  if (result.states.empty()) {
    return buildBasicNFA('\0');
  }
  return result;
}

/**
 * @brief Строит объединённый NFA из нескольких AST.
 *        Новый startState + epsilon в start каждого автомата.
 */
NFA ThompsonNFABuilder::buildCombinedNFA(const std::vector<std::shared_ptr<RegexAST>> &asts,
                                         const std::vector<int> &tokenIndices) {
  if (asts.size() != tokenIndices.size()) {
    throw std::runtime_error("Размер массива AST не совпадает с размером массива tokenIndices.");
  }
  NFA combined;
  int newStart = addState(combined);
  combined.startState = newStart;
  combined.acceptState = -1;
  for (size_t i = 0; i < asts.size(); ++i) {
    NFA local = buildFromAST(asts[i]);
    for (auto &st : local.states) {
      if (st.isAccept) {
        st.tokenIndex = tokenIndices[i];
      }
    }
    int offset = static_cast<int>(combined.states.size());
    auto copyVec = copyStatesWithOffset(local.states, offset);
    combined.states.insert(combined.states.end(), copyVec.begin(), copyVec.end());
    combined.states[newStart].epsilon.push_back(local.startState + offset);
  }
  return combined;
}
