#pragma once
#include "RegexAST.h"

#include <memory>
#include <string>

/**
 * @brief Парсер регулярных выражений, строящий абстрактное синтаксическое дерево (AST).
 *
 * Поддерживаем базовый синтаксис:
 *   - Литералы: a, b, c, ...
 *   - Классы символов: [a-z], включая экранирование внутри []
 *   - Группы: ( ... )
 *   - Операции: | (альтернатива), * (0+), + (1+), ? (0 или 1)
 *   - Конкатенация (неявная, когда символы идут подряд)
 *   - Экранированные символы: \n, \r, \t, \\, \|, \*, \+, \? и т.д.
 */
class RegexParser {
public:
    /**
     * @brief Создаёт парсер для указанной строки-шаблона.
     * @param pattern Само регулярное выражение.
     */
    explicit RegexParser(const std::string& pattern);

    /**
     * @brief Запускает процесс парсинга и возвращает корень AST.
     * @throws std::runtime_error при ошибках разбора.
     */
    std::shared_ptr<RegexAST> parse();

private:
    std::string pattern_; ///< Исходная строка с регуляркой
    size_t pos_;          ///< Текущая позиция в строке

    /**
     * @brief Возвращает текущий символ (или '\0', если конец).
     */
    [[nodiscard]] char peek() const;

    /**
     * @brief Считывает текущий символ и сдвигает `pos_`.
     */
    char get();

    /**
     * @brief Проверяет, не достигли ли конца строки.
     */
    [[nodiscard]] bool eof() const;

    /**
     * @brief Если текущий символ равен c, считывает его и возвращает true, иначе false.
     */
    bool match(char c);

    /**
     * @brief Создаёт узел AST с заданным типом и (опциональными) потомками.
     */
    std::shared_ptr<RegexAST> makeNode(RegexNodeType type,
                                       std::shared_ptr<RegexAST> left = nullptr,
                                       std::shared_ptr<RegexAST> right = nullptr);

    /**
     * @brief Парсит альтернативы (A|B), на верхнем уровне.
     *        alt := cat ('|' cat)*
     */
    std::shared_ptr<RegexAST> parseAlt();

    /**
     * @brief Парсит конкатенацию (A B), учитывая приоритет.
     *        cat := rep rep ...
     */
    std::shared_ptr<RegexAST> parseCat();

    /**
     * @brief Парсит квантификаторы (*, +, ?).
     *        rep := base (*|+|?)*
     */
    std::shared_ptr<RegexAST> parseRep();

    /**
     * @brief Парсит базовые элементы: группы (…), классы символов […], литерал, эпсилон.
     */
    std::shared_ptr<RegexAST> parseBase();

    /**
     * @brief Парсит класс символов вида `[abc\-]`, возвращая строку с возможными символами.
     * @throws std::runtime_error если нет ']' или некорректная конструкция.
     */
    std::string parseCharClass();

    /**
     * @brief Парсит экранированный символ (после '\').
     * \n, \r, \t, \\, \|, \*, \+, \?.
     */
    char parseEscaped();
};
