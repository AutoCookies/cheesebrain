# Benchmarking inference speed

This page describes how to measure prompt and generation throughput (tokens per second) for cheesebrain.

## Quick check with the CLI

Use the CLI with a fixed token count and `--perf` to see prompt and generation speed:

```bash
./build/bin/cheese-cli -m path/to/model.gguf -p "Repeat the following word 500 times: hello" -n 1000 --perf
```

At the end of the run you will see a line like:

```
[ Prompt: 165.3 t/s | Generation: 48.6 t/s ]
```

- **Prompt** (prefill) speed is usually higher than generation speed.
- For more accurate generation numbers, use a longer `-n` (e.g. 1000) so that the reported rate is averaged over many tokens.

See [Token generation performance tips](token_generation_performance_tips.md) for how to tune `-t`, `-ngl`, and other flags.

## Batched benchmark

For systematic sweeps over batch sizes and parallel sequences, use the batched benchmark:

```bash
./build/bin/batched-bench -m path/to/model.gguf -c 2048 -b 2048 -ub 512 -npp 128,256,512 -ntg 128,256 -npl 1,2,4,8,16,32
```

It prints a table with columns including **S_PP t/s** (prompt tokens per second) and **S_TG t/s** (generation tokens per second). Example usage and options are printed when you run it without arguments.

## cheese-bench

For more comprehensive benchmarks (including different backends and tests), see [tools/cheese-bench/README.md](../../tools/cheese-bench/README.md).

## CI or scripted runs

Use [scripts/bench-perf.sh](../../scripts/bench-perf.sh) to run a quick CLI benchmark and optionally extract t/s for comparison or CI.
