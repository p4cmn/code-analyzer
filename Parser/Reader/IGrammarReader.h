#pragma once
#include "../Grammar/Grammar.h"

#include <string>

/**
 * @brief Интерфейс для чтения грамматики из текстового файла.
 */
class IGrammarReader {
public:
    virtual ~IGrammarReader() = default;

    /**
     * @brief Считывает грамматику из указанного файла.
     * @param filePath Путь к файлу с описанием грамматики.
     * @return Объект Grammar со списками терминалов, нетерминалов, стартовым символом и продукциями.
     * @throws std::runtime_error Если файл не удалось открыть или данные некорректны.
     */
    virtual Grammar readGrammar(const std::string& filePath) = 0;
};
