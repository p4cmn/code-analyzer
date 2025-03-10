#pragma once
#include <string>

class IPreprocessor {
public:
    virtual ~IPreprocessor() = default;
    virtual std::string preprocessFile(const std::string& filePath) = 0;
};
