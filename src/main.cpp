#include <iostream>
#include <cstdlib>
#include <future>
#include <memory>
#include <string_view>
#include <vector>

#include "claude.hpp"
#include "codex.hpp"
#include "provider.hpp"

struct Options {
    bool short_out = false;
    bool json_out = false;
    bool refresh = false;
};

Options parse_args(int argc, char** argv);
void print_help();
void render_table(const std::vector<Report>& reports);
void render_short(const std::vector<Report>& reports);
void render_json(const std::vector<Report>& reports);

int main(int argc, char** argv) {
    Options opt = parse_args(argc, argv);

    std::vector<std::unique_ptr<Provider>> providers;
    providers.push_back(std::make_unique<ClaudeProvider>());
    providers.push_back(std::make_unique<CodexProvider>());

    // One task per provider, all running at once
    std::vector<std::future<Report>> futures;
    for (auto& p : providers) {
        futures.push_back(std::async(std::launch::async,
            [&p] { return p->fetch(); }));
    }

    // Collect in the original order so output order is fixed
    std::vector<Report> reports;
    for (auto& f : futures) reports.push_back(f.get());

    if      (opt.json_out)  render_json(reports);
    else if (opt.short_out) render_short(reports);
    else                    render_table(reports);

    for (const auto& r : reports)
        if (r.ok()) return 0;
    return 1;
}


Options parse_args(int argc, char** argv) {
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


void print_help() {
    std::cout <<
        "usage - show LLM subscription limit usage\n\n"
        "  -s, --short    one line for status bars\n"
        "  -j, --json     machine-readable output\n"
        "  -r, --refresh  skip the cache\n"
        "  -h, --help     show this help\n";
}


// Default output, e.g.
//   claude  5h   12%  resets 3h 20m
//           7d   40%  resets 4d 2h
//   codex   error: no codex session logs found
void render_table(const std::vector<Report>& reports) {
    // TODO: format resets_at as time remaining (resets_at - std::time(nullptr)).
    for (const auto& r : reports) {
        if (!r.ok()) {
            std::cout << r.provider << "  error: " << r.error << "\n";
            continue;
        }
        for (const auto& w : r.windows)
            std::cout << r.provider << "  " << w.label << "  "
                      << w.used_percent << "%\n";
    }
}

// One line for status bars, e.g. "claude 12%/40% codex 83%/44%"
void render_short(const std::vector<Report>& reports) {
    // TODO
    (void)reports;
}

// Machine-readable output. Write it with the same JSON library you use for
// parsing so escaping is handled for you.
void render_json(const std::vector<Report>& reports) {
    // TODO
    (void)reports;
}
