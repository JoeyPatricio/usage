#include <iostream>
#include <cstdlib>
#include <future>
#include <memory>
#include <string_view>
#include <vector>

#include "cache.hpp"
#include "claude.hpp"
#include "codex.hpp"
#include "http_curl.hpp"
#include "provider.hpp"
#include "render.hpp"

struct Options {
    bool no_flag = false;
    bool short_out = false;
    bool json_out = false;
    bool refresh = false;
};

Options pars_args(int argc, char** argv);
void print_help();

int main(int argc, char** argv) {

    Options opt = pars_args(argc, argv);

    CurlHttpClient http;

    std::vector<std::unique_ptr<Provider>> providers;
    providers.push_back(std::make_unique<ClaudeProvider>(http));
    providers.push_back(std::make_unique<CodexProvider>());

    // One task per provider, all running at once
    std::vector<std::future<Report>> futures;
    for (auto& p : providers)
        Provider* prov = p.get();
        futures.push_back(std::async(std::launch::async,
            [&p, refresh = opt.refresh] { return fetch_report(*p, refresh); }));
    }

    // Collect in the original order so output order is fixed
    std::vector<Report> reports;
    for (auto& f : futures) reports.push_back(f.get());

    if      (opt.json)      render_json(reports);
    else if (opt.short_out) render_short(reports);
    else                    render_table(reports);

    return any_ok(reports) ? 0 : 1;
}


Options pars_args(int argc, char** argv) {
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
    if (opt.short_out && opt.json) {
        std::cerr << "--short and --json can't be used together\n";
        std::exit(2);
    }
    if (argc == 1) {
        opt.no_flag = true;
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

