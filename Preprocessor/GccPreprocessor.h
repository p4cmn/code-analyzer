#pragma once
#include "IPreprocessor.h"

#include <string>

/**
 * @brief Конкретная реализация IPreprocessor, использующая системный вызов gcc -E -P.
 */
class GccPreprocessor final : public IPreprocessor {
public:
    GccPreprocessor() = default;
    ~GccPreprocessor() override = default;

    /**
     * @brief Запускает GCC-препроцессор для указанного файла и возвращает результат.
     * @param filePath Путь к исходному файлу.
     * @return Строка с результатом пред обработки.
     * @throws std::runtime_error Если команда gcc завершилась с ошибкой или не смогла запуститься.
     */
    std::string preprocessFile(const std::string& filePath) override;

private:
    /**
     * @brief Формирует команду для системного вызова gcc -E -P <file>.
     * @param filePath Путь к исходному файлу.
     * @return Строка-команда для выполнения.
     */
    [[nodiscard]] std::string buildCommand(const std::string& filePath) const;

    /**
     * @brief Запускает системную команду, считывая вывод построчно.
     * @param command Полная команда для выполнения.
     * @return Содержимое stdout, полученное от запущенного процесса.
     * @throws std::runtime_error Если popen/pclose завершаются неудачей.
     */
    [[nodiscard]] std::string executeCommand(const std::string& command) const;

    /**
     * @brief Удаляет из результата построчные директивы вида `# ...`, чтобы сохранить “чистый” текст.
     * @param input Исходный текст с потенциальными строками-директивами.
     * @return Тот же текст, но без строк, начинающихся на '#'.
     */
    [[nodiscard]] std::string filterPreprocessorDirectives(const std::string& input) const;
};
