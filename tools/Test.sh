#!/bin/bash
# docker-compose up --build test
docker-compose up --build app

# IMPORTANT: CHANGE THIS TO YOUR COMPUTEr
CONTAINER_NAME="grpc_nvidia_app"
SOURCE_PATH="/app/build/compile_commands.json"
DEST_PATH="$HOME/Software/gRPCNvidia/compile_commands.json"
docker cp "$CONTAINER_NAME:$SOURCE_PATH" "$DEST_PATH"

echo "Copied compile_commands.json to $DEST_PATH"
