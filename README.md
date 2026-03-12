# Cheesebrain

![Cheesebrain Hero](media/cheese0.png)

[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

**Cheesebrain** is a high-performance C/C++ runtime for Large Language Model (LLM) inference. It is designed to be small, fast, and self-contained so you can run modern GGUF models on laptops, workstations, and servers with minimal setup.

Cheesebrain is its own project and is not described as “built from cheese.cpp”. It focuses on:

- **Portability** – a single codebase that runs well on Linux, macOS, and Windows.
- **Performance** – tight low-level code with aggressive quantization support and hardware-aware kernels.
- **Practical tooling** – CLI, HTTP server, Web UI, quantization tools, and model conversion utilities.

## Key Features

- **C / C++ implementation** for easy integration into existing systems.
- **Hardware-optimized backends**:
  - Apple Silicon (NEON, Accelerate, Metal)
  - x86 (SSE/AVX/AVX2/AVX-512/AMX where available)
  - Optional GPU backends (CUDA / Metal / others, depending on build flags)
- **Quantization-aware**: supports multiple GGUF quantization schemes to reduce memory and improve throughput.
- **Rich tooling**:
  - `cheese-cli` for interactive and scripted use.
  - `cheese-server` for an OpenAI-compatible HTTP API (with optional Web UI).
  - Quantization, benchmarking, and conversion helpers under `tools/`.

## Quick Start

### Build

From the repository root:

```bash
cmake -B build
cmake --build build --config Release
```

This produces binaries under `build/bin/`.

### Run a model

Assuming you have a GGUF model at `./models/model.gguf`:

```bash
# Chat from the terminal
./build/bin/cheese-cli -m ./models/model.gguf -cnv

# Start an OpenAI-compatible HTTP server (add --webui for the Web UI)
./build/bin/cheese-server -m ./models/model.gguf --port 8080
```

The server exposes `/v1/chat/completions`, `/v1/completions`, `/v1/embeddings`, and related endpoints.

## Documentation

See the in-repo docs for details:

- [Build & configuration](docs/build.md)
- [CLI usage](tools/cli/README.md)
- [HTTP server & Web UI](tools/server/README.md)
- [Quantization](tools/quantize/README.md)

## Performance Tips

To get the best performance on your machine:

- Build with an appropriate backend (BLAS for CPU, CUDA/Metal/SYCL where applicable).
- Tune runtime flags:
  - Threads: `-t N`
  - GPU layers: `-ngl N`
  - Batch size / ubatch: `-ub N`
- See:
  - [Token generation performance tips](docs/development/token_generation_performance_tips.md)
  - [Server performance notes](tools/server/README.md#performance)
  - [Benchmarking guide](docs/development/benchmarking.md)

Cheesebrain aims to be a pragmatic, low-friction way to run GGUF models locally while remaining small and hackable.
