# OpenFHE Scheme Switching Benchmark Suite

This benchmark suite provides comprehensive performance measurements for scheme switching operations between CKKS and FHEW cryptographic schemes in OpenFHE.

## Overview

The benchmark suite measures the performance of five key scheme switching workloads:

1. **CKKS to FHEW Switching** - Converting packed CKKS ciphertexts to multiple FHEW ciphertexts
2. **FHEW to CKKS Switching** - Converting multiple FHEW ciphertexts back to packed CKKS ciphertexts
3. **Comparison via Scheme Switching** - Performing comparisons using CKKS→FHEW→CKKS switching
4. **Argmin via Scheme Switching** - Computing minimum value and its index using scheme switching
5. **Argmin via Scheme Switching (Alternative)** - Alternative implementation of argmin computation

## Features

- **Comprehensive Timing**: Measures setup, key generation, precomputation, evaluation, and total time
- **Multiple Configurations**: Predefined configurations for different performance levels
- **Customizable Parameters**: Command-line options for custom configurations
- **Detailed Reporting**: Individual results and summary tables
- **Error Handling**: Robust error handling and reporting

## Building

### Prerequisites

- OpenFHE library (installed and configured)
- CMake 3.16.3 or higher
- C++17 compatible compiler
- OpenMP (optional, for parallel processing)

### Build Steps

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the benchmark suite
make

# Optional: Install (if needed)
make install
```

### Build Options

- `BUILD_STATIC=ON`: Build with static linking
- `CMAKE_BUILD_TYPE=Release`: Optimized release build (recommended for benchmarking)

## Usage

### Basic Usage

```bash
# Run complete benchmark suite (default)
./scheme-switching-benchmark

# Run with custom configuration
./scheme-switching-benchmark --single --depth 24 --slots 256 --numValues 256
```

### Command Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `--suite` | Run complete benchmark suite | ✓ |
| `--single` | Run single benchmark configuration | |
| `--depth <value>` | Set multiplicative depth | 16 |
| `--slots <value>` | Set number of slots | 64 |
| `--numValues <value>` | Set number of values | 64 |
| `--ringDim <value>` | Set ring dimension | 2048 |
| `--help` | Show help message | |

### Predefined Configurations

The benchmark suite includes four predefined configurations:

1. **Small** (8, 16, 16, 1024) - Quick test configuration
2. **Medium** (16, 64, 64, 2048) - Standard benchmark configuration  
3. **Large** (24, 256, 256, 4096) - Performance test configuration
4. **Very Large** (32, 1024, 1024, 8192) - Stress test configuration

## Output Format

### Individual Results

Each workload produces detailed timing information:

```
=== CKKS to FHEW Switching ===
Parameters: depth=16, slots=64, numValues=64, ringDim=2048
Setup time: 0.125 s
Key generation time: 2.456 s
Precomputation time: 0.089 s
Evaluation time: 0.234 s
Total time: 2.904 s
=====================================
```

### Summary Table

The final summary provides a comprehensive overview:

```
================================================================================
                           BENCHMARK SUMMARY
================================================================================
Operation                               Setup (s)     KeyGen (s)    Precomp (s)    Eval (s)      Total (s)     
------------------------------------------------------------------------------------------------
CKKS to FHEW Switching                 0.125         2.456         0.089         0.234         2.904         
FHEW to CKKS Switching                 0.098         1.876         0.000         0.567         2.541         
Comparison via Scheme Switching        0.156         2.123         0.045         0.789         3.113         
Argmin via Scheme Switching            0.167         2.345         0.067         1.234         3.813         
Argmin via Scheme Switching (Alt)      0.178         2.456         0.078         1.345         4.057         
================================================================================
```

## Performance Considerations

### Parameter Selection

- **Depth**: Higher depth supports more operations but increases key size and computation time
- **Slots**: More slots enable larger vector operations but require more memory
- **Ring Dimension**: Larger ring dimensions provide better security but increase computation overhead

### Optimization Tips

1. **Use Release Builds**: Ensure `CMAKE_BUILD_TYPE=Release` for optimal performance
2. **Memory Management**: Large configurations require significant RAM
3. **Parallel Processing**: Enable OpenMP for better performance on multi-core systems
4. **Warm-up Runs**: First run may be slower due to system initialization

## Troubleshooting

### Common Issues

1. **OpenFHE Not Found**: Ensure OpenFHE is properly installed and `find_package(OpenFHE)` succeeds
2. **Memory Errors**: Large configurations may exceed available RAM
3. **Compilation Errors**: Verify C++17 support and OpenFHE compatibility

### Debug Mode

For debugging, build with debug symbols:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

## Extending the Benchmark Suite

### Adding New Workloads

1. Add function declaration to `workloads.h`
2. Implement function in `workloads.cpp`
3. Add call to `main.cpp` in `RunAllBenchmarks()`
4. Update CMakeLists.txt if new source files are added

### Custom Metrics

Modify the `BenchmarkResult` structure in `workloads.h` to include additional metrics:

```cpp
struct BenchmarkResult {
    // ... existing fields ...
    double memoryUsage;    // Memory usage in MB
    uint64_t keySize;      // Key size in bytes
    // ... additional fields ...
};
```

## License

This project is licensed under the BSD 2-Clause License - see the LICENSE file for details.

## Contributing

Contributions are welcome! Please ensure:

1. Code follows existing style and conventions
2. New workloads include proper error handling
3. Performance measurements are accurate and reproducible
4. Documentation is updated for new features

## References

- [OpenFHE Documentation](https://openfhe-development.readthedocs.io/)
- [Scheme Switching Paper](https://eprint.iacr.org/2021/1402)
- [CKKS Scheme](https://eprint.iacr.org/2016/421)
- [FHEW Scheme](https://eprint.iacr.org/2014/816)
