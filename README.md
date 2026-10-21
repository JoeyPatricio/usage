# usage

Shows how much of your Claude and Codex subscription limits you've used, from the command line.

```
$ usage
claude  5h    15%  resets in 2h 13m
        7d    15%  resets in 4d 3h
codex   5h     6%  resets in 1h 40m
        7d     4%  resets in 5d 2h
$ usage -s
claude 15%/15% · codex 6%/4%
```

It reads the login tokens that Claude Code and Codex already store locally, so you need to be logged in to each with `claude` and `codex`. Responses are cached for 60 seconds in `~/.cache/usage`.

## Setup

Needs GCC 14+ (for C++20 `<format>` and `std::chrono::parse`) and OpenSSL.

Build it from the repo root. This produces `build/usage`.

```
make
```

To run it as `usage` from anywhere, link it into a directory on your `PATH`. `~/.local/bin` is a common choice.

```
mkdir -p ~/.local/bin
ln -s "$PWD/build/usage" ~/.local/bin/usage
```

If `~/.local/bin` isn't on your `PATH` yet, add this to `~/.bashrc` or `~/.zshrc` and open a new terminal.

```
export PATH="$HOME/.local/bin:$PATH"
```

Because it's a symlink, rerunning `make` updates the command. After `make clean`, run `make` again before using it.

## Options

```
-s, --short    one line for status bars
-j, --json     machine-readable output
-r, --refresh  skip the cache
-h, --help     show this help
```
