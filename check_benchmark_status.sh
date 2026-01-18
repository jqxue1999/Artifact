#!/bin/bash

# Benchmark Status Checker
# Use this to check if full benchmarks are still running or completed
# Useful when SSH session disconnects during long-running benchmarks

echo "========================================="
echo "FHE Artifact - Benchmark Status Checker"
echo "========================================="
echo ""

# Check if benchmark process is running
BENCHMARK_RUNNING=false
if [ -f "full_benchmark_results/.benchmark_running" ]; then
    BENCHMARK_PID=$(cat full_benchmark_results/.benchmark_running | grep "PID:" | awk '{print $2}')
    if kill -0 "$BENCHMARK_PID" 2>/dev/null; then
        BENCHMARK_RUNNING=true
    fi
fi

if [ "$BENCHMARK_RUNNING" = true ]; then
    echo -e "\033[0;33m⚙ Benchmarks are RUNNING\033[0m"
    echo ""
    echo "Process ID: $BENCHMARK_PID"
    ps -p "$BENCHMARK_PID" -o pid,etime,cmd --no-headers 2>/dev/null | sed 's/^/  /'
    echo ""
else
    echo -e "\033[0;32m✓ No benchmark processes running\033[0m"
    echo ""
fi

# Check for status file
if [ -f "full_benchmark_results/.benchmark_status" ]; then
    echo "----------------------------------------"
    echo "Progress Status:"
    echo "----------------------------------------"
    cat full_benchmark_results/.benchmark_status
    echo "----------------------------------------"
    echo ""
fi

# Check for results directory
if [ -d "full_benchmark_results" ]; then
    echo "Results Directory: full_benchmark_results/"
    echo ""

    # Count log files by category
    TFHE_LOGS=$(find full_benchmark_results -name "TFHE-*.log" 2>/dev/null | wc -l)
    SCHEME_LOGS=$(find full_benchmark_results -name "Scheme-*.log" 2>/dev/null | wc -l)
    ENCODING_LOGS=$(find full_benchmark_results -name "Encoding-*.log" 2>/dev/null | wc -l)
    TOTAL_LOGS=$((TFHE_LOGS + SCHEME_LOGS + ENCODING_LOGS))

    echo "Completed benchmarks:"
    echo "  - TFHE: $TFHE_LOGS/5 benchmarks"
    echo "  - Scheme Switching: $SCHEME_LOGS/5 benchmarks"
    echo "  - Encoding Switching: $ENCODING_LOGS/5 benchmarks"
    echo "  - Total: $TOTAL_LOGS/15 benchmarks"
    echo ""

    # Show recent log files
    if [ $TOTAL_LOGS -gt 0 ]; then
        echo "Recent log files (last 5):"
        ls -lth full_benchmark_results/*.log 2>/dev/null | head -5 | while read -r line; do
            filename=$(echo "$line" | awk '{print $9}')
            filesize=$(echo "$line" | awk '{print $5}')
            filetime=$(echo "$line" | awk '{print $6, $7, $8}')
            basename=$(basename "$filename")
            echo "  - $basename ($filesize, $filetime)"
        done
        echo ""
    fi

    # Check for completion marker
    if [ -f "full_benchmark_results/.benchmark_complete" ]; then
        echo "========================================="
        echo -e "\033[0;32m✓ BENCHMARKS COMPLETED!\033[0m"
        echo "========================================="
        cat "full_benchmark_results/.benchmark_complete"
        echo "========================================="
        echo ""
    elif [ -f "full_benchmark_results/.benchmark_failed" ]; then
        echo "========================================="
        echo -e "\033[0;31m✗ BENCHMARKS FAILED\033[0m"
        echo "========================================="
        cat "full_benchmark_results/.benchmark_failed"
        echo "========================================="
        echo ""
    elif [ "$BENCHMARK_RUNNING" = false ] && [ $TOTAL_LOGS -lt 15 ]; then
        echo -e "\033[0;33m⚠ Benchmarks INCOMPLETE (not running, $TOTAL_LOGS/15 done)\033[0m"
        echo ""
        echo "Possible reasons:"
        echo "  - Process was killed"
        echo "  - System crash"
        echo "  - Out of memory/disk space"
        echo ""
        echo "Check the last log file for errors:"
        LAST_LOG=$(ls -t full_benchmark_results/*.log 2>/dev/null | head -1)
        if [ -n "$LAST_LOG" ]; then
            echo "  tail -50 $LAST_LOG"
        fi
        echo ""
    fi
else
    echo "No results directory found."
    echo ""
    echo "Benchmarks have not been started yet or results were deleted."
    echo ""
fi

# Check system resources
echo "========================================="
echo "System Resources"
echo "========================================="
echo "Memory:"
free -h 2>/dev/null | grep -E "Mem:|Swap:" || echo "  (free command not available)"
echo ""
echo "Disk space (current directory):"
df -h . 2>/dev/null | tail -1 | awk '{print "  Available: " $4 " / " $2 " (" $5 " used)"}' || du -sh . 2>/dev/null
echo ""

# Provide helpful commands
echo "========================================="
echo "Useful Commands"
echo "========================================="
echo ""

if [ "$BENCHMARK_RUNNING" = false ] && [ ! -f "full_benchmark_results/.benchmark_complete" ]; then
    echo "To start full benchmarks in background:"
    echo "  nohup ./run_full_benchmarks.sh > benchmark.log 2>&1 &"
    echo ""
    echo "Or use tmux/screen to keep session alive:"
    echo "  tmux new -s benchmark"
    echo "  ./run_full_benchmarks.sh"
    echo "  # Press Ctrl+B then D to detach"
    echo "  # Later: tmux attach -t benchmark"
    echo ""
elif [ "$BENCHMARK_RUNNING" = true ]; then
    echo "To view live progress:"
    SUMMARY_LOG=$(ls -t full_benchmark_results/benchmark_summary_*.txt 2>/dev/null | head -1)
    if [ -n "$SUMMARY_LOG" ]; then
        echo "  tail -f $SUMMARY_LOG"
    fi
    echo ""
    echo "To view current status file:"
    echo "  watch -n 10 cat full_benchmark_results/.benchmark_status"
    echo ""
    echo "To check which benchmark is currently running:"
    echo "  ps aux | grep -E 'cargo|workload|decision_tree|sorting|floyd|database' | grep -v grep"
    echo ""
fi

echo "To check status again:"
echo "  ./check_benchmark_status.sh"
echo ""

echo "To view summary of completed benchmarks:"
SUMMARY_LOG=$(ls -t full_benchmark_results/benchmark_summary_*.txt 2>/dev/null | head -1)
if [ -n "$SUMMARY_LOG" ]; then
    echo "  cat $SUMMARY_LOG"
else
    echo "  cat full_benchmark_results/benchmark_summary_*.txt"
fi
echo ""
