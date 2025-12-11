#!/bin/bash
################################################################################
# Quick Docker Test Script
#
# This script performs a quick sanity check of the Docker environment.
# It builds the image and runs basic tests for all three implementations.
#
# Estimated time: 30-40 minutes (build) + 10 minutes (tests)
################################################################################

set -e

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo "================================================================================"
echo "  FHE Artifact - Quick Docker Test"
echo "================================================================================"
echo ""

# Build
echo -e "${BLUE}Building Docker image (this takes 30-40 minutes)...${NC}"
docker-compose build
echo -e "${GREEN}✓ Build complete${NC}"
echo ""

# Start
echo -e "${BLUE}Starting container...${NC}"
docker-compose up -d
sleep 3
echo -e "${GREEN}✓ Container started${NC}"
echo ""

# Test each implementation
echo -e "${YELLOW}Running quick tests...${NC}"
echo ""

echo -e "${BLUE}[1/3] Testing TFHE...${NC}"
docker exec fhe-artifact timeout 120 /artifact/rust/tfhe-example/workloads/target/debug/tfhe-example
echo -e "${GREEN}✓ TFHE working${NC}"
echo ""

echo -e "${BLUE}[2/3] Testing Scheme Switching...${NC}"
docker exec fhe-artifact /artifact/scheme_switching/build/bin/test_basic
echo -e "${GREEN}✓ Scheme Switching working${NC}"
echo ""

echo -e "${BLUE}[3/3] Testing Encoding Switching...${NC}"
docker exec fhe-artifact bash -c "cd /artifact/encoding_switching/build/bin && timeout 180 ./workload | head -50"
echo -e "${GREEN}✓ Encoding Switching working${NC}"
echo ""

echo "================================================================================"
echo -e "${GREEN}  All tests passed!${NC}"
echo "================================================================================"
echo ""
echo "The Docker environment is ready for full evaluation."
echo ""
echo "To run full benchmarks:"
echo "  ./run_docker_evaluation.sh full"
echo ""
echo "To stop the container:"
echo "  docker-compose down"
echo ""
