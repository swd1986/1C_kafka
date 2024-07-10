#!/bin/bash

# Define color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

mkdir build64Win
cd build64Win
# Run CMake to generate the build files
cmake ..

# Build the project
cmake --build .

# Source file path
SOURCE="/kafka/src/build64Win/lib1C_kafka.so"

# Destination file path
DESTINATION="/home/usr1cv8/lib1C_kafka.so"

cp $SOURCE $DESTINATION

if [ $? -eq 0 ]; then
    echo -e "${BLUE}File copied successfully.${NC}"
else
    echo -e "${RED}Failed to copy the file.${NC}"
fi

echo -e "${BLUE}done compiled.${NC}"