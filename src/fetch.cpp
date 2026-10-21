#include "provider.hpp"

#include <stdexcept>

#include "httplib.h"

// TODO: build an httplib::SSLClient for host, set connect/read timeouts,
// copy headers into httplib::Headers, call Get(path). If the result is
// falsy, throw with httplib::to_string(res.error()).
HttpResponse https_get(const std::string& host, const std::string& path,
                       const Headers& headers) {
    throw std::logic_error("todo: https_get");
}
