# Betfair Stream API Client

## Building

### Without Nix

```bash
cmake -B build
cmake --build build
```

Requires Boost.

### With Nix

```bash
nix build
```

## Running

If using Nix:

```bash
nix run
```

Requires `config.ini` file, like so:

```ini
[handler]
app_key=key
session_token=token
market_id=1.227301258
```
