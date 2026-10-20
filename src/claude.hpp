#pragma once

#include "provider.hpp"

// Asks Anthropic's OAuth usage endpoint, the same one /usage in Claude Code
// reads. It is undocumented and may change.
class ClaudeProvider : public Provider {
public:
    Report fetch() override;
};
