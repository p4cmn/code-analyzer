#pragma once
#include "TokenSpec.h"

#include <string>
#include <vector>

/**
 * @brief Интерфейс для загрузки спецификаций токенов из внешнего файла.
 */
class ITokenSpecReader {
public:
    virtual ~ITokenSpecReader() = default;

    /**
     * @brief Считывает все спецификации токенов из заданного файла.
     * @param filePath Путь к файлу спецификаций.
     * @return Вектор считанных спецификаций (TokenSpec).
     * @throws std::runtime_error В случае, если файл не удалось открыть или данные некорректны.
     */
    virtual std::vector<TokenSpec> readTokenSpecs(const std::string& filePath) = 0;
};
