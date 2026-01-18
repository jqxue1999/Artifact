#!/bin/bash

# Quick test script to verify status tracking works
# This simulates a mini benchmark run

echo "Creating test full_benchmark_results directory..."
mkdir -p full_benchmark_results

echo "Creating simulated running marker..."
echo "PID: $$" > full_benchmark_results/.benchmark_running
echo "Started: $(date)" >> full_benchmark_results/.benchmark_running
echo "Total: 3 benchmarks" >> full_benchmark_results/.benchmark_running

echo "Creating simulated status file..."
echo "Progress: 1/3 (33%)" > full_benchmark_results/.benchmark_status
echo "Last Completed: Test-Benchmark-1" >> full_benchmark_results/.benchmark_status
echo "Last Updated: $(date)" >> full_benchmark_results/.benchmark_status

echo "Creating simulated log files..."
echo "Test benchmark 1 output" > full_benchmark_results/Test-Benchmark-1_20260117_120000.log
echo "Test benchmark 2 output" > full_benchmark_results/Test-Benchmark-2_20260117_120100.log

echo ""
echo "Test files created. Now check status:"
echo "./check_benchmark_status.sh"
echo ""
echo "The status checker should show:"
echo "  - Benchmarks RUNNING (PID: $$)"
echo "  - Progress: 1/3 (33%)"
echo "  - 2 log files"
echo ""
echo "Press Enter to check status..."
read

./check_benchmark_status.sh

echo ""
echo "Press Enter to simulate completion..."
read

echo "Simulating completion..."
rm -f full_benchmark_results/.benchmark_running
echo "Completed: $(date)" > full_benchmark_results/.benchmark_complete
echo "Total Duration: 00:05:30" >> full_benchmark_results/.benchmark_complete
echo "All 3/3 benchmarks finished successfully" >> full_benchmark_results/.benchmark_complete

echo ""
echo "./check_benchmark_status.sh"
./check_benchmark_status.sh

echo ""
echo "Test complete! Cleaning up..."
rm -rf full_benchmark_results
echo "Done!"
