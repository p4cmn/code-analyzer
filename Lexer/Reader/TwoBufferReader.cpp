#include "TwoBufferReader.h"
#include <stdexcept>
#include <cstring> // для std::memcpy в случае надобности

TwoBufferReader::TwoBufferReader(const std::string &filePath, size_t bufferSize)
        : m_file(nullptr),
          m_bufferSize(bufferSize),
          m_bufferData(nullptr),
          m_bufferStart(0),
          m_bufferCount(0),
          m_globalPos(0),
          m_eof(false),
          m_line(1),
          m_column(1) {
  m_file = std::fopen(filePath.c_str(), "r");
  if (!m_file) {
    throw std::runtime_error("Failed to open file: " + filePath);
  }
  m_bufferData = new char[m_bufferSize];
}

TwoBufferReader::~TwoBufferReader() {
  if (m_file) {
    std::fclose(m_file);
    m_file = nullptr;
  }
  delete[] m_bufferData;
}

void TwoBufferReader::loadBuffer(size_t newStart) {
  if (std::fseek(m_file, static_cast<long>(newStart), SEEK_SET) != 0) {
    m_bufferCount = 0;
    m_eof = true;
    return;
  }
  size_t bytesRead = std::fread(m_bufferData, 1, m_bufferSize, m_file);
  m_bufferStart  = newStart;
  m_bufferCount  = bytesRead;
  if (bytesRead == 0) {
    m_eof = true;
  }
}

/**
 * @brief Возвращает символ из глобальной позиции pos, не изменяя счётчиков строк/столбцов
 *        и не меняя m_globalPos. Если pos вне файла, вернёт '\0'. Если нужно, подгружает буфер.
 */
char TwoBufferReader::getCharAt(size_t pos) const
{
  if (m_eof && pos >= (m_bufferStart + m_bufferCount)) {
    return '\0';
  }
  if (pos < m_bufferStart || pos >= (m_bufferStart + m_bufferCount)) {
    auto self = const_cast<TwoBufferReader*>(this);
    self->loadBuffer(pos);
    if (m_bufferCount == 0 || pos < m_bufferStart || pos >= (m_bufferStart + m_bufferCount)) {
      return '\0';
    }
  }
  size_t offsetInBuf = pos - m_bufferStart;
  return m_bufferData[offsetInBuf];
}

/**
 * @brief Аналог getCharAt, но дополнительно обновляет m_line/m_column (как будто «прочитали» символ).
 *        m_globalPos НЕ сдвигаем.
 */
char TwoBufferReader::consumeCharAt(size_t pos)
{
  char c = getCharAt(pos);
  if (c == '\n') {
    m_line++;
    m_column = 1;
  } else if (c != '\0') {
    m_column++;
  }
  return c;
}

/**
 * @brief Возвращает следующий символ (по m_globalPos) и смещает m_globalPos на 1, обновляя line/column.
 */
char TwoBufferReader::getChar()
{
  if (m_eof) {
    return '\0';
  }
  char c = consumeCharAt(m_globalPos);
  if (c == '\0') {
    m_eof = true;
    return '\0';
  }
  m_globalPos++;
  return c;
}

/**
 * @brief Возвращает символ на расстоянии offset от m_globalPos, не продвигая m_globalPos,
 *        но обновлять line/column мы не должны (ведь это просто "подсмотреть").
 */
char TwoBufferReader::peekChar(int offset)
{
  if (offset < 0) {
    return '\0';
  }
  size_t pos = m_globalPos + static_cast<size_t>(offset);
  return getCharAt(pos);
}
