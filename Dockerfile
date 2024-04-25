# Dockerfile to Build, Run, and Test the C++ gRPC Server & C++ ZeroMQ Client
FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

ARG GPRC_VERSION=1.61.1
ARG NUM_JOBS=8

# Install required dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    protobuf-compiler \
    libprotobuf-dev \
    libssl-dev \
    curl \
    git \
    pkg-config \
    autoconf \
    automake \
    libtool \
    && rm -rf /var/lib/apt/lists/*

# Install grpc from source
RUN cd /tmp && \
    git clone --recurse-submodules -b v${GPRC_VERSION} https://github.com/grpc/grpc && \
    cd grpc && \
    mkdir -p cmake/build && \
    cd cmake/build && \
    cmake -DgRPC_INSTALL=ON \
          -DgRPC_BUILD_TESTS=OFF \
          -DCMAKE_INSTALL_PREFIX=/usr/local \
          ../.. && \
    make -j${NUM_JOBS} && \
    make install

WORKDIR /app
COPY . .

# Configure and build the application
RUN cmake -S . -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=YES
RUN ninja -C build

RUN mv build/compile_commands.json .
