# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository Overview

This is the artifact repository for the paper "SoK: Can Fully Homomorphic Encryption Support General AI Computation? A Functional and Cost Analysis". The repository contains benchmarking implementations for evaluating FHE (Fully Homomorphic Encryption) performance across two main approaches:

1. **TFHE (Bit-wise FHE)** - Rust implementations using TFHE-rs
2. **Scheme Switching (Word-wise FHE)** - C++ implementations using OpenFHE

The benchmarks evaluate both basic workloads and real-world privacy-sensitive AI applications.

## Build Commands

### TFHE Part (Rust)

All TFHE benchmarks are independent Rust projects in `rust/tfhe-example/`. Each can be built and run separately:

```bash
# Navigate to specific benchmark
cd rust/tfhe-example/<benchmark-name>

# Build and run (release mode recommended for accurate benchmarking)
cargo run --release

# Available benchmarks:
# - workloads/      : Basic operations (mul_cp, cp_mul, mul_cp_mul)
# - sorting/        : Sorting algorithm analysis
# - floyd/          : Floyd-Warshall shortest path
# - decision_tree/  : Decision tree evaluation
# - private_db/     : Private database aggregation
```

**Important**: Always use `--release` flag for performance benchmarks. Debug builds are orders of magnitude slower.

### Scheme Switching Part (C++)

Requires OpenFHE and Google Benchmark libraries installed:

```bash
cd scheme_switching

# Build
mkdir -p build && cd build
cmake ..
make

# Run benchmarks (executables in build/bin/)
./bin/workload
./bin/decision_tree
./bin/sorting
./bin/floyd_warshall
./bin/database_aggregation
```

**Prerequisites**: OpenFHE must be installed at the paths specified in `CMakeLists.txt` (lines 8-12). Update these paths if OpenFHE is installed elsewhere.

## Architecture

### TFHE Implementation Structure

Each TFHE benchmark follows a consistent pattern:

- `main.rs` - Entry point that runs benchmarks across different bit widths (typically 6, 8, 12, 16 bits)
- `lib.rs` - Core algorithm implementation
- `complexity_analysis.rs` - Theoretical complexity analysis
- `real_benchmark.rs` - Actual runtime measurements

**Key Pattern**: TFHE encrypts each bit separately. For an 8-bit integer, you need 8 separate ciphertexts. This makes linear operations (multiplication, addition) expensive but enables efficient non-linear operations (comparison, logic gates).

### Scheme Switching Implementation Structure

Scheme switching implementations in `scheme_switching/src/`:

- `workload.cpp` - Basic workloads combining CKKS (linear ops) and FHEW (non-linear ops)
- `utils.cpp/utils.h` - Shared benchmarking utilities
- Application-specific files (`decision_tree.cpp`, `sorting.cpp`, etc.)

**Key Pattern**: Scheme switching alternates between:
- CKKS ciphertexts (efficient for multiplication/addition with SIMD support)
- FHEW ciphertexts (efficient for comparisons and boolean operations)

The switching overhead grows exponentially with bit width, making it practical only for smaller bit sizes (< 12 bits typically).

### Workload Definitions

The paper evaluates three fundamental workloads that combine linear and non-linear operations:

1. **Workload-1**: `(a*b) compare c` - Linear followed by non-linear (database queries)
2. **Workload-2**: `(a compare b) * c` - Non-linear followed by linear (decision trees)
3. **Workload-3**: `(a*b) compare (c*d)` - Mixed sequence (neural networks)

These workloads form the building blocks for the real-world applications.

## Important Implementation Details

### TFHE-rs Specifics

- Uses `tfhe` crate v1.0.1 with `boolean`, `shortint`, and `integer` features
- Provides FheUint6, FheUint8, FheUint12, FheUint16 types for different bit widths
- Gate bootstrapping is fast (~10-20ms) but happens per-gate, leading to high total cost for complex circuits
- No SIMD support - each integer requires separate ciphertexts for each bit

### OpenFHE Scheme Switching

- CKKS supports SIMD batching (multiple plaintexts in one ciphertext)
- Scheme switching between CKKS and FHEW is expensive, especially for larger bit widths
- Parameters must be carefully chosen to maintain 128-bit security (see `CMakeLists.txt` configurations)
- The switching cost dominates for frequent alternation between linear and non-linear ops

### Performance Characteristics

Based on the paper's findings:

- **TFHE**: Best for workloads dominated by non-linear operations or when SIMD cannot be utilized (e.g., single-array sorting)
- **Scheme Switching**: Best when SIMD can be leveraged and switching frequency is low (e.g., batch database queries)
- **Encoding Switching** (not in this artifact): Most efficient for general computation when both SIMD and exact computation are required

Bit width significantly impacts performance:
- 6-bit: Generally practical for all methods
- 8-bit: Good performance for TFHE and encoding switching
- 12-bit: Scheme switching becomes impractical
- 16-bit: Only TFHE and encoding switching remain viable

## Testing and Benchmarking

### Running Complete Benchmark Suite

```bash
# TFHE benchmarks (run each separately)
cd rust/tfhe-example/workloads && cargo run --release
cd ../sorting && cargo run --release
cd ../floyd && cargo run --release
cd ../decision_tree && cargo run --release
cd ../private_db && cargo run --release

# Scheme switching benchmarks
cd scheme_switching/build/bin
./workload
./decision_tree
./sorting
./floyd_warshall
./database_aggregation
```

### Expected Runtime

- Workload benchmarks: Minutes to hours depending on bit width
- Floyd-Warshall (64 nodes, 6-bit): ~18 hours with TFHE
- Decision tree (depth 6): Minutes
- Sorting (8 elements): Seconds to minutes

**Note**: First runs may be slower due to key generation and setup. The paper reports only evaluation time excluding setup.

## Key Files to Understand

### For Understanding TFHE Approach:
- `rust/tfhe-example/workloads/src/benchmarks/workload1.rs` - Basic mul_cp pattern
- `rust/tfhe-example/floyd/src/lib.rs` - Shows O(n³) complexity without SIMD
- `rust/tfhe-example/decision_tree/src/lib.rs` - Path traversal with comparisons

### For Understanding Scheme Switching:
- `scheme_switching/src/workload.cpp` - CKKS↔FHEW switching patterns
- `scheme_switching/src/utils.cpp` - Benchmarking infrastructure
- `scheme_switching/README.md` - Detailed parameter explanations

## Common Issues

1. **Out of Memory**: Large benchmarks (128-node graphs, deep decision trees) require significant RAM (16GB+)
2. **Long Runtimes**: Some benchmarks take hours. The paper's results provide expected completion times.
3. **OpenFHE Path Issues**: Update `CMakeLists.txt` lines 8-12 with correct OpenFHE installation paths
4. **Rust Version**: Requires Rust 1.70+ for TFHE-rs compatibility

## Paper Context

This artifact supports the systematic evaluation in the paper of three computationally-general FHE methods:

1. **Bit-wise TFHE** - Implements everything as boolean circuits
2. **Scheme Switching** - Switches between word-wise (CKKS/BFV) and bit-wise (FHEW/TFHE)
3. **Encoding Switching** (not in artifact) - Switches between encodings within word-wise FHE

The benchmarks demonstrate that no single method is universally optimal - the choice depends on:
- Operation mix (linear vs non-linear ratio)
- SIMD opportunities (batch processing)
- Bit width requirements
- Exact vs approximate computation needs
