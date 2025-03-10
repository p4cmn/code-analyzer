#pragma once
#include "IPreprocessor.h"

class GccPreprocessor : public IPreprocessor {
public:
    GccPreprocessor() = default;
    virtual ~GccPreprocessor() override = default;
    std::string preprocessFile(const std::string& filePath) override;
};
