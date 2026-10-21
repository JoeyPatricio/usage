#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <future>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

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
// TODO: unless refresh, try cache_read(p.name(), kCacheMaxAge). Otherwise call
// p.fetch(). Parse, and only cache_write a body that parsed. Catch
// std::exception and store e.what() in Report::error.
static Report fetch_report(Provider& p, bool refresh) {
    throw std::logic_error("todo: fetch_report");
}


// TODO: one block per provider, one row per window:
//   claude  5h   42%  resets in 2h 13m
//           7d   18%  resets in 4d 3h
//   codex   error: not logged in
static void render_table(const std::vector<Report>& reports) {
    throw std::logic_error("todo: render_table");
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
    throw std::logic_error("todo: any_ok");
}


// TODO: "2h 13m", "4d 3h", "45s". At most the two largest nonzero units.
static std::string format_duration(std::chrono::seconds d) {
    throw std::logic_error("todo: format_duration");
}
