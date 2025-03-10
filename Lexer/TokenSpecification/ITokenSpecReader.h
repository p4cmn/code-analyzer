#pragma once
#include "TokenSpec.h"
#include <string>
#include <vector>

class ITokenSpecReader {
public:
    virtual ~ITokenSpecReader() = default;
    virtual std::vector<TokenSpec> readTokenSpecs(const std::string& filePath) = 0;
};
