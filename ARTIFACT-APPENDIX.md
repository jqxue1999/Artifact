# Artifact Appendix

Paper title: **SoK: Can Fully Homomorphic Encryption Support General AI Computation? A Functional and Cost Analysis**

<!-- Requested Badge(s):
  - [x] **Available**
  - [x] **Functional**
  - [ ] **Reproduced** -->

## Description

### Paper Information

```bibtex
@inproceedings{xue2025measuring,
  title={Measuring Computational Universality of Fully Homomorphic Encryption},
  author={Xue, Jiaqi and Xin, Xin and Zhang, Wei and Zheng, Mengxin and Song, Qianqian and Zhou, Minxuan and Dong, Yushun and Wang, Dongjie and Chen, Xun and Xie, Jiafeng and others},
  booktitle={Proceedings on Privacy Enhancing Technologies (PoPETs)},
  year={2026}
}
```

### Artifact Description

This artifact contains comprehensive benchmarking implementations for evaluating Fully Homomorphic Encryption (FHE) performance across three computational paradigms:

1. **TFHE (Bit-wise FHE)** - Rust implementations using TFHE-rs library
2. **Scheme Switching (Word-wise FHE)** - C++ implementations using OpenFHE library
3. **Encoding Switching** - C++ implementations using HElib and HE-Bridge framework

The artifact supports the paper's systematic evaluation of FHE methods for general AI computation through:
- Basic workload benchmarks (multiplication-comparison patterns)
- Sorting algorithms
- Floyd-Warshall all-pairs shortest path
- Decision tree evaluation
- Private database aggregation

Each benchmark is implemented across all three FHE paradigms with multiple bit-width configurations (6-bit, 8-bit, 12-bit, 16-bit) to demonstrate performance characteristics and trade-offs discussed in the paper.

### Security/Privacy Issues and Ethical Concerns

**No security or privacy risks.** This artifact contains:
- Benchmark implementations using standard FHE libraries (TFHE-rs, OpenFHE, HElib)
- No malicious code, exploits, or vulnerabilities
- No collection or processing of personal data
- No disabled security mechanisms

All implementations operate on synthetic encrypted data generated locally for benchmarking purposes. The artifact does not:
- Disable any system security features (ASLR, firewalls, etc.)
- Execute vulnerable or untrusted code
- Require elevated privileges beyond standard user permissions
- Connect to external networks (except for downloading dependencies during installation)

**Ethical concerns:** None. No human subjects, user studies, or real-world data collection was involved.

## Environment

### Accessibility

**Primary Repository:** This artifact is hosted on a public Git repository (GitHub/GitLab). The repository will be made permanently available with a DOI through Zenodo upon paper acceptance.

**Repository URL:** https://github.com/jqxue1999/Artifact

**Persistent DOI:** [To be assigned via Zenodo upon final submission - artifact will be archived after paper acceptance]

**License:** MIT License (see LICENSE file in repository root)

**Contents:**
- Complete source code for all three FHE implementations
- Build scripts and configuration files (CMakeLists.txt, Cargo.toml)
- Comprehensive README.md with installation and usage instructions
- Individual README files for each major component
- CLAUDE.md providing additional implementation guidance

**Repository Structure:**
```
.
├── LICENSE                      # MIT License
├── README.md                    # Main documentation
├── ARTIFACT-APPENDIX.md         # This file
├── CLAUDE.md                    # Implementation guidance
├── rust/tfhe-example/           # TFHE implementations
│   ├── workloads/               # Basic workload benchmarks
│   ├── sorting/                 # Sorting benchmarks
│   ├── floyd/                   # Floyd-Warshall benchmarks
│   ├── decision_tree/           # Decision tree benchmarks
│   └── private_db/              # Database aggregation benchmarks
├── scheme_switching/            # OpenFHE scheme switching implementations
│   ├── src/                     # Source files
│   ├── CMakeLists.txt           # Build configuration
│   └── README.md                # Detailed documentation
└── encoding_switching/          # HElib encoding switching implementations
    ├── src/                     # Source files
    ├── CMakeLists.txt           # Build configuration
    └── README.md                # Detailed documentation
```

**Access Method:**

The artifact can be cloned from the repository:
```bash
git clone https://github.com/jqxue1999/Artifact.git
```

**External Dependencies:**
All external dependencies (TFHE-rs, OpenFHE, HElib, HE-Bridge) are either:
- Automatically downloaded via package managers (TFHE-rs via Cargo)
- Cloned from their official public repositories during installation (OpenFHE, HElib, HE-Bridge)
- No proprietary or restricted-access software is required

**Long-term Availability:**
- The repository will remain publicly accessible on GitHub/GitLab
- A snapshot will be archived on Zenodo with a persistent DOI
- All dependencies are from stable, long-term maintained projects (TFHE-rs, OpenFHE, HElib)

### Set up the Environment

This section provides complete installation instructions for all three FHE implementations.

#### Step 1: Install System Dependencies

```bash
# Ubuntu/Debian (tested on Ubuntu 20.04 and 22.04)
sudo apt-get update
sudo apt-get install -y build-essential cmake git curl pkg-config \
    libssl-dev m4 libgmp-dev libntl-dev

# Packages provide:
# - build-essential: GCC/G++ compiler (version 9+)
# - cmake: Build system (version 3.16+)
# - git: Version control
# - curl: For downloading Rust installer
# - libgmp-dev, libntl-dev: Required by HElib
# - m4: Required by NTL/HElib
```

#### Step 2: Install Rust (for TFHE benchmarks)

```bash
# Install rustup and Rust toolchain
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

# Choose default installation when prompted
# Configure current shell
source $HOME/.cargo/env

# Verify installation
rustc --version  # Should be 1.70 or higher
cargo --version
```

**Note:** TFHE-rs library is automatically downloaded by Cargo - no manual installation needed.

#### Step 3: Install OpenFHE (for Scheme Switching benchmarks)

```bash
# Clone and build OpenFHE
git clone https://github.com/openfheorg/openfhe-development.git
cd openfhe-development
mkdir build && cd build

# Configure and build
cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/../../openfhelibs \
      -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
make install
cd ../..

# Verify installation
ls openfhelibs/include/openfhe  # Should show OpenFHE headers
```

#### Step 4: Install HElib (for Encoding Switching benchmarks)

```bash
# Clone and build HElib
git clone https://github.com/homenc/HElib.git
cd HElib
mkdir build && cd build

cmake -DCMAKE_INSTALL_PREFIX=$(pwd)/../../helib_install \
      -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
make install
cd ../..

# Clone HE-Bridge framework
git clone https://github.com/UCF-Lou-Lab-PET/HE-Bridge.git

# Verify installation
ls helib_install/include/helib  # Should show HElib headers
```

**Expected directory structure after installation:**
```
Artifact/
├── openfhe-development/    # OpenFHE source
├── openfhelibs/            # OpenFHE installation
├── HElib/                  # HElib source
├── helib_install/          # HElib installation
├── HE-Bridge/              # HE-Bridge framework
├── rust/tfhe-example/      # TFHE benchmarks
├── scheme_switching/       # Scheme switching benchmarks
└── encoding_switching/     # Encoding switching benchmarks
```

**Installation time estimate:** 30-60 minutes (depending on CPU cores and internet speed)

**Disk space required:** ~5-6GB (can be reduced by removing source directories after installation)

### Testing the Environment

Verify each FHE implementation is working correctly with these quick tests.

#### Test 1: TFHE Implementation

```bash
cd rust/tfhe-example/workloads
cargo build --release
cargo run --release

# Expected output: Should show workload benchmark results
# First run may take 5-10 minutes for compilation
# Execution time: ~30 seconds for quick test
```

#### Test 2: Scheme Switching Implementation

```bash
cd scheme_switching
mkdir -p build && cd build
cmake ..
make

# Run a quick test
./bin/workload

# Expected output: Tables showing workload results with CKKS/FHEW switching times
# Execution time: ~2-5 minutes
```

#### Test 3: Encoding Switching Implementation

```bash
cd encoding_switching
mkdir -p build && cd build
cmake ..
make

# Run a quick test
./bin/workload

# Expected output: Tables showing workload results for different bit widths
# Execution time: ~5-10 minutes (includes key generation)
```

**If all three tests complete successfully, the environment is correctly configured.**

## Basic Requirements

### Hardware Requirements

**Minimum requirements:**
- CPU: Multi-core processor (4+ cores recommended)
- RAM: 8GB minimum, 16GB+ strongly recommended
- Storage: 20GB free space
- OS: Linux (tested on Ubuntu 20.04 and 22.04)

**Recommended for full evaluation:**
- CPU: 8+ cores
- RAM: 32GB (some benchmarks with large parameters require 16GB+)
- Storage: 50GB for all results

**Note:** Some benchmarks (e.g., Floyd-Warshall with 64 nodes) can take many hours to complete.

### Software Requirements

| Component | Version | Purpose |
|-----------|---------|---------|
| Ubuntu Linux | 20.04+ | Operating system |
| GCC/G++ | 9+ | C++ compiler |
| CMake | 3.16+ | Build system |
| Rust | 1.70+ | TFHE implementation |
| TFHE-rs | 0.7+ | FHE library (auto-installed) |
| OpenFHE | Latest | Scheme switching library |
| HElib | Latest | Encoding switching library |
| GMP | Latest | Math library for HElib |
| NTL | Latest | Number theory library for HElib |

### Estimated Time and Storage

**Time estimates (on 8-core machine with 32GB RAM):**
- Installation: 30-60 minutes
- TFHE benchmarks: 1-10 hours (depending on bit width and workload)
- Scheme switching benchmarks: 30 minutes - 2 hours
- Encoding switching benchmarks: 1-5 hours

**Storage consumption:**
- Installation: ~6GB
- Build artifacts: ~2GB
- Result logs: ~100MB

**Total: ~10GB**

## Artifact Completeness

This section demonstrates that the artifact includes all key components described in the paper.

### Paper Components vs. Artifact Mapping

The paper (Section 6 and 7) evaluates three computationally general FHE methods:

| Paper Component | Artifact Location | Status |
|----------------|-------------------|--------|
| **Bit-wise TFHE** (Section 3.1) | `rust/tfhe-example/` | ✅ Complete |
| **Scheme Switching** (Section 3.1) | `scheme_switching/` | ✅ Complete |
| **Encoding Switching** (Section 3.1) | `encoding_switching/` | ✅ Complete |

### Workload Benchmarks (Section 6, Figure 2)

| Workload | Description | TFHE | Scheme Switching | Encoding Switching |
|----------|-------------|------|------------------|-------------------|
| **Workload-1** | (a×b) compare c | ✅ | ✅ | ✅ |
| **Workload-2** | (a compare b) × c | ✅ | ✅ | ✅ |
| **Workload-3** | (a×b) compare (c×d) | ✅ | ✅ | ✅ |

- TFHE: `rust/tfhe-example/workloads/src/benchmarks/`
- Scheme Switching: `scheme_switching/src/workload.cpp`
- Encoding Switching: `encoding_switching/src/workload.cpp`

### Application Benchmarks (Section 7)

| Application | Paper Section | TFHE | Scheme Switching | Encoding Switching |
|------------|---------------|------|------------------|-------------------|
| **Floyd-Warshall** | 7.3, Figure 4 | ✅ | ✅ | ✅ |
| **Decision Tree** | 7.4, Figure 5 | ✅ | ✅ | ✅ |
| **Sorting** | 7.5, Figure 7 | ✅ | ✅ | ✅ |
| **Database Aggregation** | 7.6, Figure 8(b) | ✅ | ✅ | ✅ |
| **Neural Network** | 7.7, Figure 8(a) | ❌ | ❌ | ❌ |

**Note on Neural Network Implementation:**

The neural network benchmarks (Section 7.7, Figure 8a) compare existing implementations from prior work (Falcon, LoLa, OPP-CNN, EVA, SHE, TFHE-DNN) rather than providing new implementations. We do not include these third-party implementations in our artifact because:

1. They are available in their original repositories with their own licenses
2. Including them would create complex dependency chains
3. The paper's contribution is the systematic comparison, not the implementations themselves

**References to original implementations:**
- Falcon: https://github.com/snwagh/falcon-public
- LoLa: https://github.com/microsoft/CryptoNets
- OPP-CNN: [Available upon request from authors]
- EVA: https://github.com/microsoft/EVA
- SHE: https://github.com/loumiaolin/SHE

All other application benchmarks are fully implemented and reproducible in this artifact.

### Bit Width Coverage

All benchmarks support multiple bit widths as described in the paper:

- **6-bit**: Baseline for fast evaluation
- **8-bit**: Standard precision for most applications
- **12-bit**: Higher precision, scheme switching becomes impractical
- **16-bit**: Maximum precision tested, only practical for TFHE and encoding switching

### Summary of Completeness

✅ **Complete**: All three FHE paradigms implemented
✅ **Complete**: All three workload patterns implemented
✅ **Complete**: 4 of 5 application benchmarks fully implemented
⚠️ **Limitation**: Neural network benchmarks reference existing implementations (reasonably justified above)
✅ **Complete**: Multiple bit widths supported (6, 8, 12, 16 bits)
✅ **Complete**: All results in paper Figures 2, 4, 5, 7, 8(b) are reproducible

## Artifact Exercisability

This section demonstrates that the artifact can be successfully built, executed, and produces meaningful results.

### Automated Build System

We provide **two approaches** for building and running the artifact:

#### Approach 1: Docker (Recommended for Reviewers)

Complete, isolated environment with all dependencies pre-installed:

```bash
docker-compose build          # Builds complete environment (45-90 min)
docker-compose up -d          # Start container
docker-compose exec fhe-artifact bash  # Enter container
```

**Docker advantages:**
- ✅ No manual dependency installation
- ✅ Guaranteed reproducible environment
- ✅ All libraries pre-built (OpenFHE, HElib)
- ✅ All benchmarks pre-compiled
- ✅ Easy cleanup (remove container)

**Docker files provided:**
- `Dockerfile` - Complete environment definition
- `docker-compose.yml` - Container orchestration with resource limits
- `.dockerignore` - Build optimization
- `DOCKER.md` - Complete Docker documentation

#### Approach 2: Native Installation

All components use standard build systems with clear instructions:

1. **TFHE (Rust)**: Uses Cargo for dependency management and building
   - Dependencies automatically downloaded
   - Single command: `cargo build --release`

2. **Scheme Switching (C++)**: Uses CMake for cross-platform building
   - Manual OpenFHE installation required (documented in README.md)
   - Standard build: `mkdir build && cd build && cmake .. && make`

3. **Encoding Switching (C++)**: Uses CMake for cross-platform building
   - Manual HElib installation required (documented in README.md)
   - Standard build: `mkdir build && cd build && cmake .. && make`

### Automated Testing Scripts

We provide automated scripts to verify the environment and run benchmarks:

- **`verify_environment.sh`**: Checks all dependencies are correctly installed
- **`build_all.sh`**: Builds all three FHE implementations
- **`run_quick_tests.sh`**: Runs fast smoke tests (~5-10 minutes)
- **`run_full_benchmarks.sh`**: Runs complete benchmark suite (hours to days)

See "Provided Scripts" section below for details.

### Dependency Management

**TFHE (Rust):**
- Dependencies managed via `Cargo.toml` in each project
- Pinned to TFHE-rs version 0.7.x
- Automatic downloading and building

**Scheme Switching (C++):**
- Requires OpenFHE (cloned from GitHub)
- Build from source ensures compatibility
- CMakeLists.txt specifies compiler requirements

**Encoding Switching (C++):**
- Requires HElib (cloned from GitHub)
- Requires HE-Bridge framework (cloned from GitHub)
- Build from source ensures compatibility
- CMakeLists.txt specifies compiler requirements

**Note on dependency versions**: We use the latest stable versions of OpenFHE and HElib at the time of paper submission (early 2025). These are actively maintained projects with stable APIs. If future versions introduce breaking changes, users can check out specific commits:
- OpenFHE: Known working commit documented in `scheme_switching/README.md`
- HElib: Known working commit documented in `encoding_switching/README.md`

### Expected Outputs

Each benchmark produces clearly formatted output with:
- Execution time measurements
- Bit width specifications
- Success/failure indicators
- Comparisons with theoretical complexity

**Example output format:**
```
=== TFHE Workload Benchmarks ===
Workload-1: (a*b) compare c
  6-bit:  290.67s  ✓
  8-bit:  323.82s  ✓

Workload-2: (a compare b) * c
  6-bit:  98.43s   ✓
  8-bit:  106.12s  ✓
```

### Provided Scripts

The artifact includes the following helper scripts in the root directory:

#### 1. Environment Verification Script

**File**: `verify_environment.sh`

Checks that all required dependencies are installed:
- System packages (GCC, CMake, Git, etc.)
- Rust toolchain
- OpenFHE installation
- HElib installation
- HE-Bridge framework

**Usage**:
```bash
./verify_environment.sh
```

**Expected output**: Checklist showing ✓ for each correctly installed component

#### 2. Build All Script

**File**: `build_all.sh`

Builds all three FHE implementations:
- Compiles all TFHE Rust projects
- Builds scheme switching C++ projects
- Builds encoding switching C++ projects

**Usage**:
```bash
./build_all.sh
```

**Execution time**: 10-20 minutes

#### 3. Quick Test Script

**File**: `run_quick_tests.sh`

Runs fast smoke tests for each implementation:
- TFHE: Workload-1 with 6-bit only
- Scheme Switching: Basic workload test
- Encoding Switching: Basic workload test

**Usage**:
```bash
./run_quick_tests.sh
```

**Execution time**: 5-10 minutes

#### 4. Full Benchmark Script

**File**: `run_full_benchmarks.sh`

Runs complete benchmark suite matching paper results.

**Warning**: This takes many hours to days to complete.

**Usage**:
```bash
./run_full_benchmarks.sh
```

**Execution time**: 24-72 hours (depending on hardware)

### Known Limitations

1. **Platform dependency**: Only tested on Ubuntu Linux 20.04/22.04
   - May work on other Linux distributions with adjustments
   - macOS/Windows not supported

2. **Resource requirements**: Some benchmarks require substantial resources
   - Floyd-Warshall with 64+ nodes: 16GB+ RAM
   - Full benchmark suite: 50+ hours of computation time

3. **Non-deterministic timing**: Execution times vary based on:
   - CPU model and core count
   - System load
   - Memory configuration
   - Results should be within ±20% of reported values

4. **External dependencies**: OpenFHE and HElib must be built from source
   - Not available via package managers
   - Build process documented but requires ~30-60 minutes

These limitations are documented in README.md and do not prevent reviewers from successfully building and running the artifact.

## Summary for "Available" and "Functional" Badges

### "Artifact Available" Badge Requirements

✅ **Public availability**: GitHub repository at https://github.com/jqxue1999/Artifact
✅ **Alternative mirror**: UCF repository at https://github.com/UCF-ML-Research/FHE-AI-Generality.git
✅ **Persistent archiving**: Zenodo DOI (to be assigned after paper acceptance)
✅ **Open-source license**: MIT License (see LICENSE file)
✅ **Relevance to paper**: Implements all three FHE paradigms evaluated in paper
✅ **No security risks**: Documented in Section "Security/Privacy Issues and Ethical Concerns"

### "Artifact Functional" Badge Requirements

✅ **Documentation**:
- Main README.md provides complete installation and usage instructions
- Component-specific READMEs for TFHE, Scheme Switching, and Encoding Switching
- ARTIFACT-APPENDIX.md (this file) provides detailed evaluation guidance
- CLAUDE.md provides additional implementation guidance

✅ **Completeness**:
- All three FHE paradigms implemented (TFHE, Scheme Switching, Encoding Switching)
- All three workload patterns implemented (Workload-1, 2, 3)
- 4 of 5 application benchmarks fully implemented
- Neural network benchmarks reference existing implementations (justified in Completeness section)
- Multiple bit widths supported (6, 8, 12, 16 bits)
- All paper results in Figures 2, 4, 5, 7, 8(b) are reproducible

✅ **Exercisability**:
- Standard build systems (Cargo for Rust, CMake for C++)
- Automated helper scripts provided (verify_environment.sh, build_all.sh, run_quick_tests.sh)
- Clear expected outputs documented
- Environment testing procedures provided
- Known limitations clearly documented

### Documentation Files

**For full reproduction of paper results**, see:
- `README.md` - Complete documentation with expected outputs
- `rust/tfhe-example/README.md` - Detailed TFHE (bit-wise FHE) documentation
- `scheme_switching/README.md` - Detailed scheme switching documentation
- `encoding_switching/README.md` - Detailed encoding switching documentation
- `CLAUDE.md` - Additional implementation guidance for developers

### Quick Start for Reviewers

#### Option 1: Using Docker (Recommended - Easiest)

```bash
# 1. Build Docker image (45-90 min, one-time)
docker-compose build

# 2. Start and enter container
docker-compose up -d
docker-compose exec fhe-artifact bash

# 3. Inside container: verify and test
./verify_environment.sh
./run_quick_tests.sh          # ~10 minutes

# 4. View results (on host machine)
exit
ls docker_results/
```

#### Option 2: Using Native Installation

```bash
# 1. Install dependencies (see Installation section)
# ... follow README.md installation steps ...

# 2. Verify environment
./verify_environment.sh

# 3. Build all components
./build_all.sh

# 4. Run quick tests (~10 mins)
./run_quick_tests.sh

# 5. Optional - Full benchmarks (hours to days)
./run_full_benchmarks.sh
```

### Additional Documentation

- **Docker Setup**: See `DOCKER.md` for complete Docker documentation
- **Native Setup**: See `README.md` for installation and usage
- **TFHE Details**: See `rust/tfhe-example/README.md`
- **Scheme Switching**: See `scheme_switching/README.md`
- **Encoding Switching**: See `encoding_switching/README.md`

For questions or issues, please open an issue on the GitHub repository.