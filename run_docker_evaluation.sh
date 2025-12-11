#!/bin/bash
################################################################################
# FHE Artifact Docker Evaluation Script
#
# This script builds and runs the complete FHE artifact evaluation in Docker.
# It tests all three FHE implementations: TFHE, Scheme Switching, and Encoding Switching.
#
# Usage:
#   ./run_docker_evaluation.sh [quick|full]
#
# Options:
#   quick - Run quick tests (~10-20 minutes)
#   full  - Run full benchmarks (several hours)
################################################################################

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

# Parse arguments
MODE="${1:-quick}"
if [[ "$MODE" != "quick" && "$MODE" != "full" ]]; then
    echo -e "${RED}Error: Invalid mode '$MODE'. Use 'quick' or 'full'.${NC}"
    exit 1
fi

echo "================================================================================"
echo "  FHE Artifact Docker Evaluation"
echo "  Mode: $MODE"
echo "================================================================================"
echo ""

# Step 1: Build Docker image
echo -e "${BLUE}[1/5] Building Docker image...${NC}"
echo "This will take 20-40 minutes depending on your system."
echo "Building TFHE (Rust), OpenFHE (Scheme Switching), and HElib (Encoding Switching)..."
echo ""

if ! docker-compose build 2>&1 | tee /tmp/docker_build_evaluation.log; then
    echo -e "${RED}Error: Docker build failed. Check /tmp/docker_build_evaluation.log for details.${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Docker image built successfully${NC}"
echo ""

# Step 2: Start container
echo -e "${BLUE}[2/5] Starting Docker container...${NC}"
docker-compose up -d
sleep 3

if ! docker ps | grep -q fhe-artifact; then
    echo -e "${RED}Error: Container failed to start${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Container started${NC}"
echo ""

# Step 3: Verify build
echo -e "${BLUE}[3/5] Verifying all executables are present...${NC}"

# Check TFHE
echo -n "  Checking TFHE executables... "
if docker exec fhe-artifact ls /artifact/rust/tfhe-example/workloads/target/debug/tfhe-example &>/dev/null; then
    echo -e "${GREEN}✓${NC}"
else
    echo -e "${RED}✗ Missing${NC}"
    exit 1
fi

# Check Scheme Switching
echo -n "  Checking Scheme Switching executables... "
if docker exec fhe-artifact ls /artifact/scheme_switching/build/bin/workload &>/dev/null; then
    echo -e "${GREEN}✓${NC}"
else
    echo -e "${RED}✗ Missing${NC}"
    exit 1
fi

# Check Encoding Switching
echo -n "  Checking Encoding Switching executables... "
if docker exec fhe-artifact ls /artifact/encoding_switching/build/bin/workload &>/dev/null; then
    echo -e "${GREEN}✓${NC}"
else
    echo -e "${RED}✗ Missing${NC}"
    exit 1
fi

echo ""

# Step 4: Run tests
echo -e "${BLUE}[4/5] Running FHE benchmarks...${NC}"
echo ""

# Create results directory
RESULTS_DIR="$SCRIPT_DIR/results_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$RESULTS_DIR"

if [[ "$MODE" == "quick" ]]; then
    echo -e "${YELLOW}Running quick tests (estimated 10-20 minutes)...${NC}"
    echo ""

    # TFHE Quick Test
    echo -e "${BLUE}Testing TFHE implementation...${NC}"
    docker exec fhe-artifact timeout 300 /artifact/rust/tfhe-example/workloads/target/debug/tfhe-example | tee "$RESULTS_DIR/tfhe_workloads.txt"
    echo -e "${GREEN}✓ TFHE test completed${NC}"
    echo ""

    # Scheme Switching Quick Test
    echo -e "${BLUE}Testing Scheme Switching implementation...${NC}"
    docker exec fhe-artifact /artifact/scheme_switching/build/bin/test_basic | tee "$RESULTS_DIR/scheme_switching_basic.txt"
    echo -e "${GREEN}✓ Scheme Switching test completed${NC}"
    echo ""

    # Encoding Switching Quick Test
    echo -e "${BLUE}Testing Encoding Switching implementation...${NC}"
    docker exec fhe-artifact timeout 600 bash -c "cd /artifact/encoding_switching/build/bin && ./workload | head -100" | tee "$RESULTS_DIR/encoding_switching_workload.txt"
    echo -e "${GREEN}✓ Encoding Switching test completed${NC}"
    echo ""

else
    echo -e "${YELLOW}Running full benchmarks (this will take several hours)...${NC}"
    echo ""

    # TFHE Full Benchmarks
    echo -e "${BLUE}[1/15] TFHE - Workloads${NC}"
    docker exec fhe-artifact /artifact/rust/tfhe-example/workloads/target/debug/tfhe-example | tee "$RESULTS_DIR/tfhe_workloads.txt"

    echo -e "${BLUE}[2/15] TFHE - Sorting${NC}"
    docker exec fhe-artifact /artifact/rust/tfhe-example/sorting/target/debug/sorting | tee "$RESULTS_DIR/tfhe_sorting.txt"

    echo -e "${BLUE}[3/15] TFHE - Floyd-Warshall${NC}"
    docker exec fhe-artifact /artifact/rust/tfhe-example/floyd/target/debug/floyd | tee "$RESULTS_DIR/tfhe_floyd.txt"

    echo -e "${BLUE}[4/15] TFHE - Decision Tree${NC}"
    docker exec fhe-artifact /artifact/rust/tfhe-example/decision_tree/target/debug/decision_tree | tee "$RESULTS_DIR/tfhe_decision_tree.txt"

    echo -e "${BLUE}[5/15] TFHE - Private Database${NC}"
    docker exec fhe-artifact /artifact/rust/tfhe-example/private_db/target/debug/private_db | tee "$RESULTS_DIR/tfhe_private_db.txt"

    # Scheme Switching Full Benchmarks
    echo -e "${BLUE}[6/15] Scheme Switching - Workloads${NC}"
    docker exec fhe-artifact /artifact/scheme_switching/build/bin/workload | tee "$RESULTS_DIR/scheme_switching_workloads.txt"

    echo -e "${BLUE}[7/15] Scheme Switching - Decision Tree${NC}"
    docker exec fhe-artifact /artifact/scheme_switching/build/bin/decision_tree | tee "$RESULTS_DIR/scheme_switching_decision_tree.txt"

    echo -e "${BLUE}[8/15] Scheme Switching - Sorting${NC}"
    docker exec fhe-artifact /artifact/scheme_switching/build/bin/sorting | tee "$RESULTS_DIR/scheme_switching_sorting.txt"

    echo -e "${BLUE}[9/15] Scheme Switching - Floyd-Warshall${NC}"
    docker exec fhe-artifact /artifact/scheme_switching/build/bin/floyd_warshall | tee "$RESULTS_DIR/scheme_switching_floyd.txt"

    echo -e "${BLUE}[10/15] Scheme Switching - Database Aggregation${NC}"
    docker exec fhe-artifact /artifact/scheme_switching/build/bin/database_aggregation | tee "$RESULTS_DIR/scheme_switching_database.txt"

    # Encoding Switching Full Benchmarks
    echo -e "${BLUE}[11/15] Encoding Switching - Workloads${NC}"
    docker exec fhe-artifact /artifact/encoding_switching/build/bin/workload | tee "$RESULTS_DIR/encoding_switching_workloads.txt"

    echo -e "${BLUE}[12/15] Encoding Switching - Decision Tree${NC}"
    docker exec fhe-artifact /artifact/encoding_switching/build/bin/decision_tree | tee "$RESULTS_DIR/encoding_switching_decision_tree.txt"

    echo -e "${BLUE}[13/15] Encoding Switching - Sorting${NC}"
    docker exec fhe-artifact /artifact/encoding_switching/build/bin/sorting | tee "$RESULTS_DIR/encoding_switching_sorting.txt"

    echo -e "${BLUE}[14/15] Encoding Switching - Floyd-Warshall${NC}"
    docker exec fhe-artifact /artifact/encoding_switching/build/bin/floyd_warshall | tee "$RESULTS_DIR/encoding_switching_floyd.txt"

    echo -e "${BLUE}[15/15] Encoding Switching - Database Aggregation${NC}"
    docker exec fhe-artifact /artifact/encoding_switching/build/bin/database_aggregation | tee "$RESULTS_DIR/encoding_switching_database.txt"

    echo ""
fi

# Step 5: Generate summary
echo -e "${BLUE}[5/5] Generating summary report...${NC}"

cat > "$RESULTS_DIR/SUMMARY.md" << 'EOF'
# FHE Artifact Evaluation Summary

This directory contains the results from evaluating the FHE artifact implementations.

## Implementations Tested

1. **TFHE (Bit-wise FHE)** - Rust implementation using TFHE-rs library
2. **Scheme Switching** - C++ implementation using OpenFHE (CKKS ↔ FHEW)
3. **Encoding Switching** - C++ implementation using HElib (FV ↔ beFV)

## Result Files

### TFHE Results
- `tfhe_workloads.txt` - Basic workload benchmarks (mul_cp, cp_mul, mul_cp_mul)
- `tfhe_sorting.txt` - Sorting algorithm benchmarks
- `tfhe_floyd.txt` - Floyd-Warshall shortest path benchmarks
- `tfhe_decision_tree.txt` - Decision tree evaluation benchmarks
- `tfhe_private_db.txt` - Private database aggregation benchmarks

### Scheme Switching Results
- `scheme_switching_basic.txt` - Basic functionality tests
- `scheme_switching_workloads.txt` - Workload benchmarks
- `scheme_switching_decision_tree.txt` - Decision tree benchmarks
- `scheme_switching_sorting.txt` - Sorting benchmarks
- `scheme_switching_floyd.txt` - Floyd-Warshall benchmarks
- `scheme_switching_database.txt` - Database aggregation benchmarks

### Encoding Switching Results
- `encoding_switching_workload.txt` - Workload benchmarks
- `encoding_switching_decision_tree.txt` - Decision tree benchmarks
- `encoding_switching_sorting.txt` - Sorting benchmarks
- `encoding_switching_floyd.txt` - Floyd-Warshall benchmarks
- `encoding_switching_database.txt` - Database aggregation benchmarks

## Interpreting Results

### Performance Metrics
- **Time**: Wall-clock time for encrypted computation
- **Bit Width**: Size of encrypted integers (6, 8, 12, 16 bits)
- **Parameters**: FHE scheme-specific parameters (p, r, m, etc.)

### Key Findings
- TFHE is efficient for non-linear operations but lacks SIMD support
- Scheme Switching benefits from SIMD but has high switching overhead
- Encoding Switching provides best balance for mixed workloads (per paper)

### Expected Performance Patterns
- Performance degrades with increased bit width
- Scheme switching becomes impractical beyond 12-bit
- TFHE scales better to 16-bit operations
- Applications with frequent linear/non-linear transitions favor encoding switching

## Verification Checklist

- [ ] All three implementations build successfully
- [ ] TFHE workloads execute and produce timing results
- [ ] Scheme switching produces correct encrypted comparisons
- [ ] Encoding switching uses custom HElib methods (multiplyModByP2R, divideModByP)
- [ ] Results align with paper's performance trends

## Paper Reference

"SoK: Can Fully Homomorphic Encryption Support General AI Computation?
A Functional and Cost Analysis"

This artifact demonstrates the evaluation methodology and performance
characteristics described in the paper.
EOF

echo -e "${GREEN}✓ Summary generated at $RESULTS_DIR/SUMMARY.md${NC}"
echo ""

# Final summary
echo "================================================================================"
echo -e "${GREEN}  Evaluation Complete!${NC}"
echo "================================================================================"
echo ""
echo "Results saved to: $RESULTS_DIR"
echo ""
echo "Next steps:"
echo "  1. Review results in $RESULTS_DIR/"
echo "  2. Check SUMMARY.md for interpretation guide"
echo "  3. Compare performance metrics across implementations"
echo ""
echo "To stop the container:"
echo "  docker-compose down"
echo ""
echo "To rebuild and re-run:"
echo "  ./run_docker_evaluation.sh $MODE"
echo ""
