# SoK: Can Fully Homomorphic Encryption Support General AI Computation? A Functional and Cost Analysis - Artifact

This repository contains the artifact for the paper "SoK: Can Fully Homomorphic Encryption Support General AI Computation? A Functional and Cost Analysis". The artifact consists of two main parts: **TFHE benchmarks** implemented in Rust and **scheme switching benchmarks** implemented in C++ with OpenFHE.

## Table of Contents

- [Overview](#overview)
- [Project Structure](#project-structure)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
  - [TFHE-rs Installation](#tfhe-rs-installation)
  - [OpenFHE Installation](#openfhe-installation)
- [TFHE Part](#tfhe-part)
  - [Running Benchmarks](#running-benchmarks)
  - [Expected Output](#expected-output)
- [Scheme Switching Part](#scheme-switching-part)
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

**Encoding Switching Part: (Doing)**
- Basic workload benchmarks (multiplication-comparison operations)
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
└── scheme_switching/           # OpenFHE scheme switching implementations
    ├── CMakeLists.txt          # CMake build configuration
    ├── README.md               # Detailed scheme switching documentation
    ├── run.sh                  # Build and run script
    ├── src/
    │   ├── workload.cpp        # Basic workload benchmarks
    │   ├── workload.h          # Workload function declarations
    │   ├── sorting.cpp         # Sorting algorithm benchmarks
    │   ├── floyd_warshall.cpp  # Floyd-Warshall algorithm benchmarks
    │   ├── decision_tree.cpp   # Decision tree evaluation benchmarks
    │   ├── database_aggregation.cpp # Private database aggregation benchmarks
    │   ├── utils.cpp           # Utility functions for benchmarking
    │   └── utils.h             # Utility function declarations
    ├── build/                  # Build directory (created after building)
    │   ├── Makefile
    │   └── bin/
    │       ├── scheme-switching-benchmark
    │       └── decision_tree
    ├── benchmarklibs/          # Google Benchmark library (after installation)
    └── openfhelibs/           # OpenFHE library (after installation)
```

## Prerequisites

- **System Requirements**: Linux (tested on Ubuntu 20.04+)
- **Rust**: Version 1.70+ with Cargo
- **C++ Compiler**: GCC 9+ or Clang 10+ with C++17 support
- **CMake**: Version 3.16.3 or higher
- **Git**: For cloning dependencies
- **Memory**: At least 8GB RAM (16GB+ recommended for larger benchmarks)

## Installation

### TFHE-rs Installation

TFHE-rs is automatically handled by Cargo. No separate installation is required as it's specified in each project's `Cargo.toml` file.

### OpenFHE Installation

1. **Clone OpenFHE repository:**
   ```bash
   git clone https://github.com/openfheorg/openfhe-development.git
   ```

2. **Create installation directory:**
   ```bash
   mkdir -p openfhelibs
   cd openfhe-development
   ```

3. **Build OpenFHE:**
   ```bash
   mkdir build
   cd build
   cmake -DCMAKE_INSTALL_PREFIX=/path/to/your/project/openfhelibs ..
   make -j$(nproc)
   make install
   ```

4. **Install Google Benchmark (required for some benchmarks):**
   ```bash
   cd /path/to/your/project
   git clone https://github.com/google/benchmark.git
   cd benchmark
   git clone https://github.com/google/googletest.git
   mkdir -p benchmarklibs
   cmake -E make_directory build
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
     -DCMAKE_INSTALL_PREFIX=/path/to/your/project/benchmarklibs \
     -DBENCHMARK_DOWNLOAD_DEPENDENCIES=ON
   cmake --build build --config Release
   cmake --install build --config Release
   ```

**Note:** Replace `/path/to/your/project` with the actual path to this repository.

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

#### Workloads with Scheme Switching Breakdown:
```
=== Workload-1: (a*b) compare c ===
CKKS to FHEW Switching time: 2.456 s
Comparison operation time: 0.234 s  
FHEW to CKKS Switching time: 1.876 s
Total time: 4.566 s
=====================================

=== Workload-2: (a compare b) * c ===
CKKS to FHEW Switching time: 2.123 s
Multiplication operation time: 0.156 s
FHEW to CKKS Switching time: 1.654 s  
Total time: 3.933 s
=====================================
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

## Troubleshooting

### Common Issues:

1. **TFHE compilation errors**: Ensure Rust 1.70+ is installed
2. **OpenFHE not found**: Verify OpenFHE installation path in CMake configuration
3. **Memory errors**: Large configurations may exceed available RAM

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
@article{xue2025measuring,
  title={Measuring Computational Universality of Fully Homomorphic Encryption},
  author={Xue, Jiaqi and Xin, Xin and Zhang, Wei and Zheng, Mengxin and Song, Qianqian and Zhou, Minxuan and Dong, Yushun and Wang, Dongjie and Chen, Xun and Xie, Jiafeng and others},
  journal={arXiv preprint arXiv:2504.11604},
  year={2025}
}
```
