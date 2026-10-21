#include "provider.hpp"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "json.hpp"

namespace {

// ISO 8601 with an offset, e.g. "2026-10-21T18:00:00.123456+00:00".
std::chrono::sys_seconds parse_timestamp(std::string_view s) {
    // Parsing into a finer duration misreads fractions longer than its
    // precision without failing, so drop the fraction entirely.
    std::string t(s);
    if (auto dot = t.find('.'); dot != std::string::npos)
        t.erase(dot, t.find_first_of("+-Z", dot) - dot);

    std::istringstream in(t);
    std::chrono::sys_seconds result;
    in >> std::chrono::parse("%FT%T%Ez", result);
    if (!in) throw std::runtime_error("bad timestamp: " + std::string(s));
    return result;
}

// Claude Code refreshes the token when it runs. We only read it.
std::string read_token() {
    const char* home = std::getenv("HOME");
    if (!home) throw std::runtime_error("HOME is not set");
    std::filesystem::path path = std::filesystem::path(home) / ".claude/.credentials.json";

    std::ifstream file(path);
    if (!file) throw std::runtime_error("not logged in: can't read " + path.string());

    auto oauth = nlohmann::json::parse(file).at("claudeAiOauth");
    // expiresAt is ms since epoch
    std::chrono::sys_time<std::chrono::milliseconds> expires{
        std::chrono::milliseconds{oauth.at("expiresAt").get<std::int64_t>()}};
    if (expires <= std::chrono::system_clock::now())
        throw std::runtime_error("token expired, run claude to refresh it");

    return oauth.at("accessToken").get<std::string>();
}

}  // namespace

std::string ClaudeProvider::fetch() {
    std::string token = read_token();
    HttpResponse res = https_get("api.anthropic.com", "/api/oauth/usage",
                                 {{"Authorization", "Bearer " + token},
                                  {"anthropic-beta", "oauth-2025-04-20"}});
    if (res.status != 200)
        throw std::runtime_error("api.anthropic.com returned HTTP " + std::to_string(res.status));
    return res.body;
}

std::vector<Window> ClaudeProvider::parse(std::string_view body) const {
    // The response also has codenamed entries (dollar limits, experiments)
    // that come and go, so only show windows with a known meaning.
    static const std::pair<const char*, const char*> kWindows[] = {
        {"five_hour", "5h"}, {"seven_day", "7d"},
        {"seven_day_opus", "opus"}, {"seven_day_sonnet", "sonnet"}};

    auto json = nlohmann::json::parse(body);
    std::vector<Window> windows;
    for (auto [key, label] : kWindows) {
        auto it = json.find(key);
        if (it == json.end() || it->is_null()) continue;
        Window w;
        w.label = label;
        w.used_pct = it->at("utilization").get<double>();
        if (const auto& reset = it->at("resets_at"); !reset.is_null())
            w.resets_at = parse_timestamp(reset.get<std::string>());
        windows.push_back(std::move(w));
    }
    if (windows.empty()) throw std::runtime_error("no usage windows in response");
    return windows;
}
