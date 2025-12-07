# TFHE Benchmarks - Bit-wise FHE Implementation

This directory contains benchmarking implementations for evaluating bit-wise Fully Homomorphic Encryption (FHE) using the TFHE-rs library. TFHE (Fast Fully Homomorphic Encryption over the Torus) encrypts individual bits into separate ciphertexts, enabling efficient non-linear operations through fast gate bootstrapping.

## Overview

TFHE is a bit-wise FHE scheme that excels at non-linear operations such as comparisons and Boolean logic. Unlike word-wise FHE schemes (BGV, BFV, CKKS), TFHE:

- **Encrypts bit-by-bit**: Each bit of an integer is encrypted separately
- **Fast bootstrapping**: Gate bootstrapping takes ~10-20ms per gate
- **Efficient non-linear ops**: Comparisons and Boolean gates are naturally supported
- **No SIMD support**: Cannot batch multiple values in a single ciphertext
- **Expensive linear ops**: Multiplication requires O(b²) complexity for b-bit integers

## Project Structure

```
rust/tfhe-example/
├── README.md                    # This file
├── workloads/                   # Basic workload benchmarks
│   ├── Cargo.toml
│   └── src/
│       ├── main.rs              # Entry point, runs all bit widths
│       └── benchmarks/
│           ├── mod.rs
│           ├── workload1.rs     # (a*b) compare c
│           ├── workload2.rs     # (a compare b) * c
│           └── workload3.rs     # (a*b) compare (c*d)
├── sorting/                     # Sorting algorithm benchmarks
│   ├── Cargo.toml
│   └── src/
│       ├── main.rs              # Entry point
│       ├── lib.rs               # Core sorting implementation
│       ├── complexity_analysis.rs    # Theoretical complexity
│       └── real_benchmark.rs    # Runtime measurements
├── floyd/                       # Floyd-Warshall shortest path
│   ├── Cargo.toml
│   └── src/
│       ├── main.rs
│       ├── lib.rs               # Floyd-Warshall implementation
│       ├── complexity_analysis.rs
│       └── real_benchmark.rs
├── decision_tree/               # Decision tree evaluation
│   ├── Cargo.toml
│   └── src/
│       ├── main.rs
│       ├── lib.rs               # Tree traversal implementation
│       ├── complexity_analysis.rs
│       └── real_benchmark.rs
└── private_db/                  # Private database aggregation
    ├── Cargo.toml
    └── src/
        ├── main.rs
        ├── lib.rs               # Database query implementation
        ├── complexity_analysis.rs
        └── real_benchmark.rs
```

## Prerequisites

- **Rust**: Version 1.70 or higher
- **Cargo**: Rust's package manager (installed with Rust)
- **Memory**: 8GB RAM minimum, 16GB+ recommended

### Installing Rust

```bash
# Install rustup (official Rust installer)
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

# Choose default installation
# Configure shell
source $HOME/.cargo/env

# Verify installation
rustc --version  # Should show 1.70 or higher
cargo --version
```

**Note**: TFHE-rs library is automatically downloaded and compiled by Cargo - no manual installation needed.

## Building and Running

### Important: Always Use Release Mode

**CRITICAL**: Always build and run with `--release` flag. Debug builds are orders of magnitude slower (10-100x) and unsuitable for benchmarking.

```bash
# ✓ CORRECT - Use release mode
cargo run --release

# ✗ WRONG - Debug mode is extremely slow
cargo run
```

### Benchmark 1: Basic Workloads

Tests fundamental operations combining linear and non-linear operations.

```bash
cd workloads
cargo run --release
```

**Operations tested:**
- **Workload-1**: `(a*b) compare c` - Linear followed by non-linear
- **Workload-2**: `(a compare b) * c` - Non-linear followed by linear
- **Workload-3**: `(a*b) compare (c*d)` - Mixed sequence

**Bit widths tested**: 6, 8, 12, 16 bits

**Expected output:**
```
=== TFHE Workload Benchmarks ===

Workload-1: (a*b) compare c
  6-bit:  290.67s
  8-bit:  323.82s
  12-bit: 495.23s
  16-bit: 696.74s

Workload-2: (a compare b) * c
  6-bit:  98.43s
  8-bit:  106.12s
  12-bit: 112.45s
  16-bit: 117.24s

Workload-3: (a*b) compare (c*d)
  6-bit:  446.9s
  8-bit:  540.89s
  12-bit: 890.12s
  16-bit: 1220s
```

**Execution time**: ~30-60 minutes for all workloads and bit widths

### Benchmark 2: Sorting Algorithm

Evaluates encrypted sorting using comparison-based algorithms.

```bash
cd sorting
cargo run --release
```

**Test cases**: Arrays of 8, 16, 32, 64 elements
**Bit widths**: 6, 8, 16 bits

**Expected output:**
```
=== TFHE Sorting Benchmarks ===

Size    6-bit     8-bit     16-bit
-------------------------------------
8       16.8s     17.0s     33.0s
16      1.1m      1.2m      2.0m
32      4.4m      4.5m      8.8m
64      17.6m     18.1m     35.2m
```

**Execution time**: ~1-2 hours

### Benchmark 3: Floyd-Warshall Algorithm

All-pairs shortest path on encrypted graphs.

```bash
cd floyd
cargo run --release
```

**Test cases**: Graphs with 16, 32, 64, 128 nodes
**Bit widths**: 6 bits (larger bit widths take prohibitively long)

**Expected output:**
```
=== TFHE Floyd-Warshall Benchmarks ===

Running 16 nodes, 6-bit...
Floyd-Warshall: 16 nodes, 6-bit: 17.0m

Running 32 nodes, 6-bit...
Floyd-Warshall: 32 nodes, 6-bit: 2.3h

Running 64 nodes, 6-bit...
Floyd-Warshall: 64 nodes, 6-bit: 18.1h

Warning: 128 nodes may take 100+ hours
```

**Execution time**: 1-20 hours (depending on graph size)

**Note**: Floyd-Warshall demonstrates TFHE's O(n³) complexity without SIMD optimization, as each matrix element requires separate ciphertexts.

### Benchmark 4: Decision Tree Evaluation

Private decision tree inference on encrypted data.

```bash
cd decision_tree
cargo run --release
```

**Test cases**: Complete binary trees of depth 2, 4, 6, 8
**Bit widths**: 6, 8 bits

**Expected output:**
```
=== TFHE Decision Tree Benchmarks ===

Running depth 2, 6-bit...
Decision Tree: depth 2, 6-bit: 2.5s

Running depth 4, 6-bit...
Decision Tree: depth 4, 6-bit: 18.3s

Running depth 6, 6-bit...
Decision Tree: depth 6, 6-bit: 2.5m

Running depth 8, 6-bit...
Decision Tree: depth 8, 6-bit: 18.2m
```

**Execution time**: ~30 minutes for all depths

### Benchmark 5: Private Database Aggregation

Encrypted database queries with filtering predicates.

```bash
cd private_db
cargo run --release
```

**Test cases**: Databases with 64, 128, 256, 512 rows
**Bit widths**: 8 bits

**Expected output:**
```
=== TFHE Database Aggregation Benchmarks ===

Running 64 rows, 8-bit...
Database Aggregation: 64 rows, 8-bit: 45.2s

Running 128 rows, 8-bit...
Database Aggregation: 128 rows, 8-bit: 1.5m

Running 256 rows, 8-bit...
Database Aggregation: 256 rows, 8-bit: 3.0m

Running 512 rows, 8-bit...
Database Aggregation: 512 rows, 8-bit: 6.2m
```

**Execution time**: ~15 minutes

## Understanding TFHE Parameters

### Bit Width Encoding

TFHE uses the `FheUint` types from TFHE-rs:

```rust
use tfhe::prelude::*;
use tfhe::{FheUint6, FheUint8, FheUint12, FheUint16};

// 6-bit integer (values 0-63)
let a: FheUint6 = FheUint6::encrypt(42u8, &client_key);

// 8-bit integer (values 0-255)
let b: FheUint8 = FheUint8::encrypt(100u8, &client_key);
```

Each `FheUintN` type encrypts a value as N separate bit ciphertexts internally.

### Security Parameters

All TFHE-rs types use default parameters providing:
- **Security level**: ~128 bits
- **Bootstrapping time**: ~10-20ms per gate
- **Correctness**: Probabilistic (error probability < 2⁻⁴⁰)

### Operation Complexity

| Operation | Complexity | Example Time (8-bit) |
|-----------|-----------|---------------------|
| Comparison | O(b) | ~200ms |
| Addition | O(b) | ~150ms |
| Multiplication | O(b²·p) | ~30s |
| AND/OR gate | O(1) | ~15ms |

Where:
- b = bit width
- p = number of CMUX operations in multiplication circuit

## Performance Characteristics

### When TFHE Performs Well

✅ **Non-linear dominated workloads**
- Comparisons, min/max operations
- Boolean logic circuits
- Decision trees with many branches

✅ **Small bit widths** (6-8 bits)
- Faster evaluation
- Lower memory usage

✅ **Single-value operations**
- When SIMD cannot be utilized
- Sorting single arrays

### When TFHE Performs Poorly

❌ **Linear operation dominated workloads**
- Matrix multiplications
- Polynomial evaluations

❌ **Large bit widths** (>16 bits)
- Multiplication becomes prohibitively slow
- 32-bit multiplication takes ~5 minutes

❌ **Batch operations**
- No SIMD support
- Must process each value separately

## Troubleshooting

### Issue: Very Slow Execution

**Problem**: Benchmark takes much longer than expected times

**Solution**: Ensure you're using `--release` flag:
```bash
# Check if you're in release mode
cargo run --release  # ✓ Correct

# NOT debug mode
cargo run            # ✗ Wrong - 10-100x slower
```

### Issue: Out of Memory

**Problem**: Process killed due to insufficient memory

**Solutions**:
1. Close other applications to free RAM
2. Reduce benchmark size (edit `main.rs` to skip large test cases)
3. Add more RAM (16GB+ recommended for large benchmarks)

### Issue: Compilation Errors

**Problem**: `cargo build` fails with errors

**Solutions**:
```bash
# Update Rust to latest version
rustup update

# Clean build cache and rebuild
cargo clean
cargo build --release

# Verify Rust version (should be 1.70+)
rustc --version
```

### Issue: Benchmark Hangs

**Problem**: Benchmark appears stuck without output

**Explanation**: This is normal for some operations. Large multiplications (16-bit) can take 10+ minutes for a single operation. Monitor CPU usage to confirm it's still running.

## Comparison with Word-wise FHE

| Aspect | TFHE (Bit-wise) | Scheme/Encoding Switching (Word-wise) |
|--------|-----------------|--------------------------------------|
| Non-linear ops | Fast (~200ms) | Slow or requires switching |
| Linear ops | Very slow (~30s for 8-bit mul) | Fast (<1ms) |
| SIMD support | No | Yes (1000s of slots) |
| Bootstrapping | Fast, per-gate (~15ms) | Slow, amortized (1-7ms/slot) |
| Best for | Comparisons, logic | Arithmetic, batch processing |

## Code Structure

### Main Entry Point Pattern

Each benchmark follows this pattern in `main.rs`:

```rust
fn main() {
    // Test multiple bit widths
    let bit_widths = vec![6, 8, 12, 16];

    for bits in bit_widths {
        match bits {
            6 => run_benchmark::<FheUint6>(bits),
            8 => run_benchmark::<FheUint8>(bits),
            12 => run_benchmark::<FheUint12>(bits),
            16 => run_benchmark::<FheUint16>(bits),
            _ => unreachable!(),
        }
    }
}
```

### Benchmark Implementation Pattern

Implementations in `lib.rs` follow this structure:

```rust
// 1. Generate keys (one-time setup)
let config = ConfigBuilder::default().build();
let (client_key, server_key) = generate_keys(config);
set_server_key(server_key);

// 2. Encrypt inputs
let enc_a = FheUint8::encrypt(a, &client_key);
let enc_b = FheUint8::encrypt(b, &client_key);

// 3. Evaluate on encrypted data
let start = Instant::now();
let enc_result = homomorphic_operation(&enc_a, &enc_b);
let duration = start.elapsed();

// 4. Decrypt result (for verification)
let result: u8 = enc_result.decrypt(&client_key);
```

## Further Reading

- **TFHE-rs Documentation**: https://docs.zama.ai/tfhe-rs
- **TFHE Paper**: "TFHE: Fast Fully Homomorphic Encryption over the Torus" (Chillotti et al., 2020)
- **Main Artifact README**: `../../README.md` - Overview of all three FHE implementations
- **Paper**: Section 3.1 - Analysis of bit-wise TFHE for general computation

## Citation

If you use these TFHE benchmarks in your research, please cite:

```bibtex
@inproceedings{xue2025measuring,
  title={Measuring Computational Universality of Fully Homomorphic Encryption},
  author={Xue, Jiaqi and Xin, Xin and Zhang, Wei and Zheng, Mengxin and Song, Qianqian and Zhou, Minxuan and Dong, Yushun and Wang, Dongjie and Chen, Xun and Xie, Jiafeng and others},
  booktitle={Proceedings on Privacy Enhancing Technologies (PoPETs)},
  year={2026}
}
```
