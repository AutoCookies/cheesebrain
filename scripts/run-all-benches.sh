#!/usr/bin/env bash
# Run cheese-bench and cheese-batched-bench. Requires a GGUF model.
# Usage: ./scripts/run-all-benches.sh [model.gguf] [build_dir]
#   model.gguf  path to GGUF model (default: models/qwen0.5b.gguf)
#   build_dir   path to build dir with bin/ (default: ./build)

set -e

MODEL="${1:-./models/qwen0.5b.gguf}"
BUILD="${2:-./build}"
BIN="${BUILD}/bin"

if [[ ! -f "$MODEL" ]]; then
    echo "Error: model file not found: $MODEL" >&2
    echo "Place a GGUF model at models/qwen0.5b.gguf or pass the path as the first argument." >&2
    exit 1
fi
if [[ ! -x "${BIN}/cheese-bench" ]]; then
    echo "Error: cheese-bench not found at ${BIN}/cheese-bench" >&2
    exit 1
fi
if [[ ! -x "${BIN}/cheese-batched-bench" ]]; then
    echo "Error: cheese-batched-bench not found at ${BIN}/cheese-batched-bench" >&2
    exit 1
fi

echo "=== cheese-bench ==="
"${BIN}/cheese-bench" -m "$MODEL" -r 1 -p 128 -n 32 --no-warmup -o md

echo ""
echo "=== cheese-batched-bench ==="
"${BIN}/cheese-batched-bench" -m "$MODEL" -c 256 -b 64 -ub 32 -npp 64 -ntg 32 -npl 1

echo ""
echo "All benches completed successfully."
