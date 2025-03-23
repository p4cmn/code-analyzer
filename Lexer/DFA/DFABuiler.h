#pragma once
#include "IDFABuilder.h"

/**
 * @brief Реализация IDFABuilder, использующая метод subset construction.
 */
class SubsetConstructionDFABuilder : public IDFABuilder {
public:
    SubsetConstructionDFABuilder() = default;
    ~SubsetConstructionDFABuilder() override = default;

    /**
     * @see IDFABuilder::buildFromNFA
     */
    DFA buildFromNFA(const NFA &nfa) override;
};
