#!/bin/bash

# Quick Test Script for FHE Artifact
# Runs fast smoke tests for each implementation (~5-10 minutes total)

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "========================================="
echo "FHE Artifact - Quick Tests"
echo "========================================="
echo "This runs fast smoke tests to verify all implementations work correctly."
echo "Expected runtime: 5-10 minutes"
echo ""
echo "For full benchmarks matching paper results, use: ./run_full_benchmarks.sh"
echo ""

# Save starting directory
START_DIR=$(pwd)

# Create results directory
RESULTS_DIR="$START_DIR/quick_test_results"
mkdir -p "$RESULTS_DIR"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

echo "Results will be saved to: $RESULTS_DIR/"
echo ""

# Test counter
TEST_PASS=0
TEST_FAIL=0

# Function to run test with timeout and capture output
run_test() {
    local name=$1
    local dir=$2
    local cmd=$3
    local timeout=$4  # in seconds

    echo "========================================="
    echo -e "${BLUE}Testing: $name${NC}"
    echo "========================================="
    echo "Directory: $dir"
    echo "Command: $cmd"
    echo "Timeout: ${timeout}s"
    echo ""

    local result_file="$RESULTS_DIR/${name// /_}_$TIMESTAMP.log"

    if cd "$dir" 2>/dev/null; then
        echo "Running test..."
        if timeout "$timeout" bash -c "$cmd" > "$result_file" 2>&1; then
            echo -e "${GREEN}✓ $name PASSED${NC}"
            echo "Output saved to: $result_file"
            echo ""
            ((TEST_PASS++))
            cd - > /dev/null
            return 0
        else
            local exit_code=$?
            if [ $exit_code -eq 124 ]; then
                echo -e "${YELLOW}⚠ $name TIMEOUT (exceeded ${timeout}s)${NC}"
            else
                echo -e "${RED}✗ $name FAILED${NC}"
            fi
            echo "Output saved to: $result_file"
            echo ""
            ((TEST_FAIL++))
            cd - > /dev/null
            return 1
        fi
    else
        echo -e "${RED}✗ Directory $dir not found${NC}"
        echo ""
        ((TEST_FAIL++))
        return 1
    fi
}

# Test 1: TFHE Workloads (quick version - only run once without full benchmark)
echo "Test 1/3: TFHE Workloads"
echo "------------------------"
run_test "TFHE-Workloads" \
    "rust/tfhe-example/workloads" \
    "cargo run 2>&1 | head -n 100" \
    300

# Test 2: Scheme Switching
echo "Test 2/3: Scheme Switching"
echo "--------------------------"
run_test "Scheme-Switching" \
    "scheme_switching/build/bin" \
    "./workload 2>&1 | head -n 100" \
    300

# Test 3: Encoding Switching
echo "Test 3/3: Encoding Switching"
echo "----------------------------"
run_test "Encoding-Switching" \
    "encoding_switching/build/bin" \
    "./workload 2>&1 | head -n 100" \
    600

# Return to starting directory
cd "$START_DIR"

# Summary
echo "========================================="
echo "Quick Test Summary"
echo "========================================="
echo ""
echo -e "Tests Passed: ${GREEN}$TEST_PASS${NC}"
if [ $TEST_FAIL -gt 0 ]; then
    echo -e "Tests Failed: ${RED}$TEST_FAIL${NC}"
fi
echo ""
echo "Results saved in: $RESULTS_DIR/"
echo ""

if [ $TEST_FAIL -eq 0 ]; then
    echo -e "${GREEN}✓ All quick tests PASSED!${NC}"
    echo ""
    echo "Your environment is working correctly."
    echo ""
    echo "Next steps:"
    echo "  - Review results in $RESULTS_DIR/"
    echo "  - Run full benchmarks: ./run_full_benchmarks.sh (WARNING: takes hours/days)"
    echo ""
    exit 0
else
    echo -e "${RED}✗ Some tests FAILED${NC}"
    echo ""
    echo "Please check:"
    echo "  - Log files in $RESULTS_DIR/"
    echo "  - Ensure all components built successfully (./build_all.sh)"
    echo "  - See README.md for troubleshooting"
    echo ""
    exit 1
fi
