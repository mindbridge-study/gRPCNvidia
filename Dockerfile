FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    python3 \
    python3-dev \
    python3-pip \
    protobuf-compiler \
    libprotobuf-dev \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . /app

ARG BUILD_TESTS=OFF
ARG BUILD_PYTHON=OFF
RUN if [ "$BUILD_TESTS" = "ON" ]; then \
        cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=YES -DBUILD_TESTS=ON && cmake --build build -j8; \
    else \
        cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=YES && cmake --build build -j8; \
    fi
