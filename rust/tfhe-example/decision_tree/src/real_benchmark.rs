use tfhe::prelude::*;
use tfhe::{generate_keys, set_server_key, ConfigBuilder, FheUint6, FheUint8, FheUint16};
use std::time::{Duration, Instant};
use crate::complexity_analysis::*;

/// Real benchmark results from actual TFHE operations
#[derive(Debug, Clone)]
pub struct RealBenchmarkResults {
    pub cp_mul_6bit: std::time::Duration,
    pub cp_mul_8bit: std::time::Duration,
    pub cp_mul_16bit: std::time::Duration,
    pub mul_cp_6bit: std::time::Duration,
    pub mul_cp_8bit: std::time::Duration,
    pub mul_cp_16bit: std::time::Duration,
}

/// Benchmark actual cp_mul and mul_cp operations using TFHE
pub fn benchmark_real_operations() -> RealBenchmarkResults {
    let cp_mul_6bit = benchmark_cp_mul_6bit();
    let cp_mul_8bit = benchmark_cp_mul_8bit();
    let cp_mul_16bit = benchmark_cp_mul_16bit();
    let mul_cp_6bit = benchmark_mul_cp_6bit();
    let mul_cp_8bit = benchmark_mul_cp_8bit();
    let mul_cp_16bit = benchmark_mul_cp_16bit();
    
    RealBenchmarkResults {
        cp_mul_6bit,
        cp_mul_8bit,
        cp_mul_16bit,
        mul_cp_6bit,
        mul_cp_8bit,
        mul_cp_16bit,
    }
}

/// Benchmark 6-bit cp_mul: (feature < threshold) * path_weight
fn benchmark_cp_mul_6bit() -> std::time::Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_feature = FheUint6::try_encrypt(25u8, &client_key).unwrap();
    let encrypted_threshold = FheUint6::try_encrypt(30u8, &client_key).unwrap();
    let encrypted_weight = FheUint6::try_encrypt(15u8, &client_key).unwrap();

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_feature.lt(&encrypted_threshold);
    let encrypted_cmp_6: FheUint6 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_cmp_6 * &encrypted_weight;
    let duration = start.elapsed();
    
    let _clear_product: u8 = _encrypted_product.decrypt(&client_key);
    
    duration
}

/// Benchmark 8-bit cp_mul: (feature < threshold) * path_weight
fn benchmark_cp_mul_8bit() -> std::time::Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_feature = FheUint8::try_encrypt(100u8, &client_key).unwrap();
    let encrypted_threshold = FheUint8::try_encrypt(150u8, &client_key).unwrap();
    let encrypted_weight = FheUint8::try_encrypt(200u8, &client_key).unwrap();

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_feature.lt(&encrypted_threshold);
    let encrypted_cmp_8: FheUint8 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_cmp_8 * &encrypted_weight;
    let duration = start.elapsed();
    
    let _clear_product: u8 = _encrypted_product.decrypt(&client_key);
    
    duration
}

/// Benchmark 16-bit cp_mul: (feature < threshold) * path_weight
fn benchmark_cp_mul_16bit() -> std::time::Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_feature = FheUint16::try_encrypt(1000u16, &client_key).unwrap();
    let encrypted_threshold = FheUint16::try_encrypt(1500u16, &client_key).unwrap();
    let encrypted_weight = FheUint16::try_encrypt(2000u16, &client_key).unwrap();

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_feature.lt(&encrypted_threshold);
    let encrypted_cmp_16: FheUint16 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_cmp_16 * &encrypted_weight;
    let duration = start.elapsed();
    
    let _clear_product: u16 = _encrypted_product.decrypt(&client_key);
    
    duration
}

/// Benchmark 6-bit mul_cp: path_value * (feature < threshold)
fn benchmark_mul_cp_6bit() -> std::time::Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_path_value = FheUint6::try_encrypt(20u8, &client_key).unwrap();
    let encrypted_feature = FheUint6::try_encrypt(25u8, &client_key).unwrap();
    let encrypted_threshold = FheUint6::try_encrypt(30u8, &client_key).unwrap();

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_feature.lt(&encrypted_threshold);
    let encrypted_cmp_6: FheUint6 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_path_value * &encrypted_cmp_6;
    let duration = start.elapsed();
    
    let _clear_product: u8 = _encrypted_product.decrypt(&client_key);
    
    duration
}

/// Benchmark 8-bit mul_cp: path_value * (feature < threshold)
fn benchmark_mul_cp_8bit() -> std::time::Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_path_value = FheUint8::try_encrypt(50u8, &client_key).unwrap();
    let encrypted_feature = FheUint8::try_encrypt(100u8, &client_key).unwrap();
    let encrypted_threshold = FheUint8::try_encrypt(150u8, &client_key).unwrap();

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_feature.lt(&encrypted_threshold);
    let encrypted_cmp_8: FheUint8 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_path_value * &encrypted_cmp_8;
    let duration = start.elapsed();
    
    let _clear_product: u8 = _encrypted_product.decrypt(&client_key);
    
    duration
}

/// Benchmark 16-bit mul_cp: path_value * (feature < threshold)
fn benchmark_mul_cp_16bit() -> std::time::Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_path_value = FheUint16::try_encrypt(500u16, &client_key).unwrap();
    let encrypted_feature = FheUint16::try_encrypt(1000u16, &client_key).unwrap();
    let encrypted_threshold = FheUint16::try_encrypt(1500u16, &client_key).unwrap();

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_feature.lt(&encrypted_threshold);
    let encrypted_cmp_16: FheUint16 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_path_value * &encrypted_cmp_16;
    let duration = start.elapsed();
    
    let _clear_product: u16 = _encrypted_product.decrypt(&client_key);
    
    duration
}

/// Benchmark decision tree by actually running the required number of operations
pub fn benchmark_decision_tree_actual(tree_depth: usize, bit_width: u8) -> Duration {
    let analysis = analyze_decision_tree(tree_depth);
    
    match bit_width {
        6 => benchmark_tree_actual_6bit(analysis.total_cp_mul_operations, analysis.total_mul_cp_operations),
        8 => benchmark_tree_actual_8bit(analysis.total_cp_mul_operations, analysis.total_mul_cp_operations),
        16 => benchmark_tree_actual_16bit(analysis.total_cp_mul_operations, analysis.total_mul_cp_operations),
        _ => panic!("Unsupported bit width: {}", bit_width),
    }
}

/// Actually run the required number of 6-bit operations
fn benchmark_tree_actual_6bit(cp_mul_count: usize, mul_cp_count: usize) -> Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let encrypted_feature = FheUint6::try_encrypt(25u8, &client_key).unwrap();
    let encrypted_threshold = FheUint6::try_encrypt(30u8, &client_key).unwrap();
    let encrypted_weight = FheUint6::try_encrypt(15u8, &client_key).unwrap();

    let start = Instant::now();
    
    // Run mul_cp operations (path selection)
    for _ in 0..mul_cp_count {
        let encrypted_cmp = encrypted_feature.lt(&encrypted_threshold);
        let encrypted_cmp_6: FheUint6 = encrypted_cmp.cast_into();
        let _encrypted_product = &encrypted_weight * &encrypted_cmp_6;
    }
    
    // Run cp_mul operations (comparisons and conditional assignments)
    for _ in 0..cp_mul_count {
        let encrypted_cmp = encrypted_feature.lt(&encrypted_threshold);
        let encrypted_cmp_6: FheUint6 = encrypted_cmp.cast_into();
        let _encrypted_product = &encrypted_cmp_6 * &encrypted_weight;
    }
    
    start.elapsed()
}

/// Actually run the required number of 8-bit operations
fn benchmark_tree_actual_8bit(cp_mul_count: usize, mul_cp_count: usize) -> Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let encrypted_feature = FheUint8::try_encrypt(100u8, &client_key).unwrap();
    let encrypted_threshold = FheUint8::try_encrypt(150u8, &client_key).unwrap();
    let encrypted_weight = FheUint8::try_encrypt(200u8, &client_key).unwrap();

    let start = Instant::now();
    
    // Run mul_cp operations
    for _ in 0..mul_cp_count {
        let encrypted_cmp = encrypted_feature.lt(&encrypted_threshold);
        let encrypted_cmp_8: FheUint8 = encrypted_cmp.cast_into();
        let _encrypted_product = &encrypted_weight * &encrypted_cmp_8;
    }
    
    // Run cp_mul operations
    for _ in 0..cp_mul_count {
        let encrypted_cmp = encrypted_feature.lt(&encrypted_threshold);
        let encrypted_cmp_8: FheUint8 = encrypted_cmp.cast_into();
        let _encrypted_product = &encrypted_cmp_8 * &encrypted_weight;
    }
    
    start.elapsed()
}

/// Actually run the required number of 16-bit operations
fn benchmark_tree_actual_16bit(cp_mul_count: usize, mul_cp_count: usize) -> Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let encrypted_feature = FheUint16::try_encrypt(1000u16, &client_key).unwrap();
    let encrypted_threshold = FheUint16::try_encrypt(1500u16, &client_key).unwrap();
    let encrypted_weight = FheUint16::try_encrypt(2000u16, &client_key).unwrap();

    let start = Instant::now();
    
    // Run mul_cp operations
    for _ in 0..mul_cp_count {
        let encrypted_cmp = encrypted_feature.lt(&encrypted_threshold);
        let encrypted_cmp_16: FheUint16 = encrypted_cmp.cast_into();
        let _encrypted_product = &encrypted_weight * &encrypted_cmp_16;
    }
    
    // Run cp_mul operations
    for _ in 0..cp_mul_count {
        let encrypted_cmp = encrypted_feature.lt(&encrypted_threshold);
        let encrypted_cmp_16: FheUint16 = encrypted_cmp.cast_into();
        let _encrypted_product = &encrypted_cmp_16 * &encrypted_weight;
    }
    
    start.elapsed()
}

/// Run comprehensive decision tree analysis with actual operation counting
pub fn run_real_analysis() {
    println!("\nDecision Tree Algorithm Results (Actual Operation Counting):");
    
    // Test different tree depths and bit widths
    let test_tree_depths = vec![2, 4, 6, 8];
    let bit_widths = vec![6, 8, 16];
    
    for &tree_depth in &test_tree_depths {
        for &bit_width in &bit_widths {
            println!("Running depth {}, {}-bit...", tree_depth, bit_width);
            let actual_time = benchmark_decision_tree_actual(tree_depth, bit_width);
            println!("Decision Tree: depth {}, {}-bit: {}", 
                     tree_depth, bit_width, format_duration(actual_time));
        }
    }
    
    println!("\nDetailed Analysis:");
    for &tree_depth in &test_tree_depths {
        println!("\n=== Depth {} ===", tree_depth);
        let analysis = analyze_decision_tree(tree_depth);
        println!("  Total nodes: {}", analysis.total_nodes);
        println!("  Comparisons: {}", analysis.comparison_count);
        println!("  Conditional assignments: {}", analysis.conditional_assignment_count);
        println!("  Total cp_mul ops: {}", analysis.total_cp_mul_operations);
        println!("  Total mul_cp ops: {}", analysis.total_mul_cp_operations);
    }
}

/// Format duration in a simple way
fn format_duration(duration: Duration) -> String {
    let total_secs = duration.as_secs_f64();
    
    if total_secs < 60.0 {
        format!("{:.1}s", total_secs)
    } else if total_secs < 3600.0 {
        format!("{:.1}m", total_secs / 60.0)
    } else if total_secs < 86400.0 {
        format!("{:.1}h", total_secs / 3600.0)
    } else {
        format!("{:.1}d", total_secs / 86400.0)
    }
}
