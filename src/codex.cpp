#include "provider.hpp"

#include <stdexcept>

namespace {

struct CodexAuth {
    std::string access_token;
    std::string account_id;
};

// TODO: read ~/.codex/auth.json (or $CODEX_HOME/auth.json) and pull
// tokens.access_token and tokens.account_id.
CodexAuth read_auth() {
    throw std::logic_error("todo: codex read_auth");
}

}  // namespace

// TODO: GET https://chatgpt.com/backend-api/wham/usage
//   Authorization: Bearer <access_token>
//   ChatGPT-Account-Id: <account_id>
std::string CodexProvider::fetch() {
    CodexAuth auth = read_auth();
    throw std::logic_error("todo: CodexProvider::fetch");
}

// TODO: map rate_limit.primary_window / secondary_window to Window.
// Derive the label from limit_window_seconds, reset_at is epoch seconds.
std::vector<Window> CodexProvider::parse(std::string_view body) const {
    throw std::logic_error("todo: CodexProvider::parse");
}
