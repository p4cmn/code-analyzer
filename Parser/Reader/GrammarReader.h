#pragma once

#include "IGrammarReader.h"
#include <string>

/**
 * @brief Конкретная реализация IGrammarReader, читающая грамматику из текстового файла
 *        со структурой вида:
 *
 *  # комментарий
 *  Terminals:
 *      + - * / ( ) ID NUM
 *
 *  NonTerminals:
 *      E T F
 *
 *  Start:
 *      E
 *
 *  Productions:
 *      E : E + T
 *      E : T
 *      T : T * F
 *      T : F
 *      F : ( E )
 *      F : ID
 *      F : NUM
 */
class GrammarReader final : public IGrammarReader {
public:
    GrammarReader() = default;
    ~GrammarReader() override = default;

    /**
     * @brief Считывает грамматику из файла по заданному пути.
     * @throws std::runtime_error При ошибках чтения или формата.
     */
    Grammar readGrammar(const std::string& filePath) override;

private:
    enum class Section {
        None,
        Terminals,
        NonTerminals,
        Start,
        Productions
    };

    /**
     * @brief Удаляет пробелы в начале и конце строки.
     */
    static std::string trim(const std::string& s);

    /**
     * @brief Обрабатывает строку секции "Terminals:".
     */
    void parseTerminalsLine(const std::string& line, Grammar& grammar);

    /**
     * @brief Обрабатывает строку секции "NonTerminals:".
     */
    void parseNonTerminalsLine(const std::string& line, Grammar& grammar);

    /**
     * @brief Обрабатывает строку секции "Start:".
     */
    void parseStartLine(const std::string& line, Grammar& grammar);

    /**
     * @brief Обрабатывает строку секции "Productions:" (вид "E : E + T").
     */
    void parseProductionLine(const std::string& line, Grammar& grammar);
};
