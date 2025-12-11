#!/bin/bash

# Environment Verification Script for FHE Artifact
# Checks that all required dependencies are installed

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Counters
PASS=0
FAIL=0
WARN=0

echo "========================================="
echo "FHE Artifact Environment Verification"
echo "========================================="
echo ""

# Function to check command existence
check_command() {
    if command -v "$1" &> /dev/null; then
        echo -e "${GREEN}✓${NC} $2"
        ((PASS++))
        return 0
    else
        echo -e "${RED}✗${NC} $2"
        ((FAIL++))
        return 1
    fi
}

# Function to check version
check_version() {
    local cmd=$1
    local min_version=$2
    local name=$3

    if command -v "$cmd" &> /dev/null; then
        local version=$($cmd --version 2>&1 | head -n1 | grep -oE '[0-9]+\.[0-9]+' | head -n1)
        if [ -n "$version" ]; then
            echo -e "${GREEN}✓${NC} $name (version: $version)"
            ((PASS++))
            return 0
        else
            echo -e "${YELLOW}⚠${NC} $name (version unknown)"
            ((WARN++))
            return 1
        fi
    else
        echo -e "${RED}✗${NC} $name"
        ((FAIL++))
        return 1
    fi
}

# Function to check directory
check_directory() {
    if [ -d "$1" ]; then
        echo -e "${GREEN}✓${NC} $2"
        ((PASS++))
        return 0
    else
        echo -e "${RED}✗${NC} $2"
        ((FAIL++))
        return 1
    fi
}

echo "1. System Tools"
echo "---------------"
check_command gcc "GCC compiler"
check_command g++ "G++ compiler"
check_command cmake "CMake build system"
check_command make "Make build tool"
check_command git "Git version control"
check_command curl "Curl download tool"
echo ""

echo "2. Required Libraries"
echo "---------------------"
# Check for GMP
if ldconfig -p | grep -q libgmp; then
    echo -e "${GREEN}✓${NC} GMP library (libgmp)"
    ((PASS++))
else
    echo -e "${RED}✗${NC} GMP library (libgmp)"
    ((FAIL++))
fi

# Check for NTL
if ldconfig -p | grep -q libntl; then
    echo -e "${GREEN}✓${NC} NTL library (libntl)"
    ((PASS++))
else
    echo -e "${RED}✗${NC} NTL library (libntl)"
    ((FAIL++))
fi
echo ""

echo "3. Rust Toolchain"
echo "-----------------"
check_version rustc "1.70" "Rust compiler"
check_version cargo "1.70" "Cargo package manager"
echo ""

echo "4. FHE Libraries"
echo "----------------"
check_directory "openfhelibs" "OpenFHE installation (openfhelibs/)"
if [ -d "openfhelibs/include/openfhe" ]; then
    echo -e "${GREEN}✓${NC} OpenFHE headers"
    ((PASS++))
else
    echo -e "${RED}✗${NC} OpenFHE headers"
    ((FAIL++))
fi

check_directory "helib_install" "HElib installation (helib_install/)"
if [ -d "helib_install/include/helib" ]; then
    echo -e "${GREEN}✓${NC} HElib headers"
    ((PASS++))
else
    echo -e "${RED}✗${NC} HElib headers"
    ((FAIL++))
fi

check_directory "HE-Bridge" "HE-Bridge framework (HE-Bridge/)"
echo ""

echo "5. Project Structure"
echo "--------------------"
check_directory "rust/tfhe-example" "TFHE implementation (rust/tfhe-example/)"
check_directory "scheme_switching" "Scheme switching implementation (scheme_switching/)"
check_directory "encoding_switching" "Encoding switching implementation (encoding_switching/)"
echo ""

# Summary
echo "========================================="
echo "Summary:"
echo "========================================="
echo -e "${GREEN}Passed:${NC} $PASS checks"
if [ $WARN -gt 0 ]; then
    echo -e "${YELLOW}Warnings:${NC} $WARN checks"
fi
if [ $FAIL -gt 0 ]; then
    echo -e "${RED}Failed:${NC} $FAIL checks"
fi
echo ""

if [ $FAIL -eq 0 ]; then
    echo -e "${GREEN}✓ Environment verification PASSED${NC}"
    echo "You can proceed to build the artifact with: ./build_all.sh"
    exit 0
else
    echo -e "${RED}✗ Environment verification FAILED${NC}"
    echo ""
    echo "Please install missing dependencies:"
    echo "  - System packages: sudo apt-get install build-essential cmake git curl libgmp-dev libntl-dev"
    echo "  - Rust: curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh"
    echo "  - See README.md for OpenFHE and HElib installation instructions"
    exit 1
fi
