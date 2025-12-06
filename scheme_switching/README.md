# OpenFHE Scheme Switching Benchmarks

This directory contains real FHE implementations of privacy-sensitive applications using **scheme switching** between CKKS and FHEW in OpenFHE.

## Overview

These benchmarks evaluate the paper's scheme switching approach, which combines:
- **CKKS** - Efficient for linear operations (addition, multiplication) with SIMD batching
- **FHEW** - Efficient for non-linear operations (comparisons, boolean logic)
- **Scheme Switching** - Converting between CKKS ↔ FHEW to leverage both schemes' strengths

### Key Advantage: SIMD Batching

Unlike bit-wise TFHE (which encrypts each bit separately), scheme switching can process **up to 128 values in parallel** using SIMD slots in CKKS ciphertexts.

## Benchmarks

### 1. **workload** - Basic Workloads
Three fundamental operation patterns:
- **Workload-1**: `(a*b) compare c` - Linear then non-linear (database queries)
- **Workload-2**: `(a compare b) * c` - Non-linear then linear (decision trees)
- **Workload-3**: `(a*b) compare (c*d)` - Mixed sequence (neural networks)

**SIMD Usage**: 128 copies of same computation (broadcast)

**Test Configuration**:
- Bit widths: 6, 8, 12, 16
- SIMD slots: 128

**Expected Runtime**: ~30-60 minutes per workload

### 2. **decision_tree** - Decision Tree Evaluation
Evaluates complete binary decision trees on encrypted data using oblivious path selection.

**SIMD Usage**: **128 different input samples** evaluated simultaneously

**Test Configuration** (from paper):
- Tree depths: 2, 4, 6, 8
- Bit widths: 6, 8, 12, 16
- Each depth with all bit widths

**Algorithm**:
1. Perform encrypted comparisons at all internal nodes (CKKS→FHEW→CKKS)
2. Compute path indicators for each leaf using encrypted multiplications
3. Oblivious selection: sum (path_indicator * leaf_value)

**Expected Runtime**:
- Depth 2, 6-bit: ~10-15 minutes (128 inputs)
- Depth 4, 6-bit: ~2-4 hours (128 inputs)
- Depth 6, 8-bit: ~1-2 days (128 inputs)
- Depth 8, 16-bit: ~3-5 days (128 inputs)

### 3. **sorting** - Private Sorting
Direct sorting algorithm on encrypted arrays.

**SIMD Usage**: None (single array is inherently sequential)

**Test Configuration** (from paper):
- **Experiment 1**: 8 elements with 6, 8, 12, 16-bit
- **Experiment 2**: 8, 16, 32, 64 elements with 8-bit

**Algorithm**:
1. Count positions: For each element, count how many are less than it
2. Oblivious placement: Use encrypted equality checks to place elements

**Expected Runtime**:
- 8 elements, 6-bit: ~30-60 minutes
- 16 elements, 8-bit: ~4-8 hours
- 32 elements, 8-bit: ~1-2 days
- 64 elements, 8-bit: ~4-7 days

### 4. **floyd_warshall** - All-Pairs Shortest Path
Floyd-Warshall algorithm on encrypted graphs.

**SIMD Usage**: Up to 128 columns per row (full SIMD)

**Test Configuration** (from paper):
- **Experiment 1**: 32 nodes with 6, 8, 12, 16-bit
- **Experiment 2**: 16, 32, 64, 128 nodes with 8-bit

**Algorithm**:
- Each row is packed into one CKKS ciphertext (up to 128 nodes)
- For each intermediate node k and source node i:
  - Compute candidate distances: D[i,k] + D[k,:]
  - Compare with current distances (scheme switching)
  - Oblivious selection of minimum

**Expected Runtime**:
- 16 nodes, 6-bit: ~1-2 hours
- 32 nodes, 8-bit: ~8-12 hours
- 64 nodes, 8-bit: ~2-4 days
- 128 nodes, 8-bit: ~1-2 weeks

### 5. **database_aggregation** - Private Database Query
SQL query evaluation on encrypted database.

**SIMD Usage**: 128 database rows per batch (full SIMD)

**Test Configuration** (from paper):
- Database sizes: 64, 128, 256, 512 rows
- All with 8-bit precision

**Query**:
```sql
SELECT ID FROM emp WHERE
  salary * work_hours BETWEEN 5000 AND 6000
  AND salary + bonus BETWEEN 700 AND 800
```

**Algorithm**:
- Pack 128 rows into each ciphertext using SIMD
- Evaluate predicates:
  - Predicate 1: 1 multiplication + 2 comparisons (range check)
  - Predicate 2: 1 addition + 2 comparisons (range check)
  - Combine with encrypted AND (multiplication)

**Expected Runtime**:
- 64 rows: ~1-2 hours
- 128 rows: ~2-4 hours
- 256 rows: ~4-8 hours
- 512 rows: ~8-16 hours

## Building

### Prerequisites

- OpenFHE 1.2.0+ installed
- CMake 3.16.3+
- C++17 compatible compiler
- 16GB+ RAM (recommended for larger benchmarks)

### Build Steps

```bash
cd scheme_switching
mkdir -p build && cd build
cmake ..
make -j4
```

All executables will be in `build/bin/`:
```
bin/
├── workload                 # Basic workloads
├── decision_tree            # Decision tree evaluation
├── sorting                  # Private sorting
├── floyd_warshall          # Shortest path
├── database_aggregation    # SQL query
├── test_basic              # Quick test (~20 seconds)
└── test_decision_tree_small # Small test (~5 minutes)
```

## Running Benchmarks

### Evaluation Strategy

**Each benchmark is an independent executable.** You can run them in any order or select only the ones you're interested in evaluating.

### Step 1: Quick Verification

Test that everything works (runs in seconds):
```bash
cd build/bin
./test_basic
```

Expected output:
```
Testing basic OpenFHE scheme switching setup...
✓ Crypto context setup successful
✓ Encryption successful
✓ Decryption successful
✓ Scheme switching comparison successful
✓ All basic tests passed!
```

### Step 2: Small Test (Optional)

Test decision tree with SIMD (runs in ~5-10 minutes):
```bash
./test_decision_tree_small
```

This evaluates a depth-2 tree on 128 different inputs using SIMD batching.

### Step 3: Choose Benchmarks to Evaluate

Select which applications you want to benchmark based on your available time and research interests. **Each runs independently:**

### Full Benchmarks

**IMPORTANT**: Each benchmark is a **separate executable**. Run them individually as needed.

**WARNING**: Full benchmarks take hours to days. Start with smallest configurations.

```bash
# Run each benchmark separately - choose the ones you want to evaluate:

# 1. Basic workloads (~30-60 min per workload)
./workload

# 2. Decision tree (start with depth 2, 6-bit: ~10-15 min)
./decision_tree

# 3. Sorting (start with 8 elements, 6-bit: ~30-60 min)
./sorting

# 4. Floyd-Warshall (start with 16 nodes, 6-bit: ~1-2 hours)
./floyd_warshall

# 5. Database (relatively fast: ~1-2 hours for 64 rows)
./database_aggregation
```

**Note**: There is NO "run all benchmarks" command. Each application must be run separately, allowing you to choose which benchmarks to evaluate based on your available time.

### Running Partial Benchmarks

To test specific configurations, you can modify the main() function in each source file to run only selected experiments. For example, in `decision_tree.cpp`:

```cpp
// Change this:
vector<uint32_t> depths = {2, 4, 6, 8};
vector<uint32_t> bit_widths = {6, 8, 12, 16};

// To this (test only depth 2 with 6-bit):
vector<uint32_t> depths = {2};
vector<uint32_t> bit_widths = {6};
```

Then rebuild:
```bash
make decision_tree
```

## Understanding Output

### Example: Decision Tree Output

```
================================================================================
OpenFHE Scheme Switching Decision Tree Evaluation
================================================================================

Evaluating decision trees with SIMD batching (128 inputs per batch)
Using scheme switching between CKKS and FHEW

Decision Tree Depth 2 (3 nodes, 4 leaves)
--------------------------------------------------------------------------------
Bit Width      Time (128 inputs)    Comparisons    Status
--------------------------------------------------------------------------------
6              12 min               3              ✓
8              15 min               3              ✓
12             25 min               3              ✓
16             40 min               3              ✓
```

**Interpretation**:
- **Time**: Total time to evaluate 128 different inputs simultaneously
- **Comparisons**: Number of encrypted comparisons (same for all bit widths)
- **Per-input time**: Divide by 128 to get average time per single input
- **Status**: ✓ means execution completed successfully

### SIMD Efficiency

The key advantage of scheme switching is SIMD batching:

| Application | Inputs per Batch | SIMD Efficiency |
|-------------|------------------|-----------------|
| **decision_tree** | 128 different inputs | **128x speedup** |
| **floyd_warshall** | Up to 128 columns | **Up to 128x** |
| **database_aggregation** | 128 rows | **128x speedup** |
| **sorting** | 1 array | **No SIMD benefit** |
| **workload** | 128 same values | Baseline measurement |

## Security Parameters

All implementations use:
- **CKKS Security**: `HEStd_128_classic` (128-bit classical security)
- **FHEW Security**: `STD128` (128-bit security)
- **Ring Dimension**: Automatically determined by OpenFHE based on security level
- **SIMD Slots**: 128 (maximum for current parameters)

## Performance Notes

### Why So Slow?

Each encrypted comparison requires:
1. **CKKS subtraction**: a - b (fast, ~milliseconds)
2. **CKKS→FHEW switching**: Convert to bit-wise encryption (~2-3 minutes)
3. **FHEW sign evaluation**: Extract sign bit (~2-3 minutes)
4. **FHEW→CKKS switching**: Convert back to packed encryption (~30-60 seconds)

**Total per comparison**: ~5-7 minutes

Even with SIMD batching, this is the fundamental bottleneck of scheme switching.

### Comparison with TFHE

From the paper's findings:
- **TFHE**: No SIMD, but faster per-bit operations (~10-20ms per gate)
- **Scheme Switching**: Slow comparisons (~5 min), but 128x SIMD parallelism
- **Winner depends on**:
  - SIMD opportunities (scheme switching wins when batching works)
  - Operation mix (TFHE wins for mostly non-linear operations)
  - Array size vs. batch processing

## Troubleshooting

### Slow Performance

This is expected! Scheme switching comparisons are inherently slow (~5 min each). The SIMD batching provides efficiency through parallelism, not faster individual operations.

### Out of Memory

Large benchmarks require significant RAM:
- 16 nodes Floyd-Warshall: ~2GB
- 64 nodes Floyd-Warshall: ~8GB
- 128 nodes Floyd-Warshall: ~16GB+

Reduce problem size or run on a machine with more RAM.

### Verification Failures

Small numerical errors (< 1.0) are expected due to FHE approximations in CKKS. The code includes tolerance thresholds.

### Long Runtimes

Expected! Plan accordingly:
- Run overnight for medium benchmarks
- Run for days for large benchmarks
- Use `screen` or `tmux` to keep sessions alive
- Monitor progress by checking output files

## Implementation Details

### File Structure

```
scheme_switching/
├── src/
│   ├── utils.cpp/h          # Shared crypto setup and comparison
│   ├── workload.cpp         # Basic workloads
│   ├── decision_tree.cpp    # Decision tree (SIMD: 128 inputs)
│   ├── sorting.cpp          # Sorting (no SIMD)
│   ├── floyd_warshall.cpp   # Floyd (SIMD: row batching)
│   ├── database_aggregation.cpp # Database (SIMD: 128 rows)
│   ├── test_basic.cpp       # Quick verification test
│   └── test_decision_tree_small.cpp # Small decision tree test
├── CMakeLists.txt           # Build configuration
└── README.md               # This file
```

### Key Functions

**`utils.cpp`**:
- `SetupCryptoContext()`: Initialize CKKS and FHEW contexts with scheme switching
- `Comparison()`: Perform encrypted comparison via CKKS→FHEW→CKKS

**Each benchmark**:
- Generates encrypted test data using SIMD batching where applicable
- Performs actual encrypted computations (not simulated!)
- Measures real execution time
- Verifies correctness for small inputs

## References

- Paper: "SoK: Can Fully Homomorphic Encryption Support General AI Computation?"
- OpenFHE Documentation: https://openfhe-development.readthedocs.io/
- Scheme Switching Guide: https://openfhe.discourse.group/

## Contact

For questions about these implementations, please refer to the main artifact README or contact the paper authors.
