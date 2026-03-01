.PHONY: build

build:
	xcodebuild -scheme cheese-batched-swift -destination "generic/platform=macOS" -derivedDataPath build
	rm -f ./cheese-batched-swift
	ln -s ./build/Build/Products/Debug/cheese-batched-swift ./cheese-batched-swift
