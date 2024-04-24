FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    libzmq5 \
    libzmq3-dev \
    libzmqpp-dev \
    cmake \
    ninja-build \
    python3 \
    python3-dev \
    python3-zmq \
    protobuf-compiler \
    libprotobuf-dev \
    libssl-dev \
    curl \
    && rm -rf /var/lib/apt/lists/*

RUN curl -sSL https://github.com/fullstorydev/grpcurl/releases/download/v1.8.5/grpcurl_1.8.5_linux_x86_64.tar.gz -o grpcurl.tar.gz \
    && tar -xzf grpcurl.tar.gz -C /usr/local/bin \
    && rm grpcurl.tar.gz

WORKDIR /app
COPY . /app

RUN cd Python && python3 -m venv env && source env/bin/activate && pip install -r requirements.txt && cd ..

RUN rm -rf build && cmake -S . -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=YES && ninja -C build

CMD ["/bin/bash", "-c", "./build/gRPCNvidia"]