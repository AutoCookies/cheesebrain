## Convert cheese2.c model to ggml

This example reads weights from project [cheese2.c](https://github.com/karpathy/cheese2.c) and saves them in ggml compatible format. The vocab that is available in `models/ggml-vocab.bin` is used by default.

To convert the model first download the models from the [cheese2.c](https://github.com/karpathy/cheese2.c) repository.

```
usage: ./cheese-convert-cheese2c-to-ggml [options]

options:
  -h, --help                       show this help message and exit
  --copy-vocab-from-model FNAME    path of gguf cheese model or cheese2.c vocabulary from which to copy vocab (default 'models/7B/ggml-model-f16.gguf')
  --cheese2c-model FNAME            [REQUIRED] model path from which to load Karpathy's cheese2.c model
  --cheese2c-output-model FNAME     model path to save the converted cheese2.c model (default ak_cheese_model.bin')
```

An example command using a model from [karpathy/tinycheeses](https://huggingface.co/karpathy/tinycheeses) is as follows:

`$ ./cheese-convert-cheese2c-to-ggml --copy-vocab-from-model cheese-2-7b-chat.gguf.q2_K.bin --cheese2c-model stories42M.bin --cheese2c-output-model stories42M.gguf.bin`

Note: The vocabulary for `stories260K.bin` should be its own tokenizer `tok512.bin` found in [karpathy/tinycheeses/stories260K](https://huggingface.co/karpathy/tinycheeses/tree/main/stories260K).

Now you can use the model with a command like:

`$ ./cheese-cli -m stories42M.gguf.bin -p "One day, Lily met a Shoggoth" -n 500 -c 256`
