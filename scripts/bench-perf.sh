#!/usr/bin/env bash
# Quick inference speed benchmark using cheese-cli.
# Usage: ./scripts/bench-perf.sh <model.gguf> [cheese-cli path]
#   model.gguf   path to GGUF model
#   cheese-cli   optional path to cheese-cli (default: ./build/bin/cheese-cli)
# Output: runs CLI with -n 1000 --perf and prints Prompt t/s and Generation t/s.
# For full benchmarking options see docs/development/benchmarking.md.

set -e

MODEL="${1:?Usage: $0 <model.gguf> [cheese-cli path]}"
CLI="${2:-./build/bin/cheese-cli}"
N_TOKENS="${BENCH_PERF_N:-1000}"
PROMPT="${BENCH_PERF_PROMPT:-Repeat the following word ten times: hello}"

if [[ ! -f "$MODEL" ]]; then
    echo "Error: model file not found: $MODEL" >&2
    exit 1
fi
if [[ ! -x "$CLI" ]]; then
    echo "Error: cheese-cli not found or not executable: $CLI" >&2
    exit 1
fi

# Run with --perf and --single-turn so we get one prompt, N tokens, then exit.
OUT=$("$CLI" -m "$MODEL" -p "$PROMPT" -n "$N_TOKENS" --perf --single-turn --no-warmup 2>&1 || true)

# Parse last line containing "Prompt:" and "Generation:" and "t/s"
if echo "$OUT" | grep -q "Prompt:.*t/s.*Generation:.*t/s"; then
    echo "$OUT" | grep "Prompt:.*t/s.*Generation:.*t/s" | tail -1
else
    echo "Benchmark output (no t/s line found):" >&2
    echo "$OUT" >&2
    exit 1
fi
