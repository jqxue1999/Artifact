# HEBridge Encoding Switching Benchmarks

This directory contains real FHE implementations of privacy-sensitive applications using **encoding switching** between FV and beFV in HElib (based on HE-Bridge).

## Overview

These benchmarks evaluate the paper's encoding switching approach, which switches between different plaintext moduli within the same FHE scheme:

- **FV Encoding** (plaintext modulus p^r) - Efficient for arithmetic operations (addition, multiplication)
- **beFV Encoding** (plaintext modulus p) - Efficient for logic operations via polynomial interpolation
- **Encoding Switching** - Converting between FV ↔ beFV to leverage both encodings' strengths

### Key Advantage: Exact Computation

Unlike scheme switching (CKKS ↔ FHEW), encoding switching stays within the same scheme (BGV/FV) and provides:
- **Exact integer arithmetic** (no approximation errors)
- **No LWE/RLWE conversion overhead** (stays in RLWE throughout)
- **Efficient SIMD** through packed plaintexts in FV
- **Fast comparison** via polynomial interpolation in beFV

## Benchmarks

### 1. **workload** - Basic Workloads

Three fundamental operation patterns:
- **Workload-1**: `(a*b) compare c` - Linear then non-linear (database queries)
- **Workload-2**: `(a compare b) * c` - Non-linear then linear (decision trees)
- **Workload-3**: `(a*b) compare (c*d)` - Mixed sequence (neural networks)

**Parameters**:
- Default: p=17, r=2 (8-bit plaintext space = 289)
- Encoding switching overhead: Reduction (FV→beFV) + Interpolation + Lifting (beFV→FV)

**Expected Runtime**: ~1-5 minutes per workload (depending on bit width)

### 2. **decision_tree** - Decision Tree Evaluation

Evaluates complete binary decision trees on encrypted data using oblivious path selection.

**Test Configuration** (from paper):
- Tree depths: 2, 4, 6, 8
- Bit widths: 6, 8, 12, 16

**Algorithm**:
1. Perform encrypted comparisons at all internal nodes (FV→beFV comparison→FV)
2. Compute path indicators for each leaf using encrypted multiplications in FV
3. Oblivious selection: sum (path_indicator * leaf_value)

**Expected Runtime**:
- Depth 2: ~5-15 minutes
- Depth 4: ~1-3 hours
- Depth 6: ~6-12 hours
- Depth 8: ~1-3 days

### 3. **sorting** - Private Sorting

Direct sorting algorithm on encrypted arrays (no SIMD batching).

**Test Configuration** (from paper):
- Array sizes: 8, 16, 32, 64 elements
- Default: 8-bit values

**Algorithm**:
1. Count positions: For each element, count how many are less than it via comparisons
2. Oblivious placement: Use encrypted equality checks to place elements

**Expected Runtime**:
- 8 elements: ~30-60 minutes
- 16 elements: ~2-4 hours
- 32 elements: ~8-16 hours
- 64 elements: ~1-3 days

### 4. **floyd_warshall** - All-Pairs Shortest Path

Floyd-Warshall algorithm on encrypted graphs.

**Test Configuration** (from paper):
- Graph sizes: 4, 8, 16, 32 nodes
- Default: 8-bit edge weights

**Algorithm**:
- Standard Floyd-Warshall with encrypted comparisons and oblivious min selection
- For each pair (i,j), compare current distance vs. path through intermediate node k

**Expected Runtime**:
- 4 nodes: ~5-15 minutes
- 8 nodes: ~30-90 minutes
- 16 nodes: ~4-8 hours
- 32 nodes: ~1-3 days

### 5. **database_aggregation** - Private Database Query

SQL query evaluation on encrypted database.

**Test Configuration** (from paper):
- Database sizes: 16, 32, 64, 128 rows
- All with 8-bit precision

**Query**:
```sql
SELECT ID FROM emp WHERE
  salary * work_hours BETWEEN 5000 AND 6000
  AND salary + bonus BETWEEN 700 AND 800
```

**Algorithm**:
- Process each row sequentially (no SIMD batching across rows)
- Evaluate predicates:
  - Predicate 1: 1 multiplication + 2 comparisons (range check)
  - Predicate 2: 1 addition + 2 comparisons (range check)
  - Combine with encrypted AND (multiplication)

**Expected Runtime**:
- 16 rows: ~15-30 minutes
- 32 rows: ~30-60 minutes
- 64 rows: ~1-2 hours
- 128 rows: ~2-4 hours

## Building

### Prerequisites

- HElib (built and installed) - **Already installed in this artifact**
- CMake 3.11+
- C++17 compatible compiler
- 8GB+ RAM (recommended for larger benchmarks)

### Build Steps

```bash
cd encoding_switching
mkdir -p build && cd build
cmake ..
make -j4
```

All executables will be in `build/bin/`:
```
bin/
├── workload                # Basic workloads
├── decision_tree           # Decision tree evaluation
├── sorting                 # Private sorting
├── floyd_warshall         # Shortest path
└── database_aggregation    # SQL query
```

## Running Benchmarks

### Evaluation Strategy

**Each benchmark is an independent executable.** You can run them in any order or select only the ones you're interested in evaluating.

### Default Parameters

All benchmarks use default parameters optimized for 8-bit computation:
- **p = 17**: Base prime modulus for beFV
- **r = 2**: Hensel lifting parameter (plaintext space = p^r = 289 ≈ 8-bit)
- **m = 13201**: Cyclotomic order
- **bits = 256**: Ciphertext modulus size
- **c = 2**: Key-switching columns
- **t = 64**: Hamming weight of secret key

### Running Benchmarks

```bash
cd build/bin

# 1. Basic workloads (~3-5 minutes total)
./workload

# 2. Decision tree (start with default: ~5-15 min for depth 2)
./decision_tree

# 3. Sorting (start with default: ~30-60 min for 8 elements)
./sorting

# 4. Floyd-Warshall (start with default: ~5-15 min for 4 nodes)
./floyd_warshall

# 5. Database (start with default: ~15-30 min for 16 rows)
./database_aggregation
```

### Customizing Parameters

You can override default parameters for different bit widths:

**6-bit (p=3, r=4):**
```bash
./workload p=3 r=4 m=16151 b=320 t=64
```

**8-bit (p=17, r=2):** (DEFAULT)
```bash
./workload p=17 r=2 m=13201 b=256 t=64
```

**12-bit (p=67, r=2):**
```bash
./workload p=67 r=2 m=31159 b=690 t=64
```

**16-bit (p=257, r=2):**
```bash
./workload p=257 r=2 m=77641 b=1000 t=64
```

## Understanding Output

### Example: Workload Output

```
================================================================================
HE-Bridge Encoding Switching Workload Benchmarks
================================================================================

Parameters:
  m=13201, p=17, r=2, bits=256, c=2, skHwt=64
  Plaintext space: p^r = 289

Initializing HE context...
  Cyclotomic order m = 13201
  ord(p) = 16
  Number of slots = 825

Workload-1: (a*b) compare c
--------------------------------------------------------------------------------
Configuration             Time                 Status
--------------------------------------------------------------------------------
8-bit                     2 min                ✓

Workload-2: (a compare b) * c
--------------------------------------------------------------------------------
Configuration             Time                 Status
--------------------------------------------------------------------------------
8-bit                     3 min                ✓

Workload-3: (a*b) compare (c*d)
--------------------------------------------------------------------------------
Configuration             Time                 Status
--------------------------------------------------------------------------------
8-bit                     2 min                ✓
```

**Interpretation**:
- **Time**: Total time for complete workload execution
- **Status**: ✓ means execution completed successfully
- **Slots**: Number of SIMD slots available (depends on m and ord(p))

## Performance Notes

### Why Different from Scheme Switching?

**Encoding Switching** (this implementation):
- Stays within BGV/FV scheme
- Exact integer arithmetic
- No LWE/RLWE conversion overhead
- Comparison via polynomial interpolation in F_p
- Each comparison takes ~10-30 seconds (vs ~5-7 minutes for scheme switching)

**Scheme Switching** (OpenFHE implementation):
- Switches between CKKS (approximate) and FHEW (exact)
- LWE/RLWE conversion is expensive
- Each comparison takes ~5-7 minutes
- Benefits from SIMD batching (128 slots)

### Key Performance Factors

1. **Bit Width Impact**:
   - 6-bit: Fast (~10s per comparison)
   - 8-bit: Moderate (~20s per comparison)
   - 12-bit: Slower (~40s per comparison)
   - 16-bit: Slowest (~60s per comparison)

2. **Comparison Cost**:
   - Reduction: FV (mod p^r) → beFV (mod p)
   - Interpolation: Polynomial evaluation of degree p in beFV
   - Lifting: beFV → FV

3. **SIMD Opportunities**:
   - Encoding switching can use SIMD within FV operations
   - But comparison happens element-wise (no SIMD across comparisons)
   - Best for applications with moderate comparison frequency

## Comparison with Other Methods

From the paper's findings:

| Method | Comparison Time | SIMD Support | Arithmetic | Best For |
|--------|-----------------|--------------|------------|----------|
| **TFHE** | ~10-20ms/gate | None | Bit-wise | Non-linear heavy |
| **Scheme Switching** | ~5-7 min | Yes (128x) | Mixed | Batch queries |
| **Encoding Switching** | ~10-30 sec | Limited | Exact | Moderate mixed ops |

**Encoding Switching wins when**:
- Exact integer arithmetic required
- Moderate number of comparisons
- Cannot leverage SIMD batching effectively

## Troubleshooting

### Slow Performance

Expected! Encoding switching comparisons are slower than TFHE gates but much faster than scheme switching. The exact arithmetic and simpler conversion make up for the lack of full SIMD batching.

### Out of Memory

Large benchmarks require significant RAM:
- 16 nodes Floyd-Warshall: ~1-2GB
- 32 nodes Floyd-Warshall: ~4-6GB
- 64 nodes Floyd-Warshall: ~8-12GB

Reduce problem size or run on a machine with more RAM.

### Parameter Selection

The default parameters are chosen to balance:
- Security (128-bit classical security)
- Performance (moderate computation time)
- Bit width (8-bit supports values 0-288)

For different bit widths, use the parameter sets listed above.

### Long Runtimes

Expected! Full benchmarks take hours to days. Plan accordingly:
- Start with smallest configurations
- Run overnight for medium benchmarks
- Use `screen` or `tmux` to keep sessions alive

## Implementation Details

### File Structure

```
encoding_switching/
├── src/
│   ├── bridge.h/cpp        # HE-Bridge core (from HE-Bridge)
│   ├── tools.h/cpp         # Utility functions (from HE-Bridge)
│   ├── Ctxt_ext.cpp        # Extensions to HElib Ctxt
│   ├── ArgMapping.h        # Command-line argument parsing
│   ├── workload.cpp        # Basic workloads
│   ├── decision_tree.cpp   # Decision tree (no SIMD batching)
│   ├── sorting.cpp         # Sorting (sequential)
│   ├── floyd_warshall.cpp  # Floyd (sequential)
│   └── database_aggregation.cpp # Database (sequential per row)
├── CMakeLists.txt          # Build configuration
└── README.md              # This file
```

### Key Operations

**Encoding Switching Flow**:
1. **FV Operations**: Encrypted arithmetic in plaintext space p^r
2. **Reduction**: Convert from p^r to p (extract base-p digits)
3. **Comparison**: Polynomial interpolation in F_p (degree p)
4. **Lifting**: Convert result from p back to p^r
5. **Continue in FV**: Further arithmetic operations

**HE-Bridge Functions**:
- `Bridge::compare()`: Main comparison function (performs reduction, interpolation, result in beFV)
- `Bridge::reduce()`: FV → beFV (extract base-p digits)
- `Bridge::lift()`: beFV → FV (lift result)

### Security Parameters

All implementations use:
- **BGV/FV Security**: 128-bit classical security
- **Ring Dimension**: Automatically determined by HElib based on (m, p, r)
- **Plaintext Space**: p^r (exact integer arithmetic modulo p^r)

## References

- Paper: "SoK: Can Fully Homomorphic Encryption Support General AI Computation?"
- HE-Bridge: "HEBridge: Connecting Arithmetic and Logic Operations in FV-style HE Schemes" (CCS WAHC'24)
- HElib: https://github.com/homenc/HElib
- Original HE-Bridge Code: https://github.com/UCF-Lou-Lab-PET/HE-Bridge

## Contact

For questions about these implementations, please refer to the main artifact README or contact the paper authors.
