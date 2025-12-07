# Artifact Appendix

Paper title: **SoK: Can Fully Homomorphic Encryption Support General AI Computation? A Functional and Cost Analysis**

<!-- Requested Badge(s):
  - [x] **Available**
  - [ ] **Functional**
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

## Basic Requirements (Optional - for "Functional" badge)

While this artifact is submitted for the **"Available"** badge only, we provide the following information for researchers interested in reproducing results.

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

## Summary for "Available" Badge

This artifact is submitted for the **"Available"** badge, which requires:
- ✅ Public availability via persistent repository (GitHub: https://github.com/jqxue1999/Artifact)
- ✅ Zenodo DOI (to be assigned after paper acceptance)
- ✅ Open-source license (MIT License included)
- ✅ Clear relationship to paper's results (three FHE paradigms evaluated in paper)
- ✅ No security/privacy risks (documented above)

**For full reproduction of paper results**, see:
- `README.md` - Complete documentation with expected outputs
- `rust/tfhe-example/README.md` - Detailed TFHE (bit-wise FHE) documentation
- `scheme_switching/README.md` - Detailed scheme switching documentation
- `encoding_switching/README.md` - Detailed encoding switching documentation