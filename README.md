# SoK: Can Fully Homomorphic Encryption Support General AI Computation? A Functional and Cost Analysis - Artifact

This repository contains the artifact for the paper "SoK: Can Fully Homomorphic Encryption Support General AI Computation? A Functional and Cost Analysis". The artifact consists of three main parts: **TFHE benchmarks** implemented in Rust, **scheme switching benchmarks** implemented in C++ with OpenFHE, and **encoding switching benchmarks** implemented in C++ with HElib.

## Table of Contents

- [Overview](#overview)
- [Project Structure](#project-structure)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
  - [TFHE-rs Installation](#tfhe-rs-installation)
  - [OpenFHE Installation](#openfhe-installation)
  - [HElib Installation](#helib-installation)
- [TFHE Part](#tfhe-part)
  - [Running Benchmarks](#running-benchmarks)
  - [Expected Output](#expected-output)
- [Scheme Switching Part](#scheme-switching-part)
  - [Building](#building)
  - [Running](#running)
  - [Expected Output](#output-format)
- [Encoding Switching Part](#encoding-switching-part)
  - [Building](#building)
  - [Running](#running)
  - [Expected Output](#output-format)

## Overview

This artifact provides comprehensive benchmarks for evaluating the performance of fully homomorphic encryption (FHE) operations in AI computation contexts. It includes:

**TFHE Part:**
- Basic workload benchmarks (multiplication-comparison operations)
- Sorting algorithm complexity analysis
- Floyd-Warshall algorithm benchmarks
- Decision tree evaluation benchmarks
- Private database aggregation benchmarks

**Scheme Switching Part:**
- Same workloads and applications as TFHE part, but implemented using OpenFHE with scheme switching
- Provides detailed breakdown of internal operations (CKKS to FHEW switching, FHEW to CKKS switching, etc.)
- Includes basic workloads, decision tree evaluation, and other AI computation benchmarks

**Encoding Switching Part:**
- Implements same benchmarks as TFHE and scheme switching using HElib with HE-Bridge
- Uses FV encoding (p^r) for arithmetic and beFV encoding (p) for comparisons
- Polynomial interpolation enables encrypted comparisons without scheme switching overhead
- Includes basic workload benchmarks (multiplication-comparison operations)
- Sorting algorithm complexity analysis
- Floyd-Warshall algorithm benchmarks
- Decision tree evaluation benchmarks
- Private database aggregation benchmarks

## Project Structure

```
.
├── ReadME.md                    # This documentation file
├── setup.sh                    # OpenFHE installation script
├── rust/                       # TFHE-rs implementations
│   └── tfhe-example/
│       ├── workloads/          # Basic TFHE workload benchmarks
│       │   ├── Cargo.toml      # Rust project configuration
│       │   └── src/
│       │       ├── main.rs     # Main benchmark runner
│       │       └── benchmarks/ # Benchmark implementations
│       │           ├── mod.rs
│       │           ├── workload1.rs    # (a*b) compare c
│       │           ├── workload2.rs    # (a compare b) * c
│       │           └── workload3.rs    # (a*b) compare (c*d)
│       ├── sorting/            # Sorting algorithm analysis
│       │   ├── Cargo.toml
│       │   └── src/
│       │       ├── main.rs
│       │       ├── lib.rs
│       │       ├── complexity_analysis.rs
│       │       └── real_benchmark.rs
│       ├── floyd/              # Floyd-Warshall algorithm benchmarks
│       │   ├── Cargo.toml
│       │   └── src/
│       │       ├── main.rs
│       │       ├── lib.rs
│       │       ├── complexity_analysis.rs
│       │       └── real_benchmark.rs
│       ├── decision_tree/      # Decision tree evaluation benchmarks
│       │   ├── Cargo.toml
│       │   └── src/
│       │       ├── main.rs
│       │       ├── lib.rs
│       │       ├── complexity_analysis.rs
│       │       └── real_benchmark.rs
│       └── private_db/         # Private database aggregation benchmarks
│           ├── Cargo.toml
│           └── src/
│               ├── main.rs
│               ├── lib.rs
│               ├── complexity_analysis.rs
│               └── real_benchmark.rs
├── scheme_switching/           # OpenFHE scheme switching implementations
│   ├── CMakeLists.txt          # CMake build configuration
│   ├── README.md               # Detailed scheme switching documentation
│   ├── run.sh                  # Build and run script
│   ├── src/
│   │   ├── workload.cpp        # Basic workload benchmarks
│   │   ├── workload.h          # Workload function declarations
│   │   ├── sorting.cpp         # Sorting algorithm benchmarks
│   │   ├── floyd_warshall.cpp  # Floyd-Warshall algorithm benchmarks
│   │   ├── decision_tree.cpp   # Decision tree evaluation benchmarks
│   │   ├── database_aggregation.cpp # Private database aggregation benchmarks
│   │   ├── utils.cpp           # Utility functions for benchmarking
│   │   └── utils.h             # Utility function declarations
│   ├── build/                  # Build directory (created after building)
│   │   ├── Makefile
│   │   └── bin/
│   │       ├── scheme-switching-benchmark
│   │       └── decision_tree
│   ├── benchmarklibs/          # Google Benchmark library (after installation)
│   └── openfhelibs/           # OpenFHE library (after installation)
└── encoding_switching/         # HElib encoding switching implementations
    ├── CMakeLists.txt          # CMake build configuration
    ├── README.md               # Detailed encoding switching documentation
    ├── src/
    │   ├── workload.cpp        # Basic workload benchmarks (3 patterns × 4 bit widths)
    │   ├── sorting.cpp         # Sorting algorithm benchmarks
    │   ├── floyd_warshall.cpp  # Floyd-Warshall algorithm benchmarks
    │   ├── decision_tree.cpp   # Decision tree evaluation benchmarks
    │   ├── database_aggregation.cpp # Private database aggregation benchmarks
    │   ├── bridge.cpp          # HE-Bridge encoding switching implementation
    │   ├── bridge.h            # Bridge header
    │   ├── tools.cpp           # Utility functions
    │   ├── tools.h             # Utility headers
    │   └── Ctxt_ext.cpp        # HElib extensions for encoding switching
    └── build/                  # Build directory (created after building)
        └── bin/
            ├── workload
            ├── sorting
            ├── floyd_warshall
            ├── decision_tree
            └── database_aggregation
```

## Prerequisites

- **System Requirements**: Linux (tested on Ubuntu 20.04+)
- **Rust**: Version 1.70+ with Cargo
- **C++ Compiler**: GCC 9+ or Clang 10+ with C++17 support
- **CMake**: Version 3.16.3 or higher
- **Git**: For cloning dependencies
- **Memory**: At least 8GB RAM (16GB+ recommended for larger benchmarks)

## Installation

This section provides step-by-step instructions for setting up all three FHE environments: TFHE (Rust), Scheme Switching (OpenFHE), and Encoding Switching (HElib).

### Step 1: Install System Dependencies

First, ensure you have the required system packages:

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y build-essential cmake git curl pkg-config \
    libssl-dev m4 libgmp-dev libntl-dev

# The packages provide:
# - build-essential: GCC/G++ compiler
# - cmake: Build system (version 3.16+)
# - git: Version control
# - curl: For downloading Rust
# - libgmp-dev, libntl-dev: Required by HElib
# - m4: Required by NTL/HElib
```

### Step 2: Install Rust (for TFHE benchmarks)

Install Rust using rustup (official installer):

```bash
# Install rustup and Rust toolchain
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

# Follow the prompts (choose default installation)
# After installation, configure your current shell:
source $HOME/.cargo/env

# Verify installation
rustc --version  # Should be 1.70 or higher
cargo --version
```

**TFHE-rs library** is automatically downloaded and compiled by Cargo when you build the TFHE benchmarks (no manual installation needed).

### Step 3: Install OpenFHE (for Scheme Switching benchmarks)

Navigate to your artifact directory and install OpenFHE:

```bash
# Clone OpenFHE repository
git clone https://github.com/openfheorg/openfhe-development.git
cd openfhe-development

# Create build directory
mkdir build && cd build

# Configure with CMake (replace /path/to/Artifact with actual path)
cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/../../openfhelibs \
      -DCMAKE_BUILD_TYPE=Release \
      ..

# Build and install (use all CPU cores)
make -j$(nproc)
make install

# Return to artifact root
cd ../..
```

**Verify OpenFHE installation:**
```bash
ls openfhelibs/include/openfhe  # Should show OpenFHE headers
ls openfhelibs/lib              # Should show libOPENFHE*.so files
```

### Step 4: Install HElib (for Encoding Switching benchmarks)

Install HElib and the HE-Bridge framework:

```bash
# Clone HElib repository
git clone https://github.com/homenc/HElib.git
cd HElib

# Create build directory
mkdir build && cd build

# Configure with CMake (installs to ../helib_install)
cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/../../helib_install \
      -DCMAKE_BUILD_TYPE=Release \
      ..

# Build and install
make -j$(nproc)
make install

# Return to artifact root
cd ../..
```

**Clone HE-Bridge framework:**
```bash
# HE-Bridge provides encoding switching utilities
git clone https://github.com/UCF-Lou-Lab-PET/HE-Bridge.git
```

**Verify HElib installation:**
```bash
ls helib_install/include/helib  # Should show HElib headers
ls helib_install/lib            # Should show libhelib.a
```

### Installation Summary

After completing all steps, your directory structure should look like:

```
Artifact/
├── openfhe-development/        # OpenFHE source (can be removed after install)
├── openfhelibs/                # OpenFHE installation
├── HElib/                      # HElib source (can be removed after install)
├── helib_install/              # HElib installation
├── HE-Bridge/                  # HE-Bridge framework source
├── rust/tfhe-example/          # TFHE benchmarks (Rust)
├── scheme_switching/           # Scheme switching benchmarks (OpenFHE)
└── encoding_switching/         # Encoding switching benchmarks (HElib)
```

**Disk space requirements:**
- OpenFHE source + build: ~2GB
- HElib source + build: ~1.5GB
- Rust toolchain: ~1GB
- Total: ~5-6GB (can reduce by removing source directories after installation)

## TFHE Part

### Project Structure

```
rust/tfhe-example/
├── workloads/          # Basic TFHE workload benchmarks
├── sorting/            # Sorting algorithm analysis
├── floyd/              # Floyd-Warshall algorithm benchmarks
├── decision_tree/      # Decision tree evaluation benchmarks
└── private_db/         # Private database aggregation benchmarks
```

### Running Benchmarks

#### 1. Workloads (Basic Operations)

Tests fundamental TFHE operations with different bit widths:

```bash
cd rust/tfhe-example/workloads
cargo run --release
```

**Operations tested:**
- Workload-1: `(a*b) compare c` (mul_cp operation)
- Workload-2: `(a compare b) * c` (cp_mul operation)  
- Workload-3: `(a*b) compare (c*d)` (mul_cp_mul operation)

#### 2. Sorting Algorithm Analysis

Analyzes encrypted sorting complexity using actual TFHE operations:

```bash
cd rust/tfhe-example/sorting
cargo run --release
```

#### 3. Floyd-Warshall Algorithm

Benchmarks shortest path algorithm for graphs with different node counts:

```bash
cd rust/tfhe-example/floyd
cargo run --release
```

**Test cases:** 16, 32, 64, 128 nodes

#### 4. Decision Tree Evaluation

Benchmarks decision tree traversal for complete binary trees:

```bash
cd rust/tfhe-example/decision_tree
cargo run --release
```

**Test cases:** Depth 2, 4, 6, 8 trees

#### 5. Private Database Aggregation

Benchmarks encrypted database query operations:

```bash
cd rust/tfhe-example/private_db
cargo run --release
```

**Test cases:** 64, 128, 256, 512 database rows

### Expected Output

#### Workloads Output:
```
Workload-1: (a*b) compare c
6-bit: 290.67s
8-bit: 323.82s
16-bit: 696.74s

Workload-2: (a compare b) * c
6-bit: 98.43s
8-bit: 106.12s
16-bit: 117.24s

Workload-3: (a*b) compare (c*d)
6-bit: 446.9s
8-bit: 540.89s
16-bit: 1220s
```

#### Sorting Analysis Output:
```
  Size        6-bit        8-bit       16-bit
  --------------------------------------------
     8        16.8s        17.0s        33.0s
    16         1.1m         1.2m         2.0m
    32         4.4m         4.5m         8.8m
    64        17.6m        18.1m        35.2m
```

#### Floyd-Warshall Output:
```
Running 16 nodes, 6-bit...
Floyd-Warshall: 16 nodes, 6-bit: 17.0m
Running 32 nodes, 6-bit...  
Floyd-Warshall: 32 nodes, 6-bit: 2.3h
Running 64 nodes, 6-bit...
Floyd-Warshall: 64 nodes, 6-bit: 18.1h
```

#### Decision Tree Output:
```
Running depth 2, 6-bit...
Decision Tree: depth 2, 6-bit: 2.5s
Running depth 4, 6-bit...
Decision Tree: depth 4, 6-bit: 18.3s
Running depth 6, 6-bit...
Decision Tree: depth 6, 6-bit: 2.5m
```

#### Private Database Aggregation Output:
```
Running 64 rows, 8-bit...
Database Aggregation: 64 rows, 8-bit: 45.2s
Running 128 rows, 8-bit...
Database Aggregation: 128 rows, 8-bit: 1.5m
Running 256 rows, 8-bit...
Database Aggregation: 256 rows, 8-bit: 3.0m
```

## Scheme Switching Part

The scheme switching part implements the same benchmarks as the TFHE part but using OpenFHE with scheme switching between CKKS and FHEW schemes. This provides a comparison between pure TFHE operations and hybrid scheme approaches. The output includes detailed breakdowns of the internal scheme switching operations.

### Building

1. **Navigate to scheme switching directory:**
   ```bash
   cd scheme_switching
   ```

2. **Build the project:**
   ```bash
   mkdir -p build
   cd build
   cmake ..
   make
   ```

   **Alternative using the provided script:**
   ```bash
   ./run.sh
   ```

### Running

Execute the scheme switching benchmarks:

```bash
# From build directory
./bin/scheme-switching-benchmark

# Or run decision tree specifically
./bin/decision_tree
```

### Output Format

#### Workloads with Scheme Switching (8-bit precision):
```
Workload-1: (a*b) compare c
Total time: 32.1 s

Workload-2: (a compare b) * c
Total time: ~25 s

Workload-3: (a*b) compare (c*d)
Total time: ~40 s

Note: Scheme switching becomes impractical for 12-bit and 16-bit inputs
due to exponential growth in switching complexity.
```

#### Applications with Scheme Switching:
```
=== Decision Tree Evaluation ===
Parameters: depth=4, nodes=15
Setup time: 0.167 s
Key generation time: 2.345 s
Precomputation time: 0.067 s
Evaluation time: 1.234 s
Total time: 3.813 s
=====================================
```

## Encoding Switching Part

The encoding switching part implements the same benchmarks using HElib with the HE-Bridge framework. Unlike scheme switching which alternates between different FHE schemes (CKKS ↔ FHEW), encoding switching works within the FV scheme by switching between different plaintext encodings (FV ↔ beFV) for efficient encrypted comparisons.

**Key Features:**
- Uses FV encoding (plaintext modulus p^r) for arithmetic operations
- Uses beFV encoding (plaintext modulus p) for comparison operations via polynomial interpolation
- No expensive scheme switching overhead - only encoding switching within FV
- Supports 6, 8, 12, and 16-bit integer operations with optimized parameters

### Building

**Prerequisites:** Ensure HElib and HE-Bridge are installed (see Step 4 in Installation section above).

Build all encoding switching benchmarks:

```bash
cd encoding_switching
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

This will create five executables in `build/bin/`:
- `workload` - Tests all 3 workload patterns with 4 bit widths
- `decision_tree` - Decision tree evaluation
- `sorting` - Sorting algorithm
- `floyd_warshall` - All-pairs shortest path
- `database_aggregation` - Private database queries

### Running

Execute the encoding switching benchmarks from the build directory:

```bash
cd encoding_switching/build/bin

# Run workload benchmarks (tests all 3 patterns with 6, 8, 12, 16-bit)
./workload

# Run other benchmarks
./decision_tree
./sorting
./floyd_warshall
./database_aggregation
```

### Output Format

#### Workloads with Encoding Switching:
```
================================================================================
HE-Bridge Encoding Switching Workload Benchmarks
================================================================================

Workload-1: (a*b) compare c
--------------------------------------------------------------------------------
Bit Width      Parameters (p, r)        Time                Status
--------------------------------------------------------------------------------
6-bit          p=3, r=4                 ~8 s                ✓
8-bit          p=17, r=2                15.5 s              ✓
12-bit         p=67, r=2                23.0 s              ✓
16-bit         p=257, r=2               46.9 s              ✓
```

#### Decision Tree with Encoding Switching:
```
================================================================================
HE-Bridge Encoding Switching Decision Tree Evaluation
================================================================================

Parameters: m=13201, p=17, r=2, bits=256

Decision Tree Benchmark
--------------------------------------------------------------------------------
Tree Depth     Bit Width      Time                Status
--------------------------------------------------------------------------------
2              9              15 s                ✓
4              9              1.2 m               ✓
6              9              8.5 m               ✓
```

## Troubleshooting

### Common Issues:

1. **TFHE compilation errors**: Ensure Rust 1.70+ is installed
2. **OpenFHE not found**: Verify OpenFHE installation path in `scheme_switching/CMakeLists.txt`
3. **HElib not found**: Verify HElib installation path in `encoding_switching/CMakeLists.txt` (should point to `helib_install/`)
4. **Memory errors**: Large configurations may exceed available RAM (16GB+ recommended)
5. **Long execution times**: This is expected - some benchmarks take hours to complete, especially for larger parameters

### Debug Mode:
```bash
# For TFHE projects
cargo run  # (without --release)

# For OpenFHE projects  
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

## Citation

If you use this artifact in your research, please cite:

```bibtex
@inproceedings{xue2025measuring,
  title={Measuring Computational Universality of Fully Homomorphic Encryption},
  author={Xue, Jiaqi and Xin, Xin and Zhang, Wei and Zheng, Mengxin and Song, Qianqian and Zhou, Minxuan and Dong, Yushun and Wang, Dongjie and Chen, Xun and Xie, Jiafeng and others},
  booktitle={Proceedings on Privacy Enhancing Technologies (PoPETs)},
  year={2026}
}
```
