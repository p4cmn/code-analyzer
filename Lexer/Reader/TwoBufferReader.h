#pragma once
#include "IReader.h"

#include <string>
#include <cstdio>

/**
 * @brief Класс для посимвольного чтения файла с использованием «двух буферов» поочередно,
 *        но в коде ниже это фактически один массив, который мы перезагружаем кусками;
 *        логика "двух буферов" / double buffering скрыта внутри.
 *
 * Особенности:
 *  - m_globalPos: абсолютная позиция от начала файла (какой символ следующий).
 *  - m_bufferStart: с какого глобального байта мы загрузили текущую партию.
 *  - m_bufferData: сюда грузим до bufferSize байт из файла (как «один» буфер).
 *  - m_bufferCount: сколько реальных байт загружено (меньше или равно bufferSize).
 *  - метод loadBuffer(newStart) загружает кусок из файла, начиная с байта newStart.
 *  - getChar()/peekChar(offset) обращаются к функции getCharAt(pos), которая при необходимости подгружает новый кусок.
 *
 *  Благодаря этому, тесты на peekChar(0) и т. д. будут корректно работать на стыках.
 */
class TwoBufferReader : public IReader {
public:
    /**
     * @brief Создает ридер, читающий указанный файл с использованием буфера.
     * @param filePath Путь к файлу.
     * @param bufferSize Размер внутреннего буфера, по умолчанию 4096 байт.
     * @throws std::runtime_error Если файл не удалось открыть.
     */
    explicit TwoBufferReader(const std::string &filePath, size_t bufferSize = 4096);

    /**
     * @brief Закрывает файл и освобождает буфер.
     */
    ~TwoBufferReader() override;

    char getChar() override;
    char peekChar(int offset) override;
    [[nodiscard]] bool isEOF() const override { return m_eof; }
    [[nodiscard]] int getLine() const override { return m_line; }
    [[nodiscard]] int getColumn() const override { return m_column; }

private:
    FILE* m_file;
    const size_t m_bufferSize;
    char*  m_bufferData;
    size_t m_bufferStart;
    size_t m_bufferCount;
    size_t m_globalPos;
    bool   m_eof;
    int    m_line;
    int    m_column;

    /**
     * @brief Загружает в m_bufferData кусок файла, начиная с байта newStart (глобальной позиции).
     *        Заполняет m_bufferData[0..m_bufferCount-1].
     *        Если ничего не прочитали, выставляет m_eof=true.
     */
    void loadBuffer(size_t newStart);

    /**
     * @brief Доступ к символу в глобальной позиции pos (не продвигая m_globalPos).
     *        Если pos выходит за границы текущего загруженного буфера, buffer перезагружается.
     *        Если файл закончился, вернёт '\0'.
     */
    char getCharAt(size_t pos) const;

    /**
     * @brief Служебный метод для «не константного» доступа (увеличения m_line, m_col),
     *        но не сдвигая m_globalPos. Обычно используется внутри getChar().
     */
    char consumeCharAt(size_t pos);
};

