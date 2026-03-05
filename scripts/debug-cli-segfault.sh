#!/bin/bash
# Run cheese-cli under GDB to capture backtrace for --simple-io segfault.
# Run from repo root (cheesebrain/) or from build/ - script detects location.

set -e
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$REPO_ROOT/build"
CLI_BIN="$BUILD_DIR/bin/cheese-cli"

if [[ ! -x "$CLI_BIN" ]]; then
  echo "Binary not found: $CLI_BIN"
  echo "Build first: cd $BUILD_DIR && make cheese-cli"
  exit 1
fi

echo "Running: gdb --args $CLI_BIN --simple-io"
echo ""
echo "In GDB, run:  run"
echo "After crash: bt full"
echo "Then:        quit"
echo ""
exec gdb --args "$CLI_BIN" --simple-io
