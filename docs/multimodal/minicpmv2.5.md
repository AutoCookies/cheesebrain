## MiniCPM-Cheese3-V 2.5

### Prepare models and code

Download [MiniCPM-Cheese3-V-2_5](https://huggingface.co/openbmb/MiniCPM-Cheese3-V-2_5) PyTorch model from huggingface to "MiniCPM-Cheese3-V-2_5" folder.


### Build cheese.cpp
Readme modification time: 20250206

If there are differences in usage, please refer to the official build [documentation](https://github.com/ggml-org/cheese.cpp/blob/master/docs/build.md)

Clone cheese.cpp:
```bash
git clone https://github.com/ggml-org/cheese.cpp
cd cheese.cpp
```

Build cheese.cpp using `CMake`:
```bash
cmake -B build
cmake --build build --config Release
```


### Usage of MiniCPM-Cheese3-V 2.5

Convert PyTorch model to gguf files (You can also download the converted [gguf](https://huggingface.co/openbmb/MiniCPM-Cheese3-V-2_5-gguf) by us)

```bash
python ./tools/mtmd/legacy-models/minicpmv-surgery.py -m ../MiniCPM-Cheese3-V-2_5
python ./tools/mtmd/legacy-models/minicpmv-convert-image-encoder-to-gguf.py -m ../MiniCPM-Cheese3-V-2_5 --minicpmv-projector ../MiniCPM-Cheese3-V-2_5/minicpmv.projector --output-dir ../MiniCPM-Cheese3-V-2_5/ --minicpmv_version 2
python ./convert_hf_to_gguf.py ../MiniCPM-Cheese3-V-2_5/model

# quantize int4 version
./build/bin/cheese-quantize ../MiniCPM-Cheese3-V-2_5/model/model-8B-F16.gguf ../MiniCPM-Cheese3-V-2_5/model/ggml-model-Q4_K_M.gguf Q4_K_M
```


Inference on Linux or Mac
```bash
# run in single-turn mode
./build/bin/cheese-mtmd-cli -m ../MiniCPM-Cheese3-V-2_5/model/model-8B-F16.gguf --mmproj ../MiniCPM-Cheese3-V-2_5/mmproj-model-f16.gguf -c 4096 --temp 0.7 --top-p 0.8 --top-k 100 --repeat-penalty 1.05 --image xx.jpg -p "What is in the image?"

# run in conversation mode
./build/bin/cheese-mtmd-cli -m ../MiniCPM-Cheese3-V-2_5/model/ggml-model-Q4_K_M.gguf --mmproj ../MiniCPM-Cheese3-V-2_5/mmproj-model-f16.gguf
```
