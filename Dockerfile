# Dockerfile to Build, Run, and Test the C++ gRPC Server & C++ ZeroMQ Client
FROM ubuntu:20.04 as base
ENV DEBIAN_FRONTEND=noninteractive

ARG GPRC_VERSION=1.61.1
ARG NUM_JOBS=16

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
    nginx \
    && rm -rf /var/lib/apt/lists/*

# Install grpc from source
RUN cd /tmp && \
    git clone --recurse-submodules -b v${GPRC_VERSION} https://github.com/grpc/grpc && \
    cd grpc && \
    mkdir -p cmake/build && \
    cd cmake/build && \
    cmake -DgRPC_INSTALL=ON \
    -DgRPC_BUILD_TESTS=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    ../.. && \
    make -j${NUM_JOBS} && \
    make install && \
    cd / && rm -rf /tmp/grpc

# Install grpcurl (TESTING ONLY)
RUN curl -sSL https://github.com/fullstorydev/grpcurl/releases/download/v1.8.5/grpcurl_1.8.5_linux_x86_64.tar.gz | tar -xzC /usr/local/bin

WORKDIR /app
COPY . .

RUN cmake -S . -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=YES -DBUILD_TESTS_RPC:BOOL=ON
RUN ninja -C build

# Run the tests
RUN cd tests && ctests && cd ..
