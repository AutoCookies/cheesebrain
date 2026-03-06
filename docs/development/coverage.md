# Code coverage

The project supports building with gcov for code coverage. Coverage is used to see which code paths are exercised by tests and to catch regressions.

## Building with coverage

Configure and build with the coverage option and Debug build type:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCHEESE_COVERAGE=ON
cmake --build build --config Debug
```

Then run the test suite. Coverage data (`.gcda`/`.gcno` files) are produced in the build tree.

## Generating a report (local)

Install `lcov`, then from the build directory:

```bash
cd build
lcov --directory . --capture --output-file coverage.info --rc lcov_branch_coverage=0
lcov --remove coverage.info '/usr/*' '*/tests/*' '*/ggml/*' '*/examples/*' --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
```

Open `coverage_html/index.html` in a browser.

## CI

The [Coverage](../../.github/workflows/coverage.yml) workflow runs on schedule (weekly) and can be triggered manually via **Actions → Coverage → Run workflow**. It builds with coverage, runs tests, and uploads the HTML report as an artifact.
