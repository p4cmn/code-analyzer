#pragma once
#include <string>

/**
 * @brief Интерфейс для работы с таблицей символов, сопоставляющей строкам уникальные IDs.
 */
class ISymbolTable {
public:
    virtual ~ISymbolTable() = default;

    /**
     * @brief Добавляет символ в таблицу, если он отсутствует, либо возвращает уже существующий ID.
     * @param symbol Строка-символ, который нужно добавить/получить.
     * @return Целочисленный ID данного символа в таблице.
     */
    virtual int addSymbol(const std::string& symbol) = 0;

    /**
     * @brief Производит поиск символа в таблице.
     * @param symbol Строка-символ, который нужно отыскать.
     * @return ID символа, или -1, если символ не найден.
     */
    [[nodiscard]] virtual int lookup(const std::string& symbol) const = 0;
};
