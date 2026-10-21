#include "provider.hpp"

#include <stdexcept>

#include "httplib.h"

HttpResponse https_get(const std::string& host, const std::string& path,
                       const Headers& headers) {
    httplib::SSLClient cli(host);
    cli.set_connection_timeout(10);
    cli.set_read_timeout(10);

    if (auto res = cli.Get(path, httplib::Headers(headers.begin(), headers.end()))) {
        return {res->status, res->body};
    } else {
        throw std::runtime_error(host + ": " + httplib::to_string(res.error()));
    }
}
