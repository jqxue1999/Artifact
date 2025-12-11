# Docker Setup Guide for FHE Artifact

This guide explains how to use Docker to run the FHE artifact benchmarks in an isolated, reproducible environment.

## Why Use Docker?

✅ **Isolated environment**: No conflicts with existing system packages
✅ **One-command setup**: No manual dependency installation
✅ **Reproducible**: Guaranteed same environment across different machines
✅ **Easy cleanup**: Remove container when done, no system changes

## Prerequisites

- **Docker**: Version 20.10+ ([Install Docker](https://docs.docker.com/get-docker/))
- **Docker Compose**: Version 1.29+ (usually included with Docker Desktop)
- **System Resources**:
  - Minimum: 16GB RAM, 50GB disk space
  - Recommended: 32GB RAM, 100GB disk space
- **Time**: Initial build takes 45-90 minutes

## Quick Start

### Recommended: Automated Evaluation Scripts

The easiest way to evaluate the artifact is using the provided scripts:

```bash
# Quick sanity check (30-50 minutes total)
./test_docker_quick.sh

# Full automated evaluation with quick tests (40-60 minutes total)
./run_docker_evaluation.sh quick

# Full automated evaluation with all benchmarks (1-2 hours build + several hours benchmarks)
./run_docker_evaluation.sh full
```

These scripts automatically build the Docker image, start the container, run tests, and save results.

### Manual Docker Compose Usage

```bash
# 1. Build the Docker image
docker-compose build

# 2. Start the container
docker-compose up -d

# 3. Enter the container
docker-compose exec fhe-artifact bash

# Now you're inside the container!
# Run verification
./verify_environment.sh

# Run quick tests
./run_quick_tests.sh

# Exit container
exit

# 4. Stop the container
docker-compose down
```

### Option 2: Using Docker Commands Directly

```bash
# 1. Build the Docker image
docker build -t fhe-artifact:latest .

# 2. Run the container
docker run -it --name fhe-artifact \
  -v $(pwd)/docker_results:/artifact/results \
  fhe-artifact:latest

# You're now inside the container
./verify_environment.sh
./run_quick_tests.sh

# Exit container
exit

# 3. Restart and reconnect
docker start fhe-artifact
docker exec -it fhe-artifact bash

# 4. Clean up when done
docker stop fhe-artifact
docker rm fhe-artifact
```

## Docker Image Details

### What's Included

The Docker image contains:
- ✅ Ubuntu 22.04 base system
- ✅ All system dependencies (GCC, CMake, Git, etc.)
- ✅ Rust toolchain (latest stable)
- ✅ OpenFHE library (pre-built)
- ✅ HElib library (pre-built)
- ✅ HE-Bridge framework
- ✅ All artifact source code
- ✅ All benchmarks pre-compiled

### Image Size

- **Final image size**: ~8-10GB
  - Base Ubuntu: ~200MB
  - Build tools: ~500MB
  - OpenFHE: ~2GB
  - HElib: ~1.5GB
  - Rust + compiled artifacts: ~4GB

### Build Time

- **First build**: 45-90 minutes (compiles OpenFHE, HElib, and all benchmarks)
- **Rebuilds**: 5-10 minutes (uses Docker cache for unchanged layers)

## Running Benchmarks in Docker

### Quick Tests (5-10 minutes)

```bash
# Enter container
docker-compose exec fhe-artifact bash

# Run quick tests
./run_quick_tests.sh

# Results saved to /artifact/results/
```

### Full Benchmarks (24-72 hours)

```bash
# Enter container
docker-compose exec fhe-artifact bash

# Run full benchmarks (WARNING: takes days!)
./run_full_benchmarks.sh

# Or run in background with nohup
nohup ./run_full_benchmarks.sh > full_benchmark.log 2>&1 &

# Check progress
tail -f full_benchmark.log

# Exit container (benchmarks continue running)
exit
```

### Accessing Results

Results are stored in mounted volumes and persist after container stops:

```bash
# On host machine
ls docker_results/      # Benchmark results
ls docker_logs/         # Log files
```

## Individual Benchmark Examples

### TFHE Workloads

```bash
# Inside container
cd rust/tfhe-example/workloads
cargo run --release

# Output shows workload results for different bit widths
```

### Scheme Switching

```bash
# Inside container
cd scheme_switching/build/bin
./workload

# Shows CKKS↔FHEW switching times
```

### Encoding Switching

```bash
# Inside container
cd encoding_switching/build/bin
./workload

# Shows encoding switching results for multiple bit widths
```

## Advanced Usage

### Rebuilding After Code Changes

If you modify the source code on your host machine:

```bash
# Rebuild the image
docker-compose build --no-cache

# Or rebuild specific services
docker-compose up -d --build
```

### Running with Custom Resources

Adjust resource limits in `docker-compose.yml`:

```yaml
deploy:
  resources:
    limits:
      cpus: '16'      # Use more CPUs
      memory: 64G     # Allocate more RAM
```

Then restart:

```bash
docker-compose down
docker-compose up -d
```

### Interactive Development

Mount source code as volume for live editing:

```bash
docker run -it \
  -v $(pwd)/rust:/artifact/rust \
  -v $(pwd)/scheme_switching:/artifact/scheme_switching \
  -v $(pwd)/encoding_switching:/artifact/encoding_switching \
  fhe-artifact:latest bash

# Changes on host are immediately visible in container
# Rebuild as needed inside container
```

### Extracting Results from Container

If you didn't use volume mounts:

```bash
# Copy results out of container
docker cp fhe-artifact:/artifact/results ./extracted_results

# Or copy specific files
docker cp fhe-artifact:/artifact/full_benchmark_results ./results
```

## Troubleshooting

### Build Fails During OpenFHE/HElib Compilation

**Problem**: Out of memory or timeout during build

**Solutions**:
1. Increase Docker memory limit (Docker Desktop → Settings → Resources)
2. Reduce parallel jobs in Dockerfile:
   ```dockerfile
   # Change make -j$(nproc) to use fewer cores
   make -j2
   ```
3. Build on a more powerful machine

### Container Runs Out of Memory

**Problem**: Benchmarks fail with memory errors

**Solutions**:
1. Increase Docker memory limit to 32GB+
2. Run smaller benchmarks (skip Floyd-Warshall 128-node tests)
3. Close other applications to free RAM

### Results Not Persisting

**Problem**: Results disappear after stopping container

**Solutions**:
1. Use volume mounts (docker-compose does this automatically)
2. Copy results out before stopping:
   ```bash
   docker cp fhe-artifact:/artifact/results ./results
   ```

### Build Takes Too Long

**Problem**: Initial build exceeds 2 hours

**Solutions**:
1. This is normal on slower machines
2. Docker caches layers - subsequent builds are much faster
3. Pull pre-built image if available (to be provided)

### Permission Issues with Mounted Volumes

**Problem**: Cannot write to docker_results/

**Solutions**:
```bash
# On host, ensure directories exist and have correct permissions
mkdir -p docker_results docker_logs
chmod 777 docker_results docker_logs
```

## Docker vs Native Installation

| Aspect | Docker | Native |
|--------|--------|--------|
| Setup time | 1-2 hours (first build) | 1-2 hours |
| Isolation | Complete | None |
| Performance | ~5-10% overhead | Full speed |
| Cleanup | Easy (delete container) | Manual uninstall |
| Debugging | Slightly harder | Easier |

## Pre-built Docker Image (Optional)

For faster setup, we can provide a pre-built image:

```bash
# Pull pre-built image (if available)
docker pull ghcr.io/jqxue1999/fhe-artifact:latest

# Run directly
docker run -it ghcr.io/jqxue1999/fhe-artifact:latest
```

**Note**: Pre-built image availability depends on artifact hosting policy.

## Cleaning Up

### Remove Container and Image

```bash
# Stop and remove container
docker-compose down

# Remove volumes (deletes results!)
docker-compose down -v

# Remove image to free disk space
docker rmi fhe-artifact:latest

# Remove all unused Docker data
docker system prune -a
```

### Disk Space Recovery

The Docker build creates significant disk usage:

```bash
# Check Docker disk usage
docker system df

# Clean up build cache
docker builder prune

# Remove dangling images
docker image prune
```

## For Reviewers

We recommend reviewers use Docker for initial verification:

1. **Quick verification** (~50 minutes total):
   ```bash
   ./test_docker_quick.sh
   # This builds the image and runs basic tests for all three implementations
   ```

2. **Comprehensive evaluation with quick tests** (~1 hour total):
   ```bash
   ./run_docker_evaluation.sh quick
   # Results will be saved in results_YYYYMMDD_HHMMSS/ directory
   ```

3. **Review results**:
   ```bash
   ls results_*/                    # List result directories
   cat results_*/SUMMARY.md         # Read evaluation summary
   cat results_*/tfhe_workloads.txt # View specific results
   ```

4. **Full evaluation** (optional, several hours):
   ```bash
   ./run_docker_evaluation.sh full
   # WARNING: This runs all benchmarks and can take many hours
   ```

### Evaluation Scripts

- `test_docker_quick.sh` - Fast sanity check (builds + basic tests)
- `run_docker_evaluation.sh quick` - Automated quick evaluation
- `run_docker_evaluation.sh full` - Automated full benchmark suite

## Support

For Docker-specific issues:
- Check Docker logs: `docker-compose logs fhe-artifact`
- Verify Docker resources: `docker stats fhe-artifact`
- See main README.md for benchmark issues

## References

- [Docker Documentation](https://docs.docker.com/)
- [Docker Compose Documentation](https://docs.docker.com/compose/)
- [Main Artifact README](./README.md)
- [Artifact Appendix](./ARTIFACT-APPENDIX.md)
