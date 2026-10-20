#pragma once

#include "provider.hpp"

// Reads the rate limits Codex records in its local session logs.
class CodexProvider : public Provider {
public:
    Report fetch() override;
};
