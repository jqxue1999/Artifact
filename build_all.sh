#!/bin/bash

# Build All Script for FHE Artifact
# Builds all three FHE implementations

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "========================================="
echo "FHE Artifact - Build All Components"
echo "========================================="
echo ""

# Track build status
BUILD_STATUS=0

# Function to build with error handling
build_component() {
    local name=$1
    local dir=$2
    local build_cmd=$3

    echo -e "${BLUE}Building $name...${NC}"
    echo "Directory: $dir"
    echo "Command: $build_cmd"
    echo "---"

    if cd "$dir" 2>/dev/null; then
        if eval "$build_cmd"; then
            echo -e "${GREEN}✓ $name build successful${NC}"
            echo ""
            cd - > /dev/null
            return 0
        else
            echo -e "${RED}✗ $name build failed${NC}"
            echo ""
            cd - > /dev/null
            BUILD_STATUS=1
            return 1
        fi
    else
        echo -e "${RED}✗ Directory $dir not found${NC}"
        echo ""
        BUILD_STATUS=1
        return 1
    fi
}

# Save starting directory
START_DIR=$(pwd)

echo "========================================="
echo "1. Building TFHE Implementations (Rust)"
echo "========================================="
echo ""

# Build TFHE workloads
build_component "TFHE Workloads" \
    "rust/tfhe-example/workloads" \
    "cargo build"

# Build TFHE sorting
build_component "TFHE Sorting" \
    "rust/tfhe-example/sorting" \
    "cargo build"

# Build TFHE Floyd-Warshall
build_component "TFHE Floyd-Warshall" \
    "rust/tfhe-example/floyd" \
    "cargo build"

# Build TFHE Decision Tree
build_component "TFHE Decision Tree" \
    "rust/tfhe-example/decision_tree" \
    "cargo build"

# Build TFHE Private DB
build_component "TFHE Private Database" \
    "rust/tfhe-example/private_db" \
    "cargo build"

echo "========================================="
echo "2. Building Scheme Switching (OpenFHE)"
echo "========================================="
echo ""

# Build scheme switching
build_component "Scheme Switching" \
    "scheme_switching" \
    "mkdir -p build && cd build && cmake .. && make -j\$(nproc)"

echo "========================================="
echo "3. Building Encoding Switching (HElib)"
echo "========================================="
echo ""

# Build encoding switching
build_component "Encoding Switching" \
    "encoding_switching" \
    "mkdir -p build && cd build && cmake .. && make -j\$(nproc)"

# Return to starting directory
cd "$START_DIR"

# Summary
echo "========================================="
echo "Build Summary"
echo "========================================="
echo ""

if [ $BUILD_STATUS -eq 0 ]; then
    echo -e "${GREEN}✓ All components built successfully!${NC}"
    echo ""
    echo "Next steps:"
    echo "  - Run quick tests: ./run_quick_tests.sh"
    echo "  - Run full benchmarks: ./run_full_benchmarks.sh"
    echo ""
    exit 0
else
    echo -e "${RED}✗ Some components failed to build${NC}"
    echo ""
    echo "Please check the error messages above and:"
    echo "  - Ensure all dependencies are installed (run ./verify_environment.sh)"
    echo "  - Check README.md for troubleshooting tips"
    echo ""
    exit 1
fi
