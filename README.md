# Cheese Brain

![Cheese Hero](media/cheese0.png)

[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://github.com/ggml-org/cheese.cpp/actions/workflows/build.yml/badge.svg)](https://github.com/ggml-org/cheese.cpp/actions/workflows/build.yml)
[![Stars](https://img.shields.io/github/stars/ggml-org/cheese.cpp.svg)](https://github.com/ggml-org/cheese.cpp/stargazers)
[![Forks](https://img.shields.io/github/forks/ggml-org/cheese.cpp.svg)](https://github.com/ggml-org/cheese.cpp/network/members)
[![Issues](https://img.shields.io/github/issues/ggml-org/cheese.cpp.svg)](https://github.com/ggml-org/cheese.cpp/issues)
[![Pull Requests](https://img.shields.io/github/issues-pr/ggml-org/cheese.cpp.svg)](https://github.com/ggml-org/cheese.cpp/pulls)

**Cheese Brain** is a state-of-the-art C/C++ library for high-performance Large Language Model (LLM) inference. Designed for efficiency, portability, and ease of use, Cheese enables you to run the latest AI models on anything from a laptop to a high-end server with minimal setup.

## Key Features

- **Pure C/C++**: Zero-dependency implementation for maximum portability.
- **Hardware Optimized**: 
  - **Apple Silicon**: First-class support via ARM NEON, Accelerate, and Metal.
  - **x86**: Optimized for AVX, AVX2, AVX512, and AMX.
  - **NVIDIA**: High-performance CUDA kernels for discrete GPUs.
- **Efficiency**: Advanced 1.5-bit to 8-bit quantization for reduced memory footprint and lightning-fast execution.
- **Extensible**: Supports a wide range of state-of-the-art models via GGUF format compatibility.

## Quick Start

Cheese is designed to be productive from minute one.

### Installation
Build from source for the best performance on your specific hardware:

```bash
cmake -B build
cmake --build build --config Release
```

### Usage
Run your first model in seconds:

```bash
# Chat with a model
./build/bin/cheese-cli -m model.gguf -cnv

# Start an OpenAI-compatible API server (no UI by default; add --webui for the web UI)
./build/bin/cheese-server -m model.gguf --port 8080
```

## Description

The core mission of **cheese.cpp** is to democratize AI by making high-performance inference accessible on everyday hardware. By leveraging the power of `ggml`, Cheese provides a playground for cutting-edge LLM features, ensuring you always have access to the latest advancements in the field.

## Documentation

Explore our comprehensive guides to get the most out of Cheese:

- [Installation & Build Guide](docs/build.md)
- [Command Line Interface (CLI)](tools/cli/README.md)
- [HTTP API Server](tools/server/README.md)
- [Quantization Guide](tools/quantize/README.md)

## Performance

To get the best speed: build with a backend that matches your hardware (see [Build](docs/build.md): BLAS for CPU prefill, CUDA/Metal/SYCL for GPU). At runtime, tune threads (`-t`), GPU layers (`-ngl`), and batch size (`-ub`). See [Token generation performance tips](docs/development/token_generation_performance_tips.md) for CPU/GPU tuning and [Server README](tools/server/README.md#performance) for server throughput. To measure speed, see [Benchmarking](docs/development/benchmarking.md) and `scripts/bench-perf.sh`.

---

*Cheese.cpp - The most delicious way to run LLMs.*
