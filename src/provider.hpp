#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// One rate-limit window, e.g. Claude's 5-hour or Codex's weekly limit.
struct Window {
    std::string label;                                 // "5h", "7d", ...
    double used_pct = 0;                               // 0..100
    std::optional<std::chrono::sys_seconds> resets_at;
};

struct Report {
    std::string provider;   // "claude", "codex"
    bool ok = false;
    std::string error;      // set when !ok
    std::vector<Window> windows;
};

class Provider {
public:
    virtual ~Provider() = default;

    virtual std::string_view name() const = 0;

    // Read credentials, make the HTTP request, return the raw response body.
    // Throws std::runtime_error with a user-facing message on failure.
    virtual std::string fetch() = 0;

    // Turn a response body into windows. Kept separate from fetch() so the
    // cache can store raw bodies and parsing can be tested without a network.
    virtual std::vector<Window> parse(std::string_view body) const = 0;
};

class ClaudeProvider : public Provider {
public:
    std::string_view name() const override { return "claude"; }
    std::string fetch() override;
    std::vector<Window> parse(std::string_view body) const override;
};

class CodexProvider : public Provider {
public:
    std::string_view name() const override { return "codex"; }
    std::string fetch() override;
    std::vector<Window> parse(std::string_view body) const override;
};

struct HttpResponse {
    int status = 0;
    std::string body;
};

using Headers = std::vector<std::pair<std::string, std::string>>;

// Defined in fetch.cpp, the only file that includes httplib.h, which keeps
// the rest of the build fast. Throws std::runtime_error on connection or TLS
// failure. A non-2xx status is returned, not thrown.
HttpResponse https_get(const std::string& host, const std::string& path,
                       const Headers& headers);
