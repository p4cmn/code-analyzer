#pragma once

#include "ITokenSpecReader.h"
#include <utility>

/**
 * @brief Реализация ITokenSpecReader, читающая спецификации токенов из текстового файла.
 * Формат строк в файле (пример):
 *   IDENT [a-zA-Z_][a-zA-Z0-9_]* false 5
 *   NUMBER [0-9]+ false 4
 *   WHITESPACE [ \t\r\n]+ true 1
 *   KEYWORD (auto|break|case) false 10
 *   # комментарий
 *
 * Здесь:
 *   - Первое "слово" до пробела: имя токена.
 *   - Последние два "слова": флаг ignore (true/false/1/0) и приоритет (целое число).
 *   - Всё, что между ними, интерпретируется как одно "сырое" регулярное выражение (с сохранением всех пробелов).
 */
class TokenSpecReader final : public ITokenSpecReader {
public:
    TokenSpecReader() = default;
    ~TokenSpecReader() override = default;

    std::vector<TokenSpec> readTokenSpecs(const std::string& filePath) override;

private:
    /**
     * @brief Удаляет пробелы в начале и конце строки.
     */
    static std::string trim(const std::string& s);

    /**
     * @brief Удаляет пробелы только слева (начало строки).
     */
    static std::string trimLeft(const std::string& s);

    /**
     * @brief Удаляет пробелы только справа (конец строки).
     */
    static std::string rtrim(const std::string& s);

    /**
     * @brief Делит строку на "последний токен" и "остаток" (оба без trailing-пробелов).
     * Пример: splitOffLastToken("foo bar baz") -> {"baz", "foo bar"}.
     */
    static std::pair<std::string, std::string> splitOffLastToken(const std::string& s);

    /**
     * @brief Парсит флаг ignore (true/false/1/0) из строки, иначе выбрасывает исключение.
     * @param str То, что прочитали в конце строки.
     * @param wholeLine Исходная строка (для отладки).
     */
    bool parseIgnoreFlag(const std::string& str, const std::string& wholeLine) const;

    /**
     * @brief Парсит приоритет (int) из строки, иначе выбрасывает исключение.
     * @param str То, что прочитали в конце строки.
     * @param wholeLine Исходная строка (для отладки).
     */
    int parsePriority(const std::string& str, const std::string& wholeLine) const;

    /**
     * @brief Парсит одну строку спецификации, возвращая TokenSpec.
     * @throws std::runtime_error при ошибках.
     */
    TokenSpec parseLine(const std::string& line) const;
};
