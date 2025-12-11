# Dockerfile for FHE Artifact
# Provides a complete environment for running TFHE, Scheme Switching, and Encoding Switching benchmarks

FROM ubuntu:22.04

# Prevent interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

# Set working directory
WORKDIR /artifact

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    wget \
    pkg-config \
    libssl-dev \
    m4 \
    libgmp-dev \
    libntl-dev \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Install Rust
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
ENV PATH="/root/.cargo/bin:${PATH}"

# Verify Rust installation
RUN rustc --version && cargo --version

# Clone and build OpenFHE
RUN git clone https://github.com/openfheorg/openfhe-development.git && \
    cd openfhe-development && \
    mkdir build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/artifact/openfhelibs \
          -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc) && \
    make install && \
    cd /artifact && \
    rm -rf openfhe-development

# Copy patched HElib header for encoding switching extensions
COPY Ctxt_patched.h /artifact/

# Clone and build HElib (specific version compatible with encoding_switching)
# with custom extensions for HE-Bridge
RUN git clone https://github.com/homenc/HElib.git && \
    cd HElib && \
    git checkout 3e337a66a91a92d49de6a9505340826b0eb71081 && \
    cp /artifact/Ctxt_patched.h include/helib/Ctxt.h && \
    mkdir build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/artifact/helib_install \
          -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc) && \
    make install && \
    cd /artifact && \
    rm -rf HElib

# Clone HE-Bridge framework
RUN git clone https://github.com/UCF-Lou-Lab-PET/HE-Bridge.git

# Copy artifact source code
COPY rust/ /artifact/rust/
COPY scheme_switching/ /artifact/scheme_switching/
COPY encoding_switching/ /artifact/encoding_switching/
COPY *.sh /artifact/
COPY *.md /artifact/
COPY LICENSE /artifact/

# Make scripts executable
RUN chmod +x /artifact/*.sh

# Pre-build TFHE projects (to speed up first run)
RUN cd /artifact/rust/tfhe-example/workloads && cargo build && \
    cd /artifact/rust/tfhe-example/sorting && cargo build && \
    cd /artifact/rust/tfhe-example/floyd && cargo build && \
    cd /artifact/rust/tfhe-example/decision_tree && cargo build && \
    cd /artifact/rust/tfhe-example/private_db && cargo build

# Set environment variables for CMake to find libraries
ENV CMAKE_PREFIX_PATH="/artifact/openfhelibs:/artifact/helib_install:${CMAKE_PREFIX_PATH}"
ENV PKG_CONFIG_PATH="/artifact/openfhelibs/lib/pkgconfig:/artifact/helib_install/lib/pkgconfig:${PKG_CONFIG_PATH}"

# Build scheme switching
RUN cd /artifact/scheme_switching && \
    mkdir -p build && cd build && \
    cmake -DCMAKE_PREFIX_PATH=/artifact/openfhelibs .. && \
    make -j$(nproc)

# Build encoding switching
RUN cd /artifact/encoding_switching && \
    mkdir -p build && cd build && \
    cmake -DCMAKE_PREFIX_PATH=/artifact/helib_install .. && \
    make -j$(nproc)

# Set environment variables for libraries at runtime
ENV LD_LIBRARY_PATH="/artifact/openfhelibs/lib:/artifact/helib_install/lib:${LD_LIBRARY_PATH}"

# Create results directories
RUN mkdir -p /artifact/results/quick_tests /artifact/results/full_benchmarks

# Set default command to bash
CMD ["/bin/bash"]

# Add labels for metadata
LABEL maintainer="FHE Artifact Authors"
LABEL description="Complete environment for FHE benchmarks (TFHE, Scheme Switching, Encoding Switching)"
LABEL version="1.0"
