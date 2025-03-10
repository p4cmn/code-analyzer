// IReader.h
#pragma once

class IReader {
public:
    virtual ~IReader() = default;
    virtual char getChar() = 0;
    virtual char peekChar(int offset) = 0;
    [[nodiscard]] virtual bool isEOF() const = 0;
    [[nodiscard]] virtual int getLine() const = 0;
    [[nodiscard]] virtual int getColumn() const = 0;
};
