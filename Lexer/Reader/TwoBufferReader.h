#pragma once
#include "IReader.h"
#include <string>

class TwoBufferReader : public IReader {
private:
    FILE* m_file;
    size_t m_bufferSize;
    char* m_buffers[2]{};
    size_t m_fillSizes[2]{};
    int m_currentBufferIndex;
    size_t m_positionInBuffer;
    bool m_eof;
    int m_line;
    int m_column;

public:
    explicit TwoBufferReader(const std::string& filePath, size_t bufferSize = 4096);
    ~TwoBufferReader() override;
    char getChar() override;
    char peekChar(int offset) override;
    [[nodiscard]] bool isEOF() const override;
    [[nodiscard]] int getLine() const override;
    [[nodiscard]] int getColumn() const override;
private:
    void loadBuffer(int bufferIndex);
    [[nodiscard]] int nextBufferIndex() const { return 1 - m_currentBufferIndex; }
};
