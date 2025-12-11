# FHE Artifact Evaluation Guide

This guide provides step-by-step instructions for evaluating the FHE artifact using Docker.

## Overview

This artifact contains three FHE (Fully Homomorphic Encryption) implementations:

1. **TFHE (Bit-wise FHE)** - Rust implementation using TFHE-rs
2. **Scheme Switching** - C++ implementation using OpenFHE (CKKS ↔ FHEW)
3. **Encoding Switching** - C++ implementation using HElib (FV ↔ beFV)

All implementations are pre-configured to build and run in a Docker container for maximum reproducibility.

## Prerequisites

- Docker 20.10+ and Docker Compose 1.29+
- 16GB+ RAM (32GB recommended)
- 50GB+ free disk space
- 2-4 hours for initial setup and quick tests

## Quick Evaluation (Recommended)

For artifact reviewers who want to verify the implementation works correctly:

### Step 1: Quick Sanity Check (50 minutes)

This builds the Docker image and runs basic tests:

```bash
./test_docker_quick.sh
```

Expected output:
- Docker image builds successfully (~30-40 minutes)
- TFHE workloads run and produce timing results
- Scheme switching produces correct encrypted comparisons
- Encoding switching executes with custom HElib extensions

### Step 2: Automated Quick Evaluation (1 hour)

This runs a more comprehensive set of quick benchmarks and saves results:

```bash
./run_docker_evaluation.sh quick
```

Results will be saved to `results_YYYYMMDD_HHMMSS/` directory containing:
- TFHE workload benchmarks
- Scheme switching basic tests
- Encoding switching workload tests
- Summary report with interpretation guide

### Step 3: Review Results

```bash
# List available results
ls results_*/

# Read summary
cat results_*/SUMMARY.md

# View specific results
cat results_*/tfhe_workloads.txt
cat results_*/scheme_switching_basic.txt
cat results_*/encoding_switching_workload.txt
```

## Full Evaluation (Optional)

For comprehensive benchmarking (takes several hours):

```bash
./run_docker_evaluation.sh full
```

This runs all 15 benchmarks across all three implementations and saves complete results.

## Manual Evaluation

If you prefer manual control:

### Build the Docker Image

```bash
docker-compose build
```

This takes 30-90 minutes and builds:
- OpenFHE library
- HElib library with custom extensions
- All TFHE Rust projects
- All C++ benchmarks

### Start the Container

```bash
docker-compose up -d
docker-compose exec fhe-artifact bash
```

You're now inside the container with all executables ready.

### Run Individual Tests

Inside the container:

```bash
# Verify environment
./verify_environment.sh

# TFHE workloads
cd /artifact/rust/tfhe-example/workloads
cargo run --release

# Scheme switching test
cd /artifact/scheme_switching/build/bin
./test_basic

# Encoding switching workload
cd /artifact/encoding_switching/build/bin
./workload
```

### Stop the Container

```bash
exit  # Exit container shell
docker-compose down
```

## What to Verify

### 1. All Three Implementations Build Successfully

The Docker build should complete without errors for:
- [ ] TFHE (5 Rust projects)
- [ ] Scheme Switching (7 executables)
- [ ] Encoding Switching (5 executables)

### 2. TFHE Produces Correct Results

TFHE workloads should show:
- [ ] Results for 6-bit, 8-bit, 12-bit, and 16-bit configurations
- [ ] Three workloads: mul_cp, cp_mul, mul_cp_mul
- [ ] Performance degrades with increasing bit width
- [ ] Workload-2 (cp_mul) is fastest (comparison then multiplication)

### 3. Scheme Switching Works Correctly

Scheme switching tests should show:
- [ ] Successful CKKS encryption/decryption
- [ ] Successful FHEW encryption/decryption
- [ ] Successful switching between schemes
- [ ] Correct comparison results (10 < 20 = 1)

### 4. Encoding Switching Uses Custom HElib Extensions

Encoding switching should:
- [ ] Execute without compilation errors
- [ ] Run benchmarks for multiple bit widths (6, 8, 12, 16)
- [ ] Show increasing execution time with bit width
- [ ] Successfully use `multiplyModByP2R()` and `divideModByP()` methods

These custom methods are defined in:
- `/artifact/Ctxt_patched.h` (header with method declarations)
- `/artifact/encoding_switching/src/Ctxt_ext.cpp` (implementation)

## Expected Performance

Based on quick tests:

| Implementation | 6-bit | 8-bit | 12-bit | 16-bit |
|----------------|-------|-------|--------|--------|
| TFHE Workload-1 | ~0.3s | ~0.4s | ~0.8s | ~1.2s |
| TFHE Workload-2 | ~0.08s | ~0.08s | ~0.1s | ~0.1s |
| Encoding Switching | ~2s | ~2s | ~20s | ~60s |

Performance will vary based on your hardware. The key patterns to verify:
- Performance degrades with increasing bit width
- TFHE Workload-2 is consistently faster (comparison-heavy)
- Encoding switching shows exponential growth beyond 12-bit

## Troubleshooting

### Docker Build Fails

**Problem**: Out of memory during build

**Solution**:
1. Increase Docker memory limit to 16GB+ (Docker Desktop → Settings → Resources)
2. Close other applications
3. Restart Docker and try again

### Container Fails to Start

**Problem**: Container exits immediately

**Solution**:
```bash
docker-compose logs fhe-artifact
# Check logs for specific error
```

### Tests Timeout

**Problem**: Encoding switching 16-bit tests timeout

**Solution**: This is expected for larger bit widths. The timeout in quick tests prevents excessive wait times. For full evaluation, increase timeouts or run without timeout limits.

### Results Not Saved

**Problem**: Cannot find results directory

**Solution**:
- When using evaluation scripts, results are saved to `results_YYYYMMDD_HHMMSS/` in the current directory
- When running manually in container, copy results out:
  ```bash
  docker cp fhe-artifact:/artifact/results ./results
  ```

## Understanding the Results

### TFHE Results

TFHE shows timing for three workloads across different bit widths:
- **Workload-1**: (a*b) compare c - Linear followed by non-linear
- **Workload-2**: (a compare b) * c - Non-linear followed by linear
- **Workload-3**: (a*b) compare (c*d) - Mixed sequence

Key insight: TFHE excels at comparison-heavy workloads (Workload-2) because comparisons in bit-wise FHE are relatively efficient.

### Scheme Switching Results

Scheme switching alternates between:
- **CKKS**: Efficient for multiplication/addition with SIMD
- **FHEW**: Efficient for comparisons and boolean operations

Key insight: Switching overhead grows with bit width, making it impractical beyond 12-bit.

### Encoding Switching Results

Encoding switching uses custom HElib methods to switch between:
- **FV encoding**: Standard word-wise encoding
- **beFV encoding**: Bit-extraction friendly encoding

Key insight: The custom methods `multiplyModByP2R()` and `divideModByP()` enable efficient encoding switching without full re-encryption.

## Paper Claims Verification

The artifact supports these paper claims:

1. **No single FHE method is universally optimal**
   - Verify: TFHE faster for Workload-2, Encoding Switching better for mixed workloads

2. **Bit width significantly impacts performance**
   - Verify: All implementations show performance degradation with bit width

3. **Scheme switching becomes impractical beyond 12-bit**
   - Verify: Encoding switching 16-bit times should be orders of magnitude higher

4. **Custom HElib extensions enable encoding switching**
   - Verify: Encoding switching builds and runs using `multiplyModByP2R()` and `divideModByP()` methods

## Cleanup

After evaluation:

```bash
# Stop and remove container
docker-compose down

# Remove Docker image to free space
docker rmi fhe-artifact:latest

# Remove all intermediate Docker artifacts
docker system prune -a
```

This frees up ~10GB of disk space.

## Support

For issues:
1. Check DOCKER.md for detailed Docker troubleshooting
2. Check README.md for general artifact information
3. See logs: `docker-compose logs fhe-artifact`

## Summary

A successful evaluation should verify:
1. ✅ Docker image builds without errors
2. ✅ All three implementations execute successfully
3. ✅ TFHE shows expected performance patterns
4. ✅ Scheme switching produces correct encrypted comparisons
5. ✅ Encoding switching uses custom HElib methods
6. ✅ Results align with paper's performance trends

The automated evaluation scripts make this process simple:
- `./test_docker_quick.sh` - Quick verification
- `./run_docker_evaluation.sh quick` - Comprehensive quick tests
- `./run_docker_evaluation.sh full` - Full benchmarks
