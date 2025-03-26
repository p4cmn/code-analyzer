#pragma once
#include <string>

/**
 * @brief Интерфейс препроцессора, который выполняет предобработку исходного кода.
 */
class IPreprocessor {
public:
    virtual ~IPreprocessor() = default;

    /**
     * @brief Выполняет пред обработку заданного файла, возвращая результирующий текст.
     * @param filePath Путь к исходному файлу.
     * @return Строка с результатом пред обработки.
     * @throws std::runtime_error В случае, если выполнить препроцессор не удалось.
     */
    virtual std::string preprocessFile(const std::string& filePath) = 0;
};
