#!/bin/bash

# Full Benchmark Script for FHE Artifact
# Runs complete benchmark suite matching paper results
# WARNING: This takes 24-72 hours to complete!

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "========================================="
echo "FHE Artifact - Full Benchmark Suite"
echo "========================================="
echo ""
echo -e "${YELLOW}⚠ WARNING: This script runs the complete benchmark suite${NC}"
echo -e "${YELLOW}⚠ Expected runtime: 24-72 hours (depending on hardware)${NC}"
echo -e "${YELLOW}⚠ Resource requirements: 16GB+ RAM, 50GB+ disk space${NC}"
echo ""
echo "This will reproduce all results from the paper:"
echo "  - Figure 2: Workload benchmarks"
echo "  - Figure 4: Floyd-Warshall (graph algorithms)"
echo "  - Figure 5: Decision Tree evaluation"
echo "  - Figure 7: Sorting algorithms"
echo "  - Figure 8(b): Database aggregation"
echo ""
echo "Press Ctrl+C within 10 seconds to cancel..."
sleep 10

# Save starting directory
START_DIR=$(pwd)

# Create results directory
RESULTS_DIR="$START_DIR/full_benchmark_results"
mkdir -p "$RESULTS_DIR"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

echo ""
echo "Results will be saved to: $RESULTS_DIR/"
echo "Benchmark started at: $(date)"
echo ""

# Log file for summary
SUMMARY_LOG="$RESULTS_DIR/benchmark_summary_$TIMESTAMP.txt"
echo "FHE Artifact - Full Benchmark Results" > "$SUMMARY_LOG"
echo "Started: $(date)" >> "$SUMMARY_LOG"
echo "=========================================" >> "$SUMMARY_LOG"
echo "" >> "$SUMMARY_LOG"

# Function to run benchmark
run_benchmark() {
    local name=$1
    local dir=$2
    local cmd=$3

    echo "=========================================" | tee -a "$SUMMARY_LOG"
    echo -e "${BLUE}Running: $name${NC}" | tee -a "$SUMMARY_LOG"
    echo "=========================================" | tee -a "$SUMMARY_LOG"
    echo "Directory: $dir" | tee -a "$SUMMARY_LOG"
    echo "Command: $cmd" | tee -a "$SUMMARY_LOG"
    echo "Started: $(date)" | tee -a "$SUMMARY_LOG"
    echo "" | tee -a "$SUMMARY_LOG"

    local result_file="$RESULTS_DIR/${name// /_}_$TIMESTAMP.log"
    local start_time=$(date +%s)

    if cd "$dir" 2>/dev/null; then
        if eval "$cmd" > "$result_file" 2>&1; then
            local end_time=$(date +%s)
            local duration=$((end_time - start_time))
            local duration_human=$(date -u -d @${duration} +"%T")

            echo -e "${GREEN}✓ $name completed${NC}" | tee -a "$SUMMARY_LOG"
            echo "Duration: $duration_human" | tee -a "$SUMMARY_LOG"
            echo "Output saved to: $result_file" | tee -a "$SUMMARY_LOG"
            echo "" | tee -a "$SUMMARY_LOG"
            cd - > /dev/null
            return 0
        else
            local end_time=$(date +%s)
            local duration=$((end_time - start_time))
            local duration_human=$(date -u -d @${duration} +"%T")

            echo -e "${RED}✗ $name failed${NC}" | tee -a "$SUMMARY_LOG"
            echo "Duration before failure: $duration_human" | tee -a "$SUMMARY_LOG"
            echo "Output saved to: $result_file" | tee -a "$SUMMARY_LOG"
            echo "" | tee -a "$SUMMARY_LOG"
            cd - > /dev/null
            return 1
        fi
    else
        echo -e "${RED}✗ Directory $dir not found${NC}" | tee -a "$SUMMARY_LOG"
        echo "" | tee -a "$SUMMARY_LOG"
        return 1
    fi
}

echo "========================================="
echo "PART 1: TFHE Benchmarks (Rust)"
echo "========================================="
echo ""

run_benchmark "TFHE-Workloads" \
    "rust/tfhe-example/workloads" \
    "cargo run --release"

run_benchmark "TFHE-Sorting" \
    "rust/tfhe-example/sorting" \
    "cargo run --release"

run_benchmark "TFHE-Floyd-Warshall" \
    "rust/tfhe-example/floyd" \
    "cargo run --release"

run_benchmark "TFHE-Decision-Tree" \
    "rust/tfhe-example/decision_tree" \
    "cargo run --release"

run_benchmark "TFHE-Private-Database" \
    "rust/tfhe-example/private_db" \
    "cargo run --release"

echo "========================================="
echo "PART 2: Scheme Switching Benchmarks"
echo "========================================="
echo ""

run_benchmark "Scheme-Switching-Workload" \
    "scheme_switching/build/bin" \
    "./workload"

run_benchmark "Scheme-Switching-Decision-Tree" \
    "scheme_switching/build/bin" \
    "./decision_tree"

run_benchmark "Scheme-Switching-Sorting" \
    "scheme_switching/build/bin" \
    "./sorting"

run_benchmark "Scheme-Switching-Floyd-Warshall" \
    "scheme_switching/build/bin" \
    "./floyd_warshall"

run_benchmark "Scheme-Switching-Database" \
    "scheme_switching/build/bin" \
    "./database_aggregation"

echo "========================================="
echo "PART 3: Encoding Switching Benchmarks"
echo "========================================="
echo ""

run_benchmark "Encoding-Switching-Workload" \
    "encoding_switching/build/bin" \
    "./workload"

run_benchmark "Encoding-Switching-Decision-Tree" \
    "encoding_switching/build/bin" \
    "./decision_tree"

run_benchmark "Encoding-Switching-Sorting" \
    "encoding_switching/build/bin" \
    "./sorting"

run_benchmark "Encoding-Switching-Floyd-Warshall" \
    "encoding_switching/build/bin" \
    "./floyd_warshall"

run_benchmark "Encoding-Switching-Database" \
    "encoding_switching/build/bin" \
    "./database_aggregation"

# Return to starting directory
cd "$START_DIR"

# Final summary
echo "" | tee -a "$SUMMARY_LOG"
echo "=========================================" | tee -a "$SUMMARY_LOG"
echo "Full Benchmark Suite Completed" | tee -a "$SUMMARY_LOG"
echo "=========================================" | tee -a "$SUMMARY_LOG"
echo "Finished: $(date)" | tee -a "$SUMMARY_LOG"
echo "" | tee -a "$SUMMARY_LOG"
echo "Results saved in: $RESULTS_DIR/" | tee -a "$SUMMARY_LOG"
echo "Summary log: $SUMMARY_LOG" | tee -a "$SUMMARY_LOG"
echo "" | tee -a "$SUMMARY_LOG"

echo ""
echo -e "${GREEN}✓ Full benchmark suite completed!${NC}"
echo ""
echo "Next steps:"
echo "  - Review summary: cat $SUMMARY_LOG"
echo "  - Compare results with paper Figures 2, 4, 5, 7, 8(b)"
echo "  - Individual benchmark logs in $RESULTS_DIR/"
echo ""
