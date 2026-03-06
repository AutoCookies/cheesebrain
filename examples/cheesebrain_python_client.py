#!/usr/bin/env python3
"""
Cheesebrain Python client — use cheese-server from Python like llama-cpp-python.

Usage:
  # Option 1: Connect to an already-running cheese-server
  from cheesebrain_python_client import Cheesebrain
  llm = Cheesebrain(server_url="http://localhost:8080")

  # Option 2: Start cheese-server with a model (model must be downloaded locally)
  llm = Cheesebrain(model_path="/path/to/model.gguf")

  # Then use the same style as llama-cpp-python
  out = llm.create_chat_completion(messages=[
      {"role": "user", "content": "What is the capital of France?"}
  ])
  print(out["choices"][0]["message"]["content"])

Requires: pip install openai
"""

from __future__ import annotations

import os
import shutil
import subprocess
import sys
import time
from typing import Any

try:
    from openai import OpenAI
except ImportError:
    print("Install the OpenAI client: pip install openai", file=sys.stderr)
    sys.exit(1)


# Default port for cheese-server
DEFAULT_PORT = 8080


def _find_cheese_server() -> str | None:
    """Find cheese-server binary (build dir or PATH)."""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    build_bin = os.path.join(script_dir, "..", "build", "bin", "cheese-server")
    if os.path.isfile(build_bin) and os.access(build_bin, os.X_OK):
        return os.path.normpath(build_bin)
    return shutil.which("cheese-server")


class Cheesebrain:
    """
    Python client for Cheesebrain (cheese-server), with an API similar to llama-cpp-python.

    Either connect to an existing server (server_url) or start one from a local GGUF path (model_path).
    """

    def __init__(
        self,
        server_url: str | None = None,
        model_path: str | None = None,
        server_port: int = DEFAULT_PORT,
        api_key: str = "no-key",
        *,
        _process: subprocess.Popen | None = None,
    ) -> None:
        if server_url is not None and model_path is not None:
            raise ValueError("Provide only one of server_url or model_path")

        self._process: subprocess.Popen | None = _process
        base = (server_url or f"http://localhost:{server_port}").rstrip("/")
        if not base.endswith("/v1"):
            base = base + "/v1"

        if model_path is not None:
            if not os.path.isfile(model_path):
                raise FileNotFoundError(f"Model file not found: {model_path}")
            exe = _find_cheese_server()
            if not exe:
                raise RuntimeError(
                    "cheese-server not found. Build it (cmake + make cheese-server) "
                    "or set PATH so 'cheese-server' is available."
                )
            base = f"http://127.0.0.1:{server_port}/v1"
            self._process = subprocess.Popen(
                [exe, "-m", model_path, "--port", str(server_port), "--host", "127.0.0.1"],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.PIPE,
                cwd=os.path.dirname(exe) or None,
            )
            for _ in range(50):
                try:
                    self._client = OpenAI(base_url=base, api_key=api_key)
                    self._client.models.list()
                    break
                except Exception:
                    time.sleep(0.2)
            else:
                self.close()
                raise RuntimeError("cheese-server did not become ready in time")
        else:
            self._client = OpenAI(base_url=base, api_key=api_key)

    def create_chat_completion(
        self,
        messages: list[dict[str, str]],
        *,
        model: str = "gpt-3.5-turbo",
        max_tokens: int | None = None,
        stream: bool = False,
        **kwargs: Any,
    ) -> Any:
        """
        Chat completion, same style as llama-cpp-python / OpenAI.

        messages: list of {"role": "user"|"system"|"assistant", "content": "..."}
        """
        payload = {"model": model, "messages": messages, **kwargs}
        if max_tokens is not None:
            payload["max_tokens"] = max_tokens
        if stream:
            payload["stream"] = True
            return self._client.chat.completions.create(**payload)
        return self._client.chat.completions.create(**payload)

    def close(self) -> None:
        """Stop the server process if this client started it."""
        if self._process is not None:
            self._process.terminate()
            try:
                self._process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                self._process.kill()
            self._process = None

    def __enter__(self) -> "Cheesebrain":
        return self

    def __exit__(self, *args: Any) -> None:
        self.close()


def main() -> None:
    import argparse
    p = argparse.ArgumentParser(description="Cheesebrain Python client example")
    p.add_argument("--model", "-m", type=str, help="Path to GGUF model (or leave unset to use server_url)")
    p.add_argument("--server", "-s", type=str, default="http://localhost:8080", help="Server URL if not starting from --model")
    p.add_argument("--prompt", "-p", type=str, default="What is the capital of France?")
    args = p.parse_args()

    if args.model:
        print("Starting cheese-server with model and sending one chat request...", flush=True)
        with Cheesebrain(model_path=args.model) as llm:
            out = llm.create_chat_completion(messages=[{"role": "user", "content": args.prompt}])
            print(out.choices[0].message.content)
    else:
        print("Using existing server and sending one chat request...", flush=True)
        llm = Cheesebrain(server_url=args.server)
        out = llm.create_chat_completion(messages=[{"role": "user", "content": args.prompt}])
        print(out.choices[0].message.content)


if __name__ == "__main__":
    main()
