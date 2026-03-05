# Cheesebrain Python client (like llama-cpp-python)

Use Cheesebrain from Python in the same style as `llama-cpp-python`: load a model (or connect to a server) and call `create_chat_completion(messages=[...])`.

## Install

```bash
pip install openai
```

## Option 1: Use with a local GGUF file (starts cheese-server for you)

Download the model yourself (e.g. from Hugging Face), then:

```python
from cheesebrain_python_client import Cheesebrain

# Path to your downloaded .gguf file
llm = Cheesebrain(model_path="/path/to/ruvltra-claude-code-0.5b-q4_k_m.gguf")

out = llm.create_chat_completion(messages=[
    {"role": "user", "content": "What is the capital of France?"}
])
print(out.choices[0].message.content)

llm.close()  # or use: with Cheesebrain(...) as llm:
```

## Option 2: Connect to an already-running cheese-server

In one terminal start the server:

```bash
./build/bin/cheese-server -m /path/to/model.gguf --port 8080 --host 127.0.0.1
```

In Python:

```python
from cheesebrain_python_client import Cheesebrain

llm = Cheesebrain(server_url="http://localhost:8080")
out = llm.create_chat_completion(messages=[
    {"role": "user", "content": "What is the capital of France?"}
])
print(out.choices[0].message.content)
```

## Command-line example

```bash
# Use a local model (script starts the server)
python examples/cheesebrain_python_client.py --model /path/to/model.gguf --prompt "Hello!"

# Use an existing server
python examples/cheesebrain_python_client.py --server http://localhost:8080 --prompt "Hello!"
```

## Notes

- **Model download**: There is no `from_pretrained(repo_id="user/model")` that downloads from Hugging Face inside this client (that would require network access). Download the GGUF file yourself (e.g. from the model’s Hugging Face page) and pass its path to `model_path`.
- **cheese-server**: When using `model_path`, the script looks for `cheese-server` in `build/bin/` or in your `PATH`. Build the server with `make cheese-server` in the project’s build directory.
