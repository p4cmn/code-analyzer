#pragma once
#include "ILR1TableBuilder.h"

#include <map>
#include <set>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

/**
 * @brief Структура "ядра" LR(1)-пункта: (A -> α . β) без учёта lookahead.
 */
struct LR1Core {
    std::string left;                 ///< нетерминал A
    std::vector<std::string> right;   ///< правая часть αβ
    int dotPos;                       ///< позиция точки (сколько символов в α)

    bool operator==(const LR1Core &other) const {
      return (left == other.left)
             && (right == other.right)
             && (dotPos == other.dotPos);
    }
};

/**
 * @brief Хэш для LR1Core, чтобы можно было класть в unordered_map.
 */
struct LR1CoreHash {
    std::size_t operator()(const LR1Core &c) const {
      // Упрощённый способ суммировать хэши строк и dotPos
      std::size_t h = std::hash<std::string>()(c.left);
      h ^= std::hash<int>()(c.dotPos) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
      for (auto &sym : c.right) {
        std::size_t hh = std::hash<std::string>()(sym);
        h ^= hh + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
      }
      return h;
    }
};

/**
 * @brief Одно LR(1)-состояние: для каждого ядра (A->α.β) храним множество lookahead’ов.
 */
using LR1State = std::unordered_map<LR1Core, std::unordered_set<std::string>, LR1CoreHash>;

/**
 * @brief Реализация канонического LR(1) построения,
 *        включая слияние lookahead’ов (чтобы не было экспоненциального взрыва).
 */
class LR1TableBuilder : public ILR1TableBuilder {
public:
    LR1TableBuilder() = default;
    ~LR1TableBuilder() override = default;

    LRTable build(const Grammar& grammar) override;

private:
    static constexpr const char* END_OF_INPUT = "$";   ///< Специальный символ конца ввода
    static constexpr const char* AUGMENTED_START = "S'"; ///< Имя нового стартового нетерминала

    Grammar m_augGrammar; ///< Расширенная грамматика (добавим S'->S)
    LRTable m_table;      ///< Результирующая таблица ACTION/GOTO
    std::vector<LR1State> m_states;
    std::map<std::string, int> m_stateIndex;
    std::unordered_map<std::string, std::unordered_set<std::string>> m_firstNT;

private:
    void buildAugmentedGrammar(const Grammar &original);
    void validateGrammarSymbols(const Grammar &g);
    void computeAllFirstSets();

    std::set<std::string> firstOfSequence(const std::vector<std::string> &symbols) const;

    void closure(LR1State &st);

    LR1State gotoState(const LR1State &st, const std::string &X);

    bool isTerminal(const std::string &sym) const;
    bool isNonTerminal(const std::string &sym) const;

    /**
     * @brief Добавить (core, lookaheads) в состояние st (объединяя lookahead’ы, если ядро уже было).
     * @return true, если действительно что-то изменилось (добавились новые lookahead).
     */
    bool addToState(LR1State &st, const LR1Core &core, const std::unordered_set<std::string> &lookaheads);

    /**
     * @brief Получить (или создать) индекс состояния st.
     */
    int getStateId(const LR1State &st, bool &isNew);

    /**
     * @brief Превращаем LR1State в строку, чтобы класть в m_stateIndex для проверки "такое состояние уже было?"
     */
    std::string serializeState(const LR1State &st) const;
};
