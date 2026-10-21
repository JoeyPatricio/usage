#include "provider.hpp"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include "json.hpp"

namespace {

struct CodexAuth {
    std::string access_token;
    std::string account_id;
};

CodexAuth read_auth() {
    std::filesystem::path dir;
    if (const char* codex_home = std::getenv("CODEX_HOME"); codex_home && *codex_home)
        dir = codex_home;
    else if (const char* home = std::getenv("HOME"))
        dir = std::filesystem::path(home) / ".codex";
    else
        throw std::runtime_error("HOME is not set");
    std::filesystem::path path = dir / "auth.json";

    std::ifstream file(path);
    if (!file) throw std::runtime_error("not logged in: can't read " + path.string());

    auto tokens = nlohmann::json::parse(file).at("tokens");
    return {tokens.at("access_token").get<std::string>(),
            tokens.at("account_id").get<std::string>()};
}

// 18000 -> "5h", 604800 -> "7d"
std::string window_label(std::int64_t seconds) {
    if (seconds % 86400 == 0) return std::to_string(seconds / 86400) + "d";
    if (seconds % 3600 == 0) return std::to_string(seconds / 3600) + "h";
    return std::to_string(seconds / 60) + "m";
}

}  // namespace

std::string CodexProvider::fetch() {
    CodexAuth auth = read_auth();
    HttpResponse res = https_get("chatgpt.com", "/backend-api/wham/usage",
                                 {{"Authorization", "Bearer " + auth.access_token},
                                  {"ChatGPT-Account-Id", auth.account_id}});
    if (res.status != 200)
        throw std::runtime_error("chatgpt.com returned HTTP " + std::to_string(res.status));
    return res.body;
}

std::vector<Window> CodexProvider::parse(std::string_view body) const {
    auto limits = nlohmann::json::parse(body).at("rate_limit");
    std::vector<Window> windows;
    for (const char* key : {"primary_window", "secondary_window"}) {
        auto it = limits.find(key);
        if (it == limits.end() || it->is_null()) continue;
        Window w;
        w.label = window_label(it->at("limit_window_seconds").get<std::int64_t>());
        w.used_pct = it->at("used_percent").get<double>();
        // reset_at is epoch seconds
        if (const auto& reset = it->at("reset_at"); !reset.is_null())
            w.resets_at = std::chrono::sys_seconds{std::chrono::seconds{reset.get<std::int64_t>()}};
        windows.push_back(std::move(w));
    }
    if (windows.empty()) throw std::runtime_error("no usage windows in response");
    return windows;
}
