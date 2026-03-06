# Error handling

This document describes how errors are reported and how to handle them when using or contributing to the cheese library.

## C API

The public C API in `include/cheese.h` uses the following conventions:

### Pointer-returning functions

- **Success:** Returns a valid pointer (e.g. `cheese_model *`, `cheese_context *`, `cheese_vocab *`).
- **Failure:** Returns `NULL`. The caller must check for `NULL` before use.

Examples: `cheese_model_load_from_file()`, `cheese_init_from_model()`, `cheese_model_get_vocab()`.

### Integer-returning functions

- **Success:** Typically returns `0` or a non-negative value (e.g. number of tokens, size).
- **Failure:** Returns a negative value or non-zero error code, depending on the function. See the function documentation in the header.

Examples: `cheese_decode()` returns non-zero on failure; `cheese_tokenize()` returns negative token count on error when the output buffer is `NULL`.

### Caller responsibilities

- Always check return values and pointers before use. Do not call other cheese API functions with `NULL` or after a failed call unless the API explicitly allows it.
- On failure, release any resources already obtained (e.g. call `cheese_model_free(model)` if `cheese_init_from_model()` fails after a successful `cheese_model_load_from_file()`).

## C++ and internal code

- **Return codes:** Use `int` or `bool` return values for functions that can fail in a recoverable way; document the meaning of non-zero or `false`.
- **Exceptions:** Internal C++ code may use `throw std::runtime_error` (or similar) for fatal, non-recoverable failures (e.g. model load failure when mmap or critical read fails). The C API catches these at the boundary and turns them into `NULL` or error return values so C callers never see exceptions.
- **Logging:** Use `CHEESE_LOG_ERROR` and `CHEESE_LOG_WARN` for diagnostics. Logging is for debugging and operator visibility; it does not replace returning an error to the caller. Prefer returning a clear error to the caller and logging once at the point of failure.

## When to use which

| Situation | Prefer |
|-----------|--------|
| C API boundary (public `cheese.h` functions) | Return codes and `NULL`; never let exceptions escape to C callers. |
| Internal C++ code, recoverable failure | Return code or `false` so the caller can retry or clean up. |
| Internal C++ code, fatal failure (e.g. corrupted file, OOM) | Exception or return code; ensure C API translates to `NULL`/error. |
| Optional or best-effort behavior | Log with `CHEESE_LOG_WARN` and continue; do not fail the whole operation. |

## Contributing

When adding or changing code, follow the existing pattern in the same file or module: if the rest of the file uses return codes, use return codes; if it uses exceptions for fatal paths, keep that consistent. For new C API functions, always document in the header whether failure is indicated by `NULL`, a negative value, or another convention.
