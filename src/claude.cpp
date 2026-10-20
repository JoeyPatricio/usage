#include "claude.hpp"

#include <optional>
#include <string>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

// Request:
//   GET https://api.anthropic.com/api/oauth/usage
//   Authorization: Bearer <accessToken>
//   anthropic-beta: oauth-2025-04-20
//
// Expected response (verify with curl before relying on it):
//   {"five_hour":{"utilization":12.0,"resets_at":"2026-10-19T23:00:00Z"},
//    "seven_day":{"utilization":40.0,"resets_at":"..."}, ...}
//
// resets_at is an ISO 8601 string here, unlike Codex's unix seconds.

struct Credentials {
    std::string access_token;
    long long expires_at_ms = 0;
};

// Reads claudeAiOauth.{accessToken,expiresAt} from ~/.claude/.credentials.json.
// Claude Code owns this file. Only read it, never write it.
static std::optional<Credentials> read_credentials() {
    // TODO: open the file, parse JSON, fill Credentials.
    return std::nullopt;
}

Report ClaudeProvider::fetch() {
    Report r;
    r.provider = "claude";

    auto creds = read_credentials();
    if (!creds) {
        r.error = "no credentials, log in with claude";
        return r;
    }

    // TODO: if expires_at_ms is in the past, set
    // r.error = "token expired, run claude once to refresh" and return.

    // TODO: make the request:
    //   httplib::Client cli("https://api.anthropic.com");
    //   httplib::Headers h = {{"Authorization", "Bearer " + ...},
    //                         {"anthropic-beta", "oauth-2025-04-20"}};
    //   auto res = cli.Get("/api/oauth/usage", h);
    // No `res` means a network error (httplib::to_string(res.error())).
    // res->status 401 means the token is bad or expired.

    // TODO: parse res->body and push a Window for five_hour ("5h") and
    // seven_day ("7d").
    r.error = "not implemented";
    return r;
}
