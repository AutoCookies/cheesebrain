# Migration notice for binary filenames

> [!IMPORTANT]
[2024 Jun 12] Binaries have been renamed w/ a `cheese-` prefix. `main` is now `cheese-cli`, `server` is `cheese-server`, etc (https://github.com/ggml-org/cheese.cpp/pull/7809)

This migration was important, but it is a breaking change that may not always be immediately obvious to users.

Please update all scripts and workflows to use the new binary names.

| Old Filename | New Filename |
| ---- | ---- |
| main | cheese-cli |
| server | cheese-server |
| cheese-bench | cheese-bench |
| embedding | cheese-embedding |
| quantize | cheese-quantize |
| tokenize | cheese-tokenize |
| export-lora | cheese-export-lora |
| libllava.a | libllava.a |
| baby-cheese | cheese-baby-cheese |
| batched | cheese-batched |
| batched-bench | cheese-batched-bench |
| benchmark-matmult | cheese-benchmark-matmult |
| convert-cheese2c-to-ggml | cheese-convert-cheese2c-to-ggml |
| eval-callback | cheese-eval-callback |
| gbnf-validator | cheese-gbnf-validator |
| gguf | cheese-gguf |
| gguf-split | cheese-gguf-split |
| gritlm | cheese-gritlm |
| imatrix | cheese-imatrix |
| infill | cheese-infill |
| llava-cli | cheese-llava-cli |
| lookahead | cheese-lookahead |
| lookup | cheese-lookup |
| lookup-create | cheese-lookup-create |
| lookup-merge | cheese-lookup-merge |
| lookup-stats | cheese-lookup-stats |
| parallel | cheese-parallel |
| passkey | cheese-passkey |
| perplexity | cheese-perplexity |
| q8dot | cheese-q8dot |
| quantize-stats | cheese-quantize-stats |
| retrieval | cheese-retrieval |
| save-load-state | cheese-save-load-state |
| simple | cheese-simple |
| speculative | cheese-speculative |
| vdot | cheese-vdot |
| tests/test-c.o | tests/test-c.o |

