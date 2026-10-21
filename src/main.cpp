#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <future>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

#include "provider.hpp"

struct Options {
    bool short_out = false;
    bool json_out = false;
    bool refresh = false;
};

static Options parse_args(int argc, char** argv);
static void print_help();
static Report fetch_report(Provider& p, bool refresh);
static void render_table(const std::vector<Report>& reports);
static void render_short(const std::vector<Report>& reports);
static void render_json(const std::vector<Report>& reports);
static bool any_ok(const std::vector<Report>& reports);
static std::string format_duration(std::chrono::seconds d);

int main(int argc, char** argv) {

    Options opt = parse_args(argc, argv);

    std::vector<std::unique_ptr<Provider>> providers;
    providers.push_back(std::make_unique<ClaudeProvider>());
    providers.push_back(std::make_unique<CodexProvider>());

    // One task per provider, all running at once
    std::vector<std::future<Report>> futures;
    for (auto& p : providers) {
        futures.push_back(std::async(std::launch::async,
            [&p, refresh = opt.refresh] { return fetch_report(*p, refresh); }));
    }

    // Collect in the original order so output order is fixed
    std::vector<Report> reports;
    for (auto& f : futures) reports.push_back(f.get());

    if      (opt.json_out)  render_json(reports);
    else if (opt.short_out) render_short(reports);
    else                    render_table(reports);

    return any_ok(reports) ? 0 : 1;
}


static Options parse_args(int argc, char** argv) {
    Options opt;
    for (int i = 1; i < argc; i++) {
       std::string_view a = argv[i];
       if       (a == "--short" || a == "-s")   opt.short_out = true;
       else if  (a == "--json"  || a == "-j")    opt.json_out = true;
       else if  (a == "--refresh" || a == "-r") opt.refresh = true;
       else if  (a == "--help"  || a == "-h") { print_help(); std::exit(0); }
       else {
           std::cerr << "unknown flag: " << a << "\n";
           std::exit(2);
       }
    }
    if (opt.short_out && opt.json_out) {
        std::cerr << "--short and --json can't be used together\n";
        std::exit(2);
    }
    return opt;
}


static void print_help() {
    std::cout <<
        "usage - show LLM subscription limit usage\n\n"
        "  -s, --short    one line for status bars\n"
        "  -j, --json     machine-readable output\n"
        "  -r, --refresh  skip the cache\n"
        "  -h, --help     show this help\n";
}


// Raw response bodies live in $XDG_CACHE_HOME/usage/<provider>.json
// (falling back to ~/.cache/usage). The file's mtime is the fetch time.
constexpr std::chrono::seconds kCacheMaxAge{60};


// TODO: return the cached body if the file exists and is younger than max_age.
static std::optional<std::string> cache_read(std::string_view provider,
                                             std::chrono::seconds max_age) {
    throw std::logic_error("todo: cache_read");
}


// TODO: create the cache dir, write to a temp file, rename over the target.
static void cache_write(std::string_view provider, std::string_view body) {
    throw std::logic_error("todo: cache_write");
}


// Never throws: failures land in Report::error.
static Report fetch_report(Provider& p, bool refresh) {
    Report r;
    r.provider = p.name();
    try {
        std::optional<std::string> body;
        if (!refresh) body = cache_read(p.name(), kCacheMaxAge);
        bool fresh = !body;
        if (fresh) body = p.fetch();
        r.windows = p.parse(*body);
        if (fresh) cache_write(p.name(), *body);
        r.ok = true;
    } catch (const std::exception& e) {
        r.error = e.what();
    }
    return r;
}


// One block per provider, one row per window:
//   claude  5h   42%  resets in 2h 13m
//           7d   18%  resets in 4d 3h
//   codex   error: not logged in
static void render_table(const std::vector<Report>& reports) {
    auto now = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
    for (const auto& r : reports) {
        if (!r.ok) {
            std::cout << std::format("{:<8}error: {}\n", r.provider, r.error);
            continue;
        }
        std::string_view name = r.provider;
        for (const auto& w : r.windows) {
            std::cout << std::format("{:<8}{:<5}{:3.0f}%", name, w.label, w.used_pct);
            // A cached body can hold a reset time that has already passed
            if (w.resets_at)
                std::cout << "  resets in "
                          << format_duration(std::max(*w.resets_at - now, std::chrono::seconds{0}));
            std::cout << "\n";
            name = "";
        }
    }
}


// TODO: one line, e.g. "claude 42%/18% codex 7%/30%"
static void render_short(const std::vector<Report>& reports) {
    throw std::logic_error("todo: render_short");
}


// TODO: array of reports, resets_at as epoch seconds or null.
static void render_json(const std::vector<Report>& reports) {
    throw std::logic_error("todo: render_json");
}


static bool any_ok(const std::vector<Report>& reports) {
    return std::ranges::any_of(reports, &Report::ok);
}


// "2h 13m", "4d 3h", "45s". At most the two largest nonzero units.
static std::string format_duration(std::chrono::seconds d) {
    long long s = d.count();
    const std::pair<long long, char> units[] = {
        {s / 86400, 'd'}, {s / 3600 % 24, 'h'}, {s / 60 % 60, 'm'}, {s % 60, 's'}};
    std::string out;
    int shown = 0;
    for (auto [n, unit] : units) {
        if (n == 0 || shown == 2) continue;
        if (shown++) out += ' ';
        out += std::to_string(n) + unit;
    }
    return out.empty() ? "0s" : out;
}
