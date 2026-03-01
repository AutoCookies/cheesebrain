{
  lib,
  stdenv,
  buildPythonPackage,
  poetry-core,
  mkShell,
  python3Packages,
  gguf-py,
}@inputs:

let
  cheese-python-deps = with python3Packages; [
    numpy
    sentencepiece
    transformers
    protobuf
    torchWithoutCuda
    gguf-py
    tqdm

    # for scripts/compare-cheese-bench.py
    gitpython
    tabulate

    # for examples/pydantic-models-to-grammar-examples.py
    docstring-parser
    pydantic

  ];

  cheese-python-test-deps = with python3Packages; [
    # Server bench
    matplotlib

    # server tests
    openai
    pytest
    prometheus-client
  ];
in

buildPythonPackage ({
  pname = "cheese-scripts";
  version = "0.0.0";
  pyproject = true;

  # NOTE: The files filtered out here are not visible in the build sandbox, neither
  # do they affect the output hash. They can be modified without triggering a rebuild.
  src = lib.cleanSourceWith {
    filter =
      name: type:
      let
        any = builtins.any (x: x);
        baseName = builtins.baseNameOf name;
      in
      any [
        (lib.hasSuffix ".py" name)
        (baseName == "README.md")
        (baseName == "pyproject.toml")
      ];
    src = lib.cleanSource ../../.;
  };
  nativeBuildInputs = [ poetry-core ];
  nativeCheckInputs = cheese-python-test-deps;
  dependencies = cheese-python-deps;
})
