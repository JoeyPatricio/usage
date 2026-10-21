#include "provider.hpp"

#include <chrono>
#include <optional>
#include <stdexcept>

namespace {

// TODO: parse an ISO 8601 timestamp like "2026-10-21T18:00:00.123+00:00".
// std::chrono::parse with "%FT%T%Ez" handles the offset. Drop fractional
// seconds first or parse into a finer duration and floor.
std::optional<std::chrono::sys_seconds> parse_timestamp(std::string_view s) {
    throw std::logic_error("todo: parse_timestamp");
}

// TODO: read ~/.claude/.credentials.json and return claudeAiOauth.accessToken.
// Throw if the file is missing or the token has expired (expiresAt is ms since epoch).
std::string read_token() {
    throw std::logic_error("todo: claude read_token");
}

}  // namespace

// TODO: GET https://api.anthropic.com/api/oauth/usage
//   Authorization: Bearer <token>
//   anthropic-beta: oauth-2025-04-20
// Non-200 responses should throw with the status code in the message.
std::string ClaudeProvider::fetch() {
    std::string token = read_token();
    throw std::logic_error("todo: ClaudeProvider::fetch");
}

// TODO: map five_hour / seven_day (and any other non-null window objects)
// to Window. utilization is already 0..100, resets_at is ISO 8601.
std::vector<Window> ClaudeProvider::parse(std::string_view body) const {
    throw std::logic_error("todo: ClaudeProvider::parse");
}
