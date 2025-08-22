use tfhe::prelude::*;
use tfhe::{generate_keys, set_server_key, ConfigBuilder, FheUint6, FheUint8, FheUint16};
use std::time::Instant;
use crate::complexity_analysis::*;

/// Real benchmark results from actual TFHE operations
#[derive(Debug, Clone)]
pub struct RealBenchmarkResults {
    pub cp_mul_6bit: std::time::Duration,
    pub cp_mul_8bit: std::time::Duration,
    pub cp_mul_16bit: std::time::Duration,
}

/// Benchmark actual cp_mul operations using TFHE
pub fn benchmark_real_cp_mul() -> RealBenchmarkResults {
    let time_6bit = benchmark_cp_mul_6bit();
    let time_8bit = benchmark_cp_mul_8bit();
    let time_16bit = benchmark_cp_mul_16bit();
    
    RealBenchmarkResults {
        cp_mul_6bit: time_6bit,
        cp_mul_8bit: time_8bit,
        cp_mul_16bit: time_16bit,
    }
}

/// Benchmark 6-bit cp_mul: (a <= b) * c
fn benchmark_cp_mul_6bit() -> std::time::Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint6::try_encrypt(3u8, &client_key).unwrap();
    let encrypted_b = FheUint6::try_encrypt(5u8, &client_key).unwrap();
    let encrypted_c = FheUint6::try_encrypt(7u8, &client_key).unwrap();

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_a.le(&encrypted_b);
    let encrypted_cmp_6: FheUint6 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_cmp_6 * &encrypted_c;
    let duration = start.elapsed();
    
    let _clear_product: u8 = _encrypted_product.decrypt(&client_key);
    
    duration
}

/// Benchmark 8-bit cp_mul: (a <= b) * c
fn benchmark_cp_mul_8bit() -> std::time::Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint8::try_encrypt(100u8, &client_key).unwrap();
    let encrypted_b = FheUint8::try_encrypt(150u8, &client_key).unwrap();
    let encrypted_c = FheUint8::try_encrypt(200u8, &client_key).unwrap();

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_a.le(&encrypted_b);
    let encrypted_cmp_8: FheUint8 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_cmp_8 * &encrypted_c;
    let duration = start.elapsed();
    
    let _clear_product: u8 = _encrypted_product.decrypt(&client_key);
    
    duration
}

/// Benchmark 16-bit cp_mul: (a <= b) * c
fn benchmark_cp_mul_16bit() -> std::time::Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint16::try_encrypt(1000u16, &client_key).unwrap();
    let encrypted_b = FheUint16::try_encrypt(1500u16, &client_key).unwrap();
    let encrypted_c = FheUint16::try_encrypt(2000u16, &client_key).unwrap();

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_a.le(&encrypted_b);
    let encrypted_cmp_16: FheUint16 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_cmp_16 * &encrypted_c;
    let duration = start.elapsed();
    
    let _clear_product: u16 = _encrypted_product.decrypt(&client_key);
    
    duration
}

/// Run comprehensive analysis with real benchmark data
pub fn run_real_analysis() {
    let real_results = benchmark_real_cp_mul();
    
    println!("\nBubble Sort Results:");
    
    // Test different array sizes and bit widths
    let test_sizes = vec![8, 16, 32, 64];
    let bit_configs = vec![
        (real_results.cp_mul_6bit, 6),
        (real_results.cp_mul_8bit, 8),
        (real_results.cp_mul_16bit, 16),
    ];
    
    for &size in &test_sizes {
        for &(cp_mul_time, bit_width) in &bit_configs {
            analyze_bubble_sort_simple(size, cp_mul_time, bit_width);
        }
    }
}