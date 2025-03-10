#include "TwoBufferReader.h"
#include <cstdio>
#include <cstring>
#include <stdexcept>

TwoBufferReader::TwoBufferReader(const std::string& filePath, size_t bufferSize)
        : m_file(nullptr), m_bufferSize(bufferSize),
          m_currentBufferIndex(0), m_positionInBuffer(0),
          m_eof(false), m_line(1), m_column(1) {
  m_file = std::fopen(filePath.c_str(), "r");
  if (!m_file) {
    throw std::runtime_error("Не удалось открыть файл: " + filePath);
  }
  m_buffers[0] = new char[m_bufferSize];
  m_buffers[1] = new char[m_bufferSize];
  m_fillSizes[0] = 0;
  m_fillSizes[1] = 0;
  loadBuffer(m_currentBufferIndex);
}

TwoBufferReader::~TwoBufferReader() {
  if (m_file) {
    std::fclose(m_file);
    m_file = nullptr;
  }
  delete[] m_buffers[0];
  delete[] m_buffers[1];
}

void TwoBufferReader::loadBuffer(int bufferIndex) {
  if (!m_file) {
    m_fillSizes[bufferIndex] = 0;
    return;
  }
  size_t bytesRead = std::fread(m_buffers[bufferIndex], 1, m_bufferSize, m_file);
  m_fillSizes[bufferIndex] = bytesRead;
  if (bytesRead < m_bufferSize) {
    if (std::feof(m_file)) {
    }
  }
}

char TwoBufferReader::getChar() {
  if (m_eof) return '\0';
  if (m_positionInBuffer >= m_fillSizes[m_currentBufferIndex]) {
    m_currentBufferIndex = nextBufferIndex();
    loadBuffer(m_currentBufferIndex);
    m_positionInBuffer = 0;
    if (m_fillSizes[m_currentBufferIndex] == 0) {
      m_eof = true;
      return '\0';
    }
  }
  char c = m_buffers[m_currentBufferIndex][m_positionInBuffer++];
  if (c == '\n') {
    m_line++;
    m_column = 1;
  } else {
    m_column++;
  }
  return c;
}

char TwoBufferReader::peekChar(int offset) {
  if (m_eof) return '\0';
  size_t pos = m_positionInBuffer + offset;
  if (pos < m_fillSizes[m_currentBufferIndex]) {
    return m_buffers[m_currentBufferIndex][pos];
  } else {
    size_t remaining = m_fillSizes[m_currentBufferIndex] - m_positionInBuffer;
    int nextBuf = nextBufferIndex();
    if (offset >= remaining && offset - remaining < m_fillSizes[nextBuf]) {
      return m_buffers[nextBuf][offset - remaining];
    }
  }
  return '\0';
}

bool TwoBufferReader::isEOF() const {
  return m_eof;
}

int TwoBufferReader::getLine() const {
  return m_line;
}

int TwoBufferReader::getColumn() const {
  return m_column;
}
