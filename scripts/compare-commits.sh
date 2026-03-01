#!/usr/bin/env bash

if [ $# -lt 2 ]; then
    echo "usage: ./scripts/compare-commits.sh <commit1> <commit2> [tool] [additional arguments]"
    echo "  tool: 'cheese-bench' (default) or 'test-backend-ops'"
    echo "  additional arguments: passed to the selected tool"
    exit 1
fi

set -e
set -x

# Parse arguments
commit1=$1
commit2=$2
tool=${3:-cheese-bench}
additional_args="${@:4}"

# Validate tool argument
if [ "$tool" != "cheese-bench" ] && [ "$tool" != "test-backend-ops" ]; then
    echo "Error: tool must be 'cheese-bench' or 'test-backend-ops'"
    exit 1
fi

# verify at the start that the compare script has all the necessary dependencies installed
./scripts/compare-cheese-bench.py --check

if ! command -v sqlite3 >/dev/null 2>&1; then
    echo "Error: sqlite3 is not installed or not in PATH"
    echo "Please install sqlite3 to use this script"
    exit 1
fi

if [ "$tool" = "cheese-bench" ]; then
    db_file="cheese-bench.sqlite"
    target="cheese-bench"
    run_args="-o sql -oe md $additional_args"
else  # test-backend-ops
    db_file="test-backend-ops.sqlite"
    target="test-backend-ops"
    run_args="perf --output sql $additional_args"
fi

rm -f "$db_file" > /dev/null

# to test a backend, call the script with the corresponding environment variable (e.g. GGML_CUDA=1 ./scripts/compare-commits.sh ...)
if [ -n "$GGML_CUDA" ]; then
    CMAKE_OPTS="${CMAKE_OPTS} -DGGML_CUDA=ON"
fi

dir="build-bench"

function run {
    rm -fr ${dir} > /dev/null
    cmake -B ${dir} -S . ${CMAKE_OPTS} > /dev/null
    cmake --build ${dir} -t $target -j $(nproc) > /dev/null
    ${dir}/bin/$target $run_args | sqlite3 "$db_file"
}

git checkout $commit1 > /dev/null
run

git checkout $commit2 > /dev/null
run

./scripts/compare-cheese-bench.py -b $commit1 -c $commit2 --tool $tool -i "$db_file"
