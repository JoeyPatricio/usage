#pragma once

#include <ctime>
#include <string>
#include <vector>

// One rate-limit window, e.g. the 5 hour or the weekly limit.
struct Window {
    std::string label;          // "5h", "7d", ...
    double used_percent = 0;    // 0-100
    std::time_t resets_at = 0;  // unix seconds
};

// What a provider hands back to main. A failed fetch sets `error` and leaves
// `windows` empty, so one broken provider doesn't hide the others.
struct Report {
    std::string provider;
    std::vector<Window> windows;
    std::string error;

    bool ok() const { return error.empty(); }
};

class Provider {
public:
    virtual ~Provider() = default;
    virtual Report fetch() = 0;
};
