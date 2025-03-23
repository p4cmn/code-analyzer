#pragma once
#include "RegexAST.h"
#include "IRegexParser.h"

#include <memory>
#include <string>

/**
 * @brief Конкретная реализация IRegexParser для разбора регулярных выражений.
 *
 * Поддерживает базовый синтаксис:
 *   - Литералы: a, b, c, ...
 *   - Классы символов: [a-z] (с экранированием внутри `[]`)
 *   - Группы: ( ... )
 *   - Операции: | (альтернатива), * (0+), + (1+), ? (0 или 1)
 *   - Конкатенация (неявная, когда символы идут подряд)
 *   - Экранированные символы: \n, \r, \t, \\, \|, \*, \+, \? и т.д.
 */
class RegexParser final : public IRegexParser {
public:
    RegexParser() = default;
    ~RegexParser() override = default;

    /**
     * @brief Парсит строку `pattern` и возвращает корень AST.
     * @param pattern Строка регулярного выражения.
     * @return Указатель на корень AST (RegexAST).
     * @throws std::runtime_error При несоответствии синтаксису.
     */
    std::shared_ptr<RegexAST> parse(const std::string& pattern) override;

private:
    std::string m_pattern;
    size_t m_pos{0};

    /**
     * @brief Основной вход в рекурсивный парсер (парсит полный шаблон).
     */
    std::shared_ptr<RegexAST> parseImpl();

    /**
     * @brief Возвращает текущий символ (или '\0', если конец строки).
     */
    [[nodiscard]] char peek() const;

    /**
     * @brief Возвращает текущий символ, сдвигая позицию вперёд.
     */
    char get();

    /**
     * @brief Проверяет, достигли ли конца входной строки.
     */
    [[nodiscard]] bool eof() const;

    /**
     * @brief Если текущий символ равен `c`, считывает его и возвращает true, иначе false.
     */
    bool match(char c);

    /**
     * @brief Создаёт узел AST с заданным типом и (опциональными) потомками.
     * @param type Тип узла (Literal, Concat, Alt и т.д.).
     * @param left Левый потомок (по умолчанию nullptr).
     * @param right Правый потомок (по умолчанию nullptr).
     * @return Новый узел AST.
     */
    [[nodiscard]] std::shared_ptr<RegexAST> makeNode(RegexNodeType type,
                                       std::shared_ptr<RegexAST> left = nullptr,
                                       std::shared_ptr<RegexAST> right = nullptr) const;

    /**
     * @brief Парсит альтернативы (A|B).
     *        alt := cat ('|' cat)*
     */
    std::shared_ptr<RegexAST> parseAlt();

    /**
     * @brief Парсит конкатенацию (A B).
     *        cat := rep rep ...
     */
    std::shared_ptr<RegexAST> parseCat();

    /**
     * @brief Парсит квантификаторы (*, +, ?).
     *        rep := base (*|+|?)*
     */
    std::shared_ptr<RegexAST> parseRep();

    /**
     * @brief Парсит базовые элементы: группы (…), классы символов […], литералы, эпсилон.
     */
    std::shared_ptr<RegexAST> parseBase();

    /**
     * @brief Парсит класс символов вида `[abc\-]`, возвращая итоговую строку символов.
     * @throws std::runtime_error Если нет ']' или некорректная конструкция.
     */
    std::string parseCharClass();

    /**
     * @brief Парсит экранированный символ после символа '\\'.
     *        Поддерживает: \n, \r, \t, \\, \|, \*, \+, \?, \), \(, \[ и \].
     */
    char parseEscaped();
};
