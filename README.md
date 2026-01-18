# SoK: Can Fully Homomorphic Encryption Support General AI Computation? A Functional and Cost Analysis - Artifact

This repository contains the artifact for the paper "SoK: Can Fully Homomorphic Encryption Support General AI Computation? A Functional and Cost Analysis". The artifact consists of three main parts: **TFHE benchmarks** implemented in Rust, **scheme switching benchmarks** implemented in C++ with OpenFHE, and **encoding switching benchmarks** implemented in C++ with HElib.

## Table of Contents

- [Quick Start](#quick-start)
  - [Option 1: Docker (Recommended)](#option-1-docker-recommended---easiest-setup)
  - [Option 2: Native Installation](#option-2-native-installation-manual-setup)
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
- [Automated Helper Scripts](#automated-helper-scripts)
- [Docker Setup](#docker-setup)
- [Troubleshooting](#troubleshooting)
- [Citation](#citation)

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

## Quick Start

We provide **two ways** to run the artifact: using **Docker** (recommended for reviewers) or **native installation**.

### Option 1: Docker (Recommended - Easiest Setup)

Docker provides an isolated, reproducible environment with all dependencies pre-installed:

```bash
# 1. Build Docker image (45-90 minutes, one-time)
docker-compose build

# 2. Start container
docker-compose up -d

# 3. Enter container and run tests
docker-compose exec fhe-artifact bash
./verify_environment.sh
./run_quick_tests.sh

# 4. View results
exit
ls docker_results/
```

See **[DOCKER.md](DOCKER.md)** for complete Docker documentation.

**Advantages:**
- ✅ No manual dependency installation
- ✅ Guaranteed reproducible environment
- ✅ Easy cleanup (just remove container)
- ✅ No conflicts with existing system packages

### Option 2: Native Installation (Manual Setup)

For users who prefer native installation or need maximum performance:

```bash
# 1. Verify your environment has all dependencies
./verify_environment.sh

# 2. Build all three FHE implementations
./build_all.sh

# 3. Run quick smoke tests (~5-10 minutes)
./run_quick_tests.sh

# 4. (Optional) Run full benchmark suite (~24-72 hours)
./run_full_benchmarks.sh
```

These scripts provide:
- **Environment verification**: Checks that all required dependencies are installed
- **Automated building**: Compiles all three FHE implementations
- **Quick testing**: Fast smoke tests to verify correctness
- **Full benchmarks**: Complete evaluation matching paper results

See [Installation](#installation) and [Automated Helper Scripts](#automated-helper-scripts) sections below for detailed documentation.

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

## Automated Helper Scripts

This artifact includes four automated scripts to simplify environment verification, building, and testing.

### 1. Environment Verification (`verify_environment.sh`)

Verifies that all required dependencies are installed.

**What it checks:**
- System tools (GCC, CMake, Git, etc.)
- Required libraries (GMP, NTL)
- Rust toolchain
- OpenFHE installation
- HElib installation
- HE-Bridge framework
- Project structure

**Usage:**
```bash
./verify_environment.sh
```

**Output:** Checklist showing ✓ or ✗ for each component

**Expected runtime:** < 1 minute

### 2. Build All (`build_all.sh`)

Builds all three FHE implementations automatically.

**What it builds:**
- All 5 TFHE Rust projects (workloads, sorting, floyd, decision_tree, private_db)
- Scheme switching C++ project
- Encoding switching C++ project

**Usage:**
```bash
./build_all.sh
```

**Output:** Build status for each component with colored indicators

**Expected runtime:** 10-20 minutes (first build includes dependency downloads)

### 3. Quick Tests (`run_quick_tests.sh`)

Runs fast smoke tests to verify all implementations work correctly.

**What it tests:**
- TFHE workloads (limited output)
- Scheme switching workload
- Encoding switching workload

**Usage:**
```bash
./run_quick_tests.sh
```

**Output:** Test results saved to `quick_test_results/` directory

**Expected runtime:** 5-10 minutes

### 4. Full Benchmarks (`run_full_benchmarks.sh`)

Runs complete benchmark suite matching all paper results.

**Warning:** This takes 24-72 hours to complete!

**What it runs:**
- All TFHE benchmarks (all bit widths, all applications)
- All scheme switching benchmarks
- All encoding switching benchmarks

**Usage:**

For quick verification (runs in foreground):
```bash
./run_full_benchmarks.sh
```

**For long-running benchmarks** (recommended - survives SSH disconnect):

Option 1: Using `nohup` (simplest):
```bash
nohup ./run_full_benchmarks.sh > benchmark.log 2>&1 &

# Check if running
./check_benchmark_status.sh

# View live progress
tail -f benchmark.log
```

Option 2: Using `tmux` (persistent session):
```bash
# Start new session
tmux new -s benchmark

# Run benchmarks
./run_full_benchmarks.sh

# Detach: Press Ctrl+B, then D
# Resume later: tmux attach -t benchmark
```

**Checking Progress:**

Since full benchmarks take 24-72 hours, use the status checker anytime:
```bash
./check_benchmark_status.sh
```

This shows:
- Whether benchmarks are still running or completed
- Progress (X/15 benchmarks done)
- Time elapsed and last completed benchmark
- Recent log files
- System resources

**Output:** Individual logs and summary in `full_benchmark_results/` directory

**Expected runtime:** 24-72 hours (hardware dependent)

### Script Features

All scripts include:
- **Color-coded output**: Green (✓) for success, Red (✗) for failure, Yellow (⚠) for warnings
- **Progress indicators**: Shows what's currently running
- **Result logging**: Saves outputs to timestamped files
- **Error handling**: Continues on non-critical failures
- **Summary reports**: Final status of all operations

## Docker Setup

For complete, isolated environment with all dependencies pre-installed, we provide Docker support.

### Quick Docker Start

```bash
# Build image (45-90 minutes, one-time)
docker-compose build

# Start container
docker-compose up -d

# Enter container
docker-compose exec fhe-artifact bash

# Inside container: run tests
./verify_environment.sh
./run_quick_tests.sh
```

### What Docker Provides

✅ **Complete environment**: Ubuntu 22.04 + all dependencies + pre-built libraries
✅ **Reproducible**: Same environment across all machines
✅ **Isolated**: No conflicts with your system
✅ **Pre-compiled**: All benchmarks ready to run

### Docker Files

- **`Dockerfile`**: Builds the complete FHE environment
- **`docker-compose.yml`**: Easy container management with resource limits
- **`DOCKER.md`**: Complete Docker documentation (100+ lines)
- **`.dockerignore`**: Optimizes build context

### Docker Image Details

| Component | Status |
|-----------|--------|
| Base System | Ubuntu 22.04 |
| System Tools | GCC, CMake, Git, etc. |
| Rust | Latest stable |
| OpenFHE | Pre-built |
| HElib | Pre-built |
| HE-Bridge | Included |
| All Benchmarks | Pre-compiled |
| **Total Size** | ~8-10GB |
| **Build Time** | 45-90 minutes |

### Accessing Results

Results persist in mounted volumes:

```bash
# On host machine (outside container)
ls docker_results/       # All test results
ls docker_logs/          # Execution logs
```

### Full Documentation

See **[DOCKER.md](DOCKER.md)** for:
- Detailed setup instructions
- Advanced usage (custom resources, development mode)
- Troubleshooting Docker-specific issues
- Running full benchmarks in Docker
- Cleanup and maintenance

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
