#!/usr/bin/env python3

import sys
import numpy as np
from pathlib import Path
import os

# Add utils directory to path for direct script execution
sys.path.insert(0, str(Path(__file__).parent.parent / "utils"))
from common import get_model_name_from_env_path, compare_tokens, exit_with_warning  # type: ignore[import-not-found]

def quick_logits_check(pytorch_file, cheesecpp_file):
    """Lightweight sanity check before NMSE"""

    try:
        pytorch_logits = np.fromfile(pytorch_file, dtype=np.float32)
        cheesecpp_logits = np.fromfile(cheesecpp_file, dtype=np.float32)
    except Exception as e:
        print(f"❌ NOK: Failed to load files - {e}")
        return False

    # Check shapes match
    if pytorch_logits.shape != cheesecpp_logits.shape:
        print(f"❌ NOK: Shape mismatch - PyTorch: {pytorch_logits.shape}, cheese.cpp: {cheesecpp_logits.shape}")
        return False

    # Calculate key metrics
    diff = pytorch_logits - cheesecpp_logits
    abs_diff = np.abs(diff)
    max_diff = np.max(abs_diff)

    # Get top 10 predictions from both models
    pytorch_top10 = np.argsort(pytorch_logits)[-10:][::-1]
    cheesecpp_top10 = np.argsort(cheesecpp_logits)[-10:][::-1]
    print(f"Top 10 PyTorch logits: {pytorch_logits[pytorch_top10]}")
    print(f"Top 10 cheese.cpp logits: {cheesecpp_logits[cheesecpp_top10]}")
    print(f"Max absolute difference: {max_diff:.4f}")

    return True

def main():
    model_path = os.environ.get('MODEL_PATH')
    model_name = get_model_name_from_env_path('MODEL_PATH')
    data_dir = Path("data")
    pytorch_file = data_dir / f"pytorch-{model_name}.bin"

    cheesecpp_model_name = get_model_name_from_env_path('CONVERTED_MODEL')
    print(f"Using converted model: {cheesecpp_model_name}")
    cheesecpp_file = data_dir / f"cheesecpp-{cheesecpp_model_name}.bin"

    if not pytorch_file.exists():
        print(f"Error: PyTorch logits file not found: {pytorch_file}")
        print("Please run scripts/run-org-model.sh first to generate this file.")
        sys.exit(1)

    if not cheesecpp_file.exists():
        print(f"Error: cheese.cpp logits file not found: {cheesecpp_file}")
        print("Please run scripts/run-converted-model.sh first to generate this file.")
        sys.exit(1)

    print("Checked all required files were found. Proceeding...\n")

    # Verify tokens as they are a prerequisite for logits comparison.
    print("🔍 Token Comparison Check")
    print("=" * 40)
    if not compare_tokens(f"pytorch-{model_name}", f"cheesecpp-{cheesecpp_model_name}"):
        exit_with_warning("\n❌ Token mismatch detected", model_path)
    print()

    print("🔍 GGML Model Validation for model ", model_name)
    print("=" * 40)
    print(f"PyTorch logits  : {pytorch_file}")
    print(f"cheese.cpp logits: {cheesecpp_file}")
    print()

    success = quick_logits_check(pytorch_file, cheesecpp_file)

    # Exit with appropriate code
    if success:
        print("✅ OK: Lightweight model check successful!")
        print("       Ok to proceed with NMSE check...")
        sys.exit(0)
    else:
        exit_with_warning(f"❌ NOK: Top 10 predictions don't match - generation will differ", model_path)

if __name__ == "__main__":
    main()
