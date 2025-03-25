#pragma once
#include "LRTable.h"
#include "ILR1TableBuilder.h"
#include "../Grammar/Grammar.h"

#include <map>
#include <set>
#include <queue>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

/**
 * @brief Структура, представляющая "ядро" LR(1)-пункта: A -> α . β (без учета lookahead).
 */
struct LR1Core {
    std::string left;
    std::vector<std::string> right;
    int dotPos;

    bool operator==(const LR1Core &other) const {
      return left == other.left &&
             right == other.right &&
             dotPos == other.dotPos;
    }
};

/**
 * @brief Хэш-функция для LR1Core.
 */
struct LR1CoreHash {
    std::size_t operator()(const LR1Core &c) const {
      std::size_t h = std::hash<std::string>()(c.left);
      h ^= std::hash<int>()(c.dotPos) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
      for (const auto &sym : c.right) {
        std::size_t hh = std::hash<std::string>()(sym);
        h ^= hh + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
      }
      return h;
    }
};

/**
 * @brief Один LR(1)-элемент, состоящий из ядра и символа lookahead.
 */
struct LR1Item {
    LR1Core core;
    std::string lookahead;

    bool operator==(const LR1Item &other) const {
      return core == other.core && lookahead == other.lookahead;
    }
};

/**
 * @brief Хэш-функция для LR1Item.
 */
struct LR1ItemHash {
    std::size_t operator()(const LR1Item &item) const {
      std::size_t h = LR1CoreHash()(item.core);
      h ^= std::hash<std::string>()(item.lookahead) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
      return h;
    }
};

/**
 * @brief LR(1)-состояние – множество LR(1)-элементов.
 */
using LR1State = std::unordered_set<LR1Item, LR1ItemHash>;

/**
 * @brief Класс для построения LR(1)-таблицы (ACTION и GOTO) по заданной грамматике.
 *
 * Реализует канонический алгоритм построения множества LR(1)-элементов, вычисление
 * FIRST-множеств и построение таблицы разбора.
 */
class LR1TableBuilder : public ILR1TableBuilder {
public:
    LR1TableBuilder() = default;
    ~LR1TableBuilder() override = default;

    /**
     * @brief Построение LR(1)-таблицы для заданной грамматики.
     *
     * @param grammar Грамматика с терминалами, нетерминалами, продукциями и стартовым символом.
     * @return Объект LRTable, содержащий таблицы ACTION и GOTO.
     * @throws std::runtime_error При обнаружении конфликтов или некорректных данных.
     */
    LRTable build(const Grammar& grammar) override;

private:
    static constexpr const char* END_OF_INPUT = "$";       ///< Маркер конца ввода.
    static constexpr const char* AUGMENTED_START = "S'";     ///< Дополнительный стартовый символ.

    Grammar m_augGrammar;                        ///< Аугментированная грамматика.
    LRTable m_table;                             ///< Таблица разбора (ACTION и GOTO).
    std::vector<LR1State> m_states;                ///< Множество LR(1)-состояний.
    std::map<std::string, int> m_stateIndex;       ///< Отображение сериализованного состояния в его идентификатор.
    std::unordered_map<std::string, std::unordered_set<std::string>> m_firstNT; ///< FIRST-множества для нетерминалов.

    /**
     * @brief Формирует аугментированную грамматику, добавляя новую продукцию старта.
     *
     * @param original Исходная грамматика.
     */
    void buildAugmentedGrammar(const Grammar &original);

    /**
     * @brief Валидирует символы грамматики: проверяет, что все символы, используемые в продукциях,
     *        объявлены в списках терминалов или нетерминалов.
     *
     * @param g Грамматика для проверки.
     * @throws std::runtime_error Если обнаружен неописанный символ.
     */
    void validateGrammarSymbols(const Grammar &g);

    /**
     * @brief Вычисляет FIRST-множества для всех нетерминалов в аугментированной грамматике.
     */
    void computeAllFirstSets();

    /**
     * @brief Вычисляет FIRST-множество для последовательности символов.
     *
     * @param symbols Последовательность грамматических символов.
     * @return Множество терминальных символов, входящих в FIRST.
     */
    std::set<std::string> firstOfSequence(const std::vector<std::string> &symbols) const;

    /**
     * @brief Вычисляет замыкание для LR(1)-состояния.
     *
     * @param st Состояние, для которого необходимо вычислить замыкание.
     */
    void closure(LR1State &st);

    /**
     * @brief Вычисляет переход GOTO для заданного LR(1)-состояния по символу X.
     *
     * @param st Текущее LR(1)-состояние.
     * @param X Грамматический символ для перехода.
     * @return Новое LR(1)-состояние после перехода.
     */
    LR1State gotoState(const LR1State &st, const std::string &X);

    /**
     * @brief Проверяет, является ли символ терминалом.
     *
     * @param sym Грамматический символ.
     * @return true, если символ является терминалом; иначе false.
     */
    bool isTerminal(const std::string &sym) const;

    /**
     * @brief Проверяет, является ли символ нетерминалом.
     *
     * @param sym Грамматический символ.
     * @return true, если символ является нетерминалом; иначе false.
     */
    bool isNonTerminal(const std::string &sym) const;

    /**
     * @brief Добавляет LR(1)-элемент в состояние, если его там еще нет.
     *
     * @param st LR(1)-состояние.
     * @param item LR(1)-элемент для добавления.
     * @return true, если элемент был добавлен; иначе false.
     */
    bool addToState(LR1State &st, const LR1Item &item);

    /**
     * @brief Возвращает идентификатор состояния. Если состояние новое, оно добавляется в коллекцию.
     *
     * @param st LR(1)-состояние.
     * @param isNew Переменная, указывающая, было ли состояние создано заново.
     * @return Идентификатор состояния.
     */
    int getStateId(const LR1State &st, bool &isNew);

    /**
     * @brief Сериализует LR(1)-состояние в строковое представление.
     *
     * @param st LR(1)-состояние.
     * @return Строковое представление состояния.
     */
    std::string serializeState(const LR1State &st) const;
};
