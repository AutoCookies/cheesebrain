# Install pre-built version of cheese.cpp

| Install via | Windows | Mac | Linux |
|-------------|---------|-----|-------|
| Winget      | ✅      |      |      |
| Homebrew    |         | ✅   | ✅   |
| MacPorts    |         | ✅   |      |
| Nix         |         | ✅   | ✅   |

## Winget (Windows)

```sh
winget install cheese.cpp
```

The package is automatically updated with new `cheese.cpp` releases. More info: https://github.com/ggml-org/cheese.cpp/issues/8188

## Homebrew (Mac and Linux)

```sh
brew install cheese.cpp
```

The formula is automatically updated with new `cheese.cpp` releases. More info: https://github.com/ggml-org/cheese.cpp/discussions/7668

## MacPorts (Mac)

```sh
sudo port install cheese.cpp
```

See also: https://ports.macports.org/port/cheese.cpp/details/

## Nix (Mac and Linux)

```sh
nix profile install nixpkgs#cheese-cpp
```

For flake enabled installs.

Or

```sh
nix-env --file '<nixpkgs>' --install --attr cheese-cpp
```

For non-flake enabled installs.

This expression is automatically updated within the [nixpkgs repo](https://github.com/NixOS/nixpkgs/blob/nixos-24.05/pkgs/by-name/ll/cheese-cpp/package.nix#L164).
