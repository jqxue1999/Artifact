use std::time::Duration;

/// Analysis of sorting algorithm complexity in terms of comparison operations
#[derive(Debug, Clone)]
pub struct SortingComplexity {
    pub algorithm_name: String,
    pub array_size: usize,
    pub comparison_count: usize,
    pub swap_count: usize,
    pub total_cp_mul_operations: usize,
}

impl SortingComplexity {
    /// Estimate total execution time based on single cp_mul operation time
    pub fn estimate_time(&self, cp_mul_time: Duration) -> Duration {
        cp_mul_time * self.total_cp_mul_operations as u32
    }
    
    /// Print detailed analysis
    pub fn print_analysis(&self, cp_mul_time: Duration) {
        println!("Algorithm: {}", self.algorithm_name);
        println!("  Array size: {}", self.array_size);
        println!("  Comparisons: {}", self.comparison_count);
        println!("  Swaps: {}", self.swap_count);
        println!("  Total cp_mul ops: {}", self.total_cp_mul_operations);
        println!("  Estimated time: {:.2?}", self.estimate_time(cp_mul_time));
        println!();
    }
}

/// Analyze bubble sort complexity
/// Each comparison is 1 cp_mul operation
/// Each swap requires 2 conditional assignments = 2 cp_mul operations  
pub fn analyze_bubble_sort(n: usize) -> SortingComplexity {
    let comparison_count = n * (n - 1) / 2;  // O(n²) comparisons
    let swap_count = comparison_count;  // Worst case: every comparison results in a swap
    let total_cp_mul_operations = comparison_count + (swap_count * 2);  // 1 for compare + 2 for conditional swap
    
    SortingComplexity {
        algorithm_name: "Bubble Sort".to_string(),
        array_size: n,
        comparison_count,
        swap_count,
        total_cp_mul_operations,
    }
}

/// Benchmark bubble sort by actually running cp_mul operations
pub fn benchmark_bubble_sort_actual(n: usize, bit_width: u8) -> Duration {
    let bubble_analysis = analyze_bubble_sort(n);
    let total_ops = bubble_analysis.total_cp_mul_operations;
    
    let start = std::time::Instant::now();
    
    // Actually run cp_mul operations that many times
    for _ in 0..total_ops {
        match bit_width {
            6 => { let _ = single_cp_mul_6bit(); },
            8 => { let _ = single_cp_mul_8bit(); },
            16 => { let _ = single_cp_mul_16bit(); },
            _ => panic!("Unsupported bit width"),
        }
    }
    
    start.elapsed()
}

/// Single cp_mul operation for benchmarking - 6 bit
fn single_cp_mul_6bit() -> Duration {
    use tfhe::prelude::*;
    use tfhe::{generate_keys, set_server_key, ConfigBuilder, FheUint6};
    
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);
    
    let encrypted_a = FheUint6::try_encrypt(3u8, &client_key).unwrap();
    let encrypted_b = FheUint6::try_encrypt(5u8, &client_key).unwrap();
    let encrypted_c = FheUint6::try_encrypt(7u8, &client_key).unwrap();
    
    set_server_key(server_keys);
    
    let start = std::time::Instant::now();
    let encrypted_cmp = encrypted_a.le(&encrypted_b);
    let encrypted_cmp_6: FheUint6 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_cmp_6 * &encrypted_c;
    start.elapsed()
}

/// Single cp_mul operation for benchmarking - 8 bit
fn single_cp_mul_8bit() -> Duration {
    use tfhe::prelude::*;
    use tfhe::{generate_keys, set_server_key, ConfigBuilder, FheUint8};
    
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);
    
    let encrypted_a = FheUint8::try_encrypt(100u8, &client_key).unwrap();
    let encrypted_b = FheUint8::try_encrypt(150u8, &client_key).unwrap();
    let encrypted_c = FheUint8::try_encrypt(200u8, &client_key).unwrap();
    
    set_server_key(server_keys);
    
    let start = std::time::Instant::now();
    let encrypted_cmp = encrypted_a.le(&encrypted_b);
    let encrypted_cmp_8: FheUint8 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_cmp_8 * &encrypted_c;
    start.elapsed()
}

/// Single cp_mul operation for benchmarking - 16 bit
fn single_cp_mul_16bit() -> Duration {
    use tfhe::prelude::*;
    use tfhe::{generate_keys, set_server_key, ConfigBuilder, FheUint16};
    
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);
    
    let encrypted_a = FheUint16::try_encrypt(1000u16, &client_key).unwrap();
    let encrypted_b = FheUint16::try_encrypt(1500u16, &client_key).unwrap();
    let encrypted_c = FheUint16::try_encrypt(2000u16, &client_key).unwrap();
    
    set_server_key(server_keys);
    
    let start = std::time::Instant::now();
    let encrypted_cmp = encrypted_a.le(&encrypted_b);
    let encrypted_cmp_16: FheUint16 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_cmp_16 * &encrypted_c;
    start.elapsed()
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
