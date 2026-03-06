#!/usr/bin/env bash
# Validate a subset of grammars in grammars/ using test-gbnf-validator and minimal valid inputs.
# Usage: ./scripts/validate-grammars.sh [build_dir]
#   build_dir  path to build dir containing bin/test-gbnf-validator (default: ./build)
# Exit: 0 if all validated, 1 if validator missing or any grammar fails.

set -e

BUILD="${1:-./build}"
VALIDATOR="${BUILD}/bin/test-gbnf-validator"
GRAMMARS_DIR="${BASH_SOURCE%/*}/../grammars"

if [[ ! -x "$VALIDATOR" ]]; then
    echo "Error: test-gbnf-validator not found at $VALIDATOR (build with -DCHEESE_BUILD_TESTS=ON)" >&2
    exit 1
fi
if [[ ! -d "$GRAMMARS_DIR" ]]; then
    echo "Error: grammars dir not found: $GRAMMARS_DIR" >&2
    exit 1
fi

run_one() {
    local grammar="$1"
    local input="$2"
    local tmp
    tmp=$(mktemp)
    echo -n "$input" > "$tmp"
    if "$VALIDATOR" "$GRAMMARS_DIR/$grammar" "$tmp" >/dev/null 2>&1; then
        echo "  OK $grammar"
        rm -f "$tmp"
        return 0
    else
        echo "  FAIL $grammar"
        "$VALIDATOR" "$GRAMMARS_DIR/$grammar" "$tmp" || true
        rm -f "$tmp"
        return 1
    fi
}

failed=0
echo "Validating grammars (minimal valid inputs)..."
run_one "json.gbnf" "{}" || failed=1
run_one "list.gbnf" "- a
" || failed=1
run_one "chess.gbnf" "1. e4 e5
2. Nf3 Nc6
" || failed=1

if [[ $failed -eq 0 ]]; then
    echo "All validated."
else
    echo "Some grammars failed validation."
    exit 1
fi
