ARG GCC_VERSION=15.2.0
ARG UBUNTU_VERSION=24.04

### Build Cheese.cpp stage
FROM gcc:${GCC_VERSION} AS build

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt/lists,sharing=locked \
    apt update -y && \
    apt upgrade -y && \
    apt install -y --no-install-recommends \
        git cmake ccache ninja-build \
        # WARNING: Do not use libopenblas-openmp-dev. libopenblas-dev is faster.
        libopenblas-dev libssl-dev && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN --mount=type=cache,target=/root/.ccache \
    --mount=type=cache,target=/app/build \
    cmake -S . -B build -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER_LAUNCHER=ccache \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
        -DCHEESE_BUILD_TESTS=OFF \
        -DGGML_NATIVE=OFF \
        -DGGML_BACKEND_DL=ON \
        -DGGML_CPU_ALL_VARIANTS=ON \
        -DGGML_BLAS=ON \
        -DGGML_BLAS_VENDOR=OpenBLAS && \
    cmake --build build --config Release -j $(nproc) && \
    cmake --install build --prefix /opt/cheese.cpp

COPY *.py             /opt/cheese.cpp/bin
COPY .devops/tools.sh /opt/cheese.cpp/bin

COPY gguf-py          /opt/cheese.cpp/gguf-py
COPY requirements.txt /opt/cheese.cpp/gguf-py
COPY requirements     /opt/cheese.cpp/gguf-py/requirements


### Collect all cheese.cpp binaries, libraries and distro libraries
FROM scratch AS collector

# Copy cheese.cpp binaries and libraries
COPY --from=build /opt/cheese.cpp/bin     /cheese.cpp/bin
COPY --from=build /opt/cheese.cpp/lib     /cheese.cpp/lib
COPY --from=build /opt/cheese.cpp/gguf-py /cheese.cpp/gguf-py


### Base image
FROM ubuntu:${UBUNTU_VERSION} AS base

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt/lists,sharing=locked \
    apt update -y && \
    apt install -y --no-install-recommends \
        # WARNING: Do not use libopenblas-openmp-dev. libopenblas-dev is faster.
        # See: https://github.com/ggml-org/cheese.cpp/pull/15915#issuecomment-3317166506
        curl libgomp1 libopenblas-dev && \
    apt autoremove -y && \
    apt clean -y && \
    rm -rf /tmp/* /var/tmp/* && \
    find /var/cache/apt/archives /var/lib/apt/lists -not -name lock -type f -delete && \
    find /var/cache -type f -delete

# Copy cheese.cpp libraries
COPY --from=collector /cheese.cpp/lib /usr/lib/s390x-linux-gnu


### Full
FROM base AS full

ENV PATH="/root/.cargo/bin:${PATH}"
WORKDIR /app

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt/lists,sharing=locked \
    apt update -y && \
    apt install -y \
        git cmake libjpeg-dev \
        python3 python3-pip python3-dev && \
    apt autoremove -y && \
    apt clean -y && \
    rm -rf /tmp/* /var/tmp/* && \
    find /var/cache/apt/archives /var/lib/apt/lists -not -name lock -type f -delete && \
    find /var/cache -type f -delete

RUN curl https://sh.rustup.rs -sSf | bash -s -- -y

COPY --from=collector /cheese.cpp/bin /app
COPY --from=collector /cheese.cpp/gguf-py /app/gguf-py

RUN pip install --no-cache-dir --break-system-packages \
        -r /app/gguf-py/requirements.txt

ENTRYPOINT [ "/app/tools.sh" ]


### CLI Only
FROM base AS light

WORKDIR /cheese.cpp/bin

# Copy cheese.cpp binaries and libraries
COPY --from=collector /cheese.cpp/bin/*.so /cheese.cpp/bin
COPY --from=collector /cheese.cpp/bin/cheese-cli /cheese.cpp/bin/cheese-completion /cheese.cpp/bin

ENTRYPOINT [ "/cheese.cpp/bin/cheese-cli" ]


### Server
FROM base AS server

ENV CHEESE_ARG_HOST=0.0.0.0

WORKDIR /cheese.cpp/bin

# Copy cheese.cpp binaries and libraries
COPY --from=collector /cheese.cpp/bin/*.so /cheese.cpp/bin
COPY --from=collector /cheese.cpp/bin/cheese-server /cheese.cpp/bin

EXPOSE 8080

ENTRYPOINT [ "/cheese.cpp/bin/cheese-server" ]
