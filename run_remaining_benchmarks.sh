#!/bin/bash

# Run Remaining Benchmarks Script
# Verifies ALL missing benchmark types using quick_all tests
# Covers: Workload, Decision Tree, Sorting, Floyd-Warshall, Database
# Uses minimal parameters for fast verification (~5-10 minutes total)

# NOTE: We don't use "set -e" so that failures don't stop subsequent tests

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "========================================="
echo "FHE Artifact - Run Remaining Benchmarks"
echo "========================================="
echo ""
echo "This script verifies ALL missing benchmark types:"
echo "  - Scheme Switching: Workload, Decision Tree, Sorting, Floyd-Warshall, Database"
echo "  - Encoding Switching: Workload, Decision Tree, Sorting, Floyd-Warshall, Database"
echo ""
echo "Uses minimal parameters (6-bit, small problem sizes) for fast verification."
echo ""
echo "NOTE: TFHE benchmarks are already complete (4/5)"
echo "NOTE: TFHE-Floyd-Warshall is SKIPPED (takes ~30 days)"
echo ""
echo "Expected runtime: 5-10 minutes"
echo ""

# Save starting directory
START_DIR=$(pwd)

# Create results directory
RESULTS_DIR="$START_DIR/remaining_benchmark_results"
mkdir -p "$RESULTS_DIR"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

echo "Results will be saved to: $RESULTS_DIR/"
echo "Started at: $(date)"
echo ""

# Log file - capture all output
LOG_FILE="$RESULTS_DIR/remaining_benchmarks_$TIMESTAMP.log"
exec > >(tee -a "$LOG_FILE") 2>&1

# Status tracking
STATUS_FILE="$RESULTS_DIR/.status"
PASS=0
FAIL=0
TOTAL=2  # 1 scheme switching + 1 encoding switching (each covers 5 benchmarks)

update_status() {
    local current=$1
    local name=$2
    echo "Progress: $current/$TOTAL" > "$STATUS_FILE"
    echo "Current: $name" >> "$STATUS_FILE"
    echo "Updated: $(date)" >> "$STATUS_FILE"
}

# Function to run benchmark with timeout
run_benchmark() {
    local name=$1
    local dir=$2
    local cmd=$3
    local timeout_mins=${4:-10}  # Default 10 min timeout

    echo "========================================="
    echo -e "${BLUE}Running: $name${NC}"
    echo "========================================="
    echo "Directory: $dir"
    echo "Command: $cmd"
    echo "Timeout: ${timeout_mins} minutes"
    echo "Started: $(date)"
    echo ""

    local current_num=$((PASS + FAIL + 1))
    update_status "$current_num" "$name"

    local result_file="$RESULTS_DIR/${name}_$TIMESTAMP.log"
    local start_time=$(date +%s)

    if ! cd "$dir" 2>/dev/null; then
        echo -e "${RED}✗ Directory not found: $dir${NC}"
        echo ""
        FAIL=$((FAIL + 1))
        cd "$START_DIR" 2>/dev/null || true
        return 1
    fi

    # Run with timeout
    local exit_code=0
    timeout "${timeout_mins}m" bash -c "$cmd" > "$result_file" 2>&1 || exit_code=$?

    local end_time=$(date +%s)
    local duration=$((end_time - start_time))

    if [ $exit_code -eq 0 ]; then
        echo -e "${GREEN}✓ $name PASSED${NC} (${duration}s)"
        echo "Output: $result_file"
        echo ""
        # Show results summary
        echo "--- Results Summary ---"
        cat "$result_file" | grep -E "(PASSED|✓|All.*tests)" | head -10
        echo ""
        PASS=$((PASS + 1))
    elif [ $exit_code -eq 124 ]; then
        echo -e "${YELLOW}⏱ $name TIMEOUT${NC} (${timeout_mins}min limit)"
        echo "Last 20 lines of output:"
        tail -20 "$result_file" 2>/dev/null || echo "(no output)"
        echo ""
        FAIL=$((FAIL + 1))
    else
        echo -e "${RED}✗ $name FAILED${NC} (exit code: $exit_code)"
        echo "Last 20 lines of output:"
        tail -20 "$result_file" 2>/dev/null || echo "(no output)"
        echo ""
        FAIL=$((FAIL + 1))
    fi

    cd "$START_DIR" 2>/dev/null || true
    return 0  # Always return success to continue with next benchmark
}

echo "========================================="
echo "PART 1: Scheme Switching - All Benchmarks"
echo "========================================="
echo ""
echo "Testing: Workload, Decision Tree, Sorting, Floyd-Warshall, Database"
echo "Parameters: 6-bit, 8 SIMD slots, minimal sizes"
echo ""

run_benchmark "Scheme-Switching-All" \
    "scheme_switching/build/bin" \
    "./quick_all" \
    10

echo "========================================="
echo "PART 2: Encoding Switching - All Benchmarks"
echo "========================================="
echo ""
echo "Testing: Workload, Decision Tree, Sorting, Floyd-Warshall, Database"
echo "Parameters: 6-bit, minimal sizes"
echo ""

run_benchmark "Encoding-Switching-All" \
    "encoding_switching/build/bin" \
    "./quick_all" \
    10

# Summary
echo ""
echo "========================================="
echo "SUMMARY"
echo "========================================="
echo "Finished: $(date)"
echo ""
echo -e "Passed: ${GREEN}$PASS${NC}/$TOTAL"
if [ $FAIL -gt 0 ]; then
    echo -e "Failed:  ${RED}$FAIL${NC}/$TOTAL"
fi
echo ""
echo "Results saved in: $RESULTS_DIR/"
echo ""

# Detailed coverage
echo "========================================="
echo "Benchmark Coverage"
echo "========================================="
echo ""
echo "Each test suite verifies 5 benchmark types:"
echo "  1. Workload (a*b compare c)"
echo "  2. Decision Tree (depth 2)"
echo "  3. Sorting (4 elements)"
echo "  4. Floyd-Warshall (4 nodes)"
echo "  5. Database Query"
echo ""

# Final status
if [ $FAIL -eq 0 ]; then
    echo -e "${GREEN}✓ All remaining benchmarks verified successfully!${NC}"
    echo ""
    echo "Both Scheme Switching and Encoding Switching work correctly"
    echo "for all 5 benchmark types (10 total verifications)."
    echo ""
    echo "SUCCESS: $PASS/$TOTAL test suites passed (10/10 benchmarks verified)" > "$STATUS_FILE"
    exit 0
else
    echo -e "${YELLOW}⚠ Some tests failed${NC}"
    echo "Check individual log files for details."
    echo "PARTIAL: $PASS passed, $FAIL failed out of $TOTAL" > "$STATUS_FILE"
    exit 0  # Exit 0 so nohup doesn't report failure
fi
