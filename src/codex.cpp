#include "codex.hpp"

#include <filesystem>
#include <optional>
#include <string>

namespace fs = std::filesystem;

// Codex appends events to ~/.codex/sessions/YYYY/MM/DD/rollout-*.jsonl.
// "token_count" events carry a snapshot of your limits, for example:
//
//   "rate_limits":{"limit_id":"codex",
//     "primary":  {"used_percent":83.0,"window_minutes":300,  "resets_at":1792204638},
//     "secondary":{"used_percent":44.0,"window_minutes":10080,"resets_at":1792680680}}
//
// primary is the 5 hour window, secondary is the weekly one.

// Returns the most recently modified rollout-*.jsonl under `dir`, or nothing
// if there are none.
static std::optional<fs::path> newest_rollout(const fs::path& dir) {
    // TODO: walk `dir` with fs::recursive_directory_iterator and keep the
    // regular file with the largest fs::last_write_time.
    (void)dir;
    return std::nullopt;
}

// Returns the last line in `file` that contains "rate_limits", or nothing.
static std::optional<std::string> last_rate_limits_line(const fs::path& file) {
    // TODO: read with std::ifstream + std::getline. Reading forward and keeping
    // the last match is fine, these files are small.
    (void)file;
    return std::nullopt;
}

Report CodexProvider::fetch() {
    Report r;
    r.provider = "codex";

    // TODO: build the path from std::getenv("HOME"). Set r.error and return if
    // HOME isn't set.
    fs::path sessions = "/dev/null";

    auto file = newest_rollout(sessions);
    if (!file) {
        r.error = "no codex session logs found";
        return r;
    }

    auto line = last_rate_limits_line(*file);
    if (!line) {
        r.error = "no rate_limits in " + file->filename().string();
        return r;
    }

    // TODO: parse *line as JSON and push a Window for "primary" and
    // "secondary". Derive the label from window_minutes (300 -> "5h",
    // 10080 -> "7d") instead of hardcoding it.
    r.error = "not implemented";
    return r;
}
