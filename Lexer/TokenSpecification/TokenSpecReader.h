#pragma once
#include "ITokenSpecReader.h"

class TokenSpecReader : public ITokenSpecReader {
public:
    std::vector<TokenSpec> readTokenSpecs(const std::string& filePath) override;
};
