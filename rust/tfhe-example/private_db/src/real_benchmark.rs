use tfhe::prelude::*;
use tfhe::{generate_keys, set_server_key, ConfigBuilder, FheUint8, FheUint16, FheUint32};
use std::time::{Duration, Instant};
use rand::Rng;
use crate::complexity_analysis::*;

/// Database record structure
#[derive(Debug, Clone)]
pub struct DatabaseRecord {
    pub id: u32,
    pub salary: u32,
    pub work_hours: u32,
    pub bonus: u32,
}

/// Real benchmark results from actual TFHE operations
#[derive(Debug, Clone)]
pub struct RealBenchmarkResults {
    pub cp_mul_8bit: Duration,
    pub cp_mul_16bit: Duration,
    pub cp_mul_32bit: Duration,
    pub mul_cp_8bit: Duration,
    pub mul_cp_16bit: Duration,
    pub mul_cp_32bit: Duration,
}

/// Benchmark actual cp_mul and mul_cp operations using TFHE
pub fn benchmark_real_operations() -> RealBenchmarkResults {
    let cp_mul_8bit = benchmark_cp_mul_8bit();
    let cp_mul_16bit = benchmark_cp_mul_16bit();
    let cp_mul_32bit = benchmark_cp_mul_32bit();
    let mul_cp_8bit = benchmark_mul_cp_8bit();
    let mul_cp_16bit = benchmark_mul_cp_16bit();
    let mul_cp_32bit = benchmark_mul_cp_32bit();
    
    RealBenchmarkResults {
        cp_mul_8bit,
        cp_mul_16bit,
        cp_mul_32bit,
        mul_cp_8bit,
        mul_cp_16bit,
        mul_cp_32bit,
    }
}

/// Benchmark 8-bit cp_mul: (a < b) * c
fn benchmark_cp_mul_8bit() -> Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint8::try_encrypt(100u8, &client_key).unwrap();
    let encrypted_b = FheUint8::try_encrypt(150u8, &client_key).unwrap();
    let encrypted_c = FheUint8::try_encrypt(1u8, &client_key).unwrap();

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_a.lt(&encrypted_b);
    let encrypted_cmp_8: FheUint8 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_cmp_8 * &encrypted_c;
    let duration = start.elapsed();
    
    let _clear_product: u8 = _encrypted_product.decrypt(&client_key);
    
    duration
}

/// Benchmark 16-bit cp_mul: (a < b) * c
fn benchmark_cp_mul_16bit() -> Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint16::try_encrypt(1000u16, &client_key).unwrap();
    let encrypted_b = FheUint16::try_encrypt(1500u16, &client_key).unwrap();
    let encrypted_c = FheUint16::try_encrypt(1u16, &client_key).unwrap();

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_a.lt(&encrypted_b);
    let encrypted_cmp_16: FheUint16 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_cmp_16 * &encrypted_c;
    let duration = start.elapsed();
    
    let _clear_product: u16 = _encrypted_product.decrypt(&client_key);
    
    duration
}

/// Benchmark 32-bit cp_mul: (a < b) * c
fn benchmark_cp_mul_32bit() -> Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint32::try_encrypt(10000u32, &client_key).unwrap();
    let encrypted_b = FheUint32::try_encrypt(15000u32, &client_key).unwrap();
    let encrypted_c = FheUint32::try_encrypt(1u32, &client_key).unwrap();

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_a.lt(&encrypted_b);
    let encrypted_cmp_32: FheUint32 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_cmp_32 * &encrypted_c;
    let duration = start.elapsed();
    
    let _clear_product: u32 = _encrypted_product.decrypt(&client_key);
    
    duration
}

/// Benchmark 8-bit mul_cp: a * (b < c)
fn benchmark_mul_cp_8bit() -> Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint8::try_encrypt(50u8, &client_key).unwrap();
    let encrypted_b = FheUint8::try_encrypt(100u8, &client_key).unwrap();
    let encrypted_c = FheUint8::try_encrypt(150u8, &client_key).unwrap();

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_b.lt(&encrypted_c);
    let encrypted_cmp_8: FheUint8 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_a * &encrypted_cmp_8;
    let duration = start.elapsed();
    
    let _clear_product: u8 = _encrypted_product.decrypt(&client_key);
    
    duration
}

/// Benchmark 16-bit mul_cp: a * (b < c)
fn benchmark_mul_cp_16bit() -> Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint16::try_encrypt(500u16, &client_key).unwrap();
    let encrypted_b = FheUint16::try_encrypt(1000u16, &client_key).unwrap();
    let encrypted_c = FheUint16::try_encrypt(1500u16, &client_key).unwrap();

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_b.lt(&encrypted_c);
    let encrypted_cmp_16: FheUint16 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_a * &encrypted_cmp_16;
    let duration = start.elapsed();
    
    let _clear_product: u16 = _encrypted_product.decrypt(&client_key);
    
    duration
}

/// Benchmark 32-bit mul_cp: a * (b < c)
fn benchmark_mul_cp_32bit() -> Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint32::try_encrypt(5000u32, &client_key).unwrap();
    let encrypted_b = FheUint32::try_encrypt(10000u32, &client_key).unwrap();
    let encrypted_c = FheUint32::try_encrypt(15000u32, &client_key).unwrap();

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_b.lt(&encrypted_c);
    let encrypted_cmp_32: FheUint32 = encrypted_cmp.cast_into();
    let _encrypted_product = &encrypted_a * &encrypted_cmp_32;
    let duration = start.elapsed();
    
    let _clear_product: u32 = _encrypted_product.decrypt(&client_key);
    
    duration
}

/// Generate random database records
pub fn generate_random_database(num_rows: usize) -> Vec<DatabaseRecord> {
    let mut rng = rand::thread_rng();
    let mut records = Vec::new();
    
    for i in 0..num_rows {
        records.push(DatabaseRecord {
            id: i as u32,
            salary: rng.gen_range(20..100),  // salary range: 20-99
            work_hours: rng.gen_range(40..80), // work_hours range: 40-79
            bonus: rng.gen_range(600..800),  // bonus range: 600-799
        });
    }
    
    records
}

/// Benchmark database aggregation by actually running the required number of operations
pub fn benchmark_database_aggregation_actual(num_rows: usize, bit_width: u8) -> Duration {
    let analysis = analyze_database_aggregation(num_rows);
    
    match bit_width {
        8 => benchmark_db_actual_8bit(analysis.total_cp_mul_operations, analysis.total_mul_cp_operations),
        16 => benchmark_db_actual_16bit(analysis.total_cp_mul_operations, analysis.total_mul_cp_operations),
        32 => benchmark_db_actual_32bit(analysis.total_cp_mul_operations, analysis.total_mul_cp_operations),
        _ => panic!("Unsupported bit width: {}", bit_width),
    }
}

/// Actually run the required number of 8-bit operations
fn benchmark_db_actual_8bit(cp_mul_count: usize, mul_cp_count: usize) -> Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let encrypted_salary = FheUint8::try_encrypt(50u8, &client_key).unwrap();
    let encrypted_work_hours = FheUint8::try_encrypt(60u8, &client_key).unwrap();
    let encrypted_bonus = FheUint8::try_encrypt(200u8, &client_key).unwrap();
    let encrypted_threshold1 = FheUint8::try_encrypt(100u8, &client_key).unwrap();
    let encrypted_threshold2 = FheUint8::try_encrypt(200u8, &client_key).unwrap();

    let start = Instant::now();
    
    // Run mul_cp operations (multiplications and AND operations)
    for _ in 0..mul_cp_count {
        let encrypted_product = &encrypted_salary * &encrypted_work_hours;
        let encrypted_cmp = encrypted_product.lt(&encrypted_threshold1);
        let encrypted_cmp_8: FheUint8 = encrypted_cmp.cast_into();
        let _encrypted_result = &encrypted_cmp_8 * &encrypted_threshold2;
    }
    
    // Run cp_mul operations (comparisons)
    for _ in 0..cp_mul_count {
        let encrypted_cmp = encrypted_salary.lt(&encrypted_work_hours);
        let encrypted_cmp_8: FheUint8 = encrypted_cmp.cast_into();
        let _encrypted_result = &encrypted_cmp_8 * &encrypted_bonus;
    }
    
    start.elapsed()
}

/// Actually run the required number of 16-bit operations
fn benchmark_db_actual_16bit(cp_mul_count: usize, mul_cp_count: usize) -> Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let encrypted_salary = FheUint16::try_encrypt(500u16, &client_key).unwrap();
    let encrypted_work_hours = FheUint16::try_encrypt(600u16, &client_key).unwrap();
    let encrypted_bonus = FheUint16::try_encrypt(700u16, &client_key).unwrap();
    let encrypted_threshold1 = FheUint16::try_encrypt(5000u16, &client_key).unwrap();
    let encrypted_threshold2 = FheUint16::try_encrypt(6000u16, &client_key).unwrap();

    let start = Instant::now();
    
    // Run mul_cp operations
    for _ in 0..mul_cp_count {
        let encrypted_product = &encrypted_salary * &encrypted_work_hours;
        let encrypted_cmp = encrypted_product.lt(&encrypted_threshold1);
        let encrypted_cmp_16: FheUint16 = encrypted_cmp.cast_into();
        let _encrypted_result = &encrypted_cmp_16 * &encrypted_threshold2;
    }
    
    // Run cp_mul operations
    for _ in 0..cp_mul_count {
        let encrypted_cmp = encrypted_salary.lt(&encrypted_work_hours);
        let encrypted_cmp_16: FheUint16 = encrypted_cmp.cast_into();
        let _encrypted_result = &encrypted_cmp_16 * &encrypted_bonus;
    }
    
    start.elapsed()
}

/// Actually run the required number of 32-bit operations
fn benchmark_db_actual_32bit(cp_mul_count: usize, mul_cp_count: usize) -> Duration {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let encrypted_salary = FheUint32::try_encrypt(5000u32, &client_key).unwrap();
    let encrypted_work_hours = FheUint32::try_encrypt(6000u32, &client_key).unwrap();
    let encrypted_bonus = FheUint32::try_encrypt(7000u32, &client_key).unwrap();
    let encrypted_threshold1 = FheUint32::try_encrypt(50000u32, &client_key).unwrap();
    let encrypted_threshold2 = FheUint32::try_encrypt(60000u32, &client_key).unwrap();

    let start = Instant::now();
    
    // Run mul_cp operations
    for _ in 0..mul_cp_count {
        let encrypted_product = &encrypted_salary * &encrypted_work_hours;
        let encrypted_cmp = encrypted_product.lt(&encrypted_threshold1);
        let encrypted_cmp_32: FheUint32 = encrypted_cmp.cast_into();
        let _encrypted_result = &encrypted_cmp_32 * &encrypted_threshold2;
    }
    
    // Run cp_mul operations
    for _ in 0..cp_mul_count {
        let encrypted_cmp = encrypted_salary.lt(&encrypted_work_hours);
        let encrypted_cmp_32: FheUint32 = encrypted_cmp.cast_into();
        let _encrypted_result = &encrypted_cmp_32 * &encrypted_bonus;
    }
    
    start.elapsed()
}

/// Run comprehensive database aggregation analysis with actual operation counting
pub fn run_real_analysis() {
    println!("\nPrivate Database Aggregation Results (Actual Operation Counting):");
    
    // Test different database sizes and bit widths
    let test_db_sizes = vec![64, 128, 256, 512];
    let bit_widths = vec![8, 16, 32];
    
    for &db_size in &test_db_sizes {
        for &bit_width in &bit_widths {
            println!("Running {} rows, {}-bit...", db_size, bit_width);
            let actual_time = benchmark_database_aggregation_actual(db_size, bit_width);
            println!("Database Aggregation: {} rows, {}-bit: {}", 
                     db_size, bit_width, format_duration(actual_time));
        }
    }
    
    println!("\nDetailed Analysis:");
    for &db_size in &test_db_sizes {
        println!("\n=== {} rows ===", db_size);
        let analysis = analyze_database_aggregation(db_size);
        println!("  Comparisons: {}", analysis.comparison_count);
        println!("  Multiplications: {}", analysis.multiplication_count);
        println!("  Total cp_mul ops: {}", analysis.total_cp_mul_operations);
        println!("  Total mul_cp ops: {}", analysis.total_mul_cp_operations);
    }
    
    // Show a sample of the query logic
    println!("\nSample Query Logic Demo:");
    let sample_db = generate_random_database(5);
    for record in &sample_db {
        let salary_times_hours = record.salary * record.work_hours;
        let salary_plus_bonus = record.salary + record.bonus;
        
        // Predicate 1: salary * work_hours BETWEEN 5000 AND 6000
        let pred1 = salary_times_hours >= 5000 && salary_times_hours <= 6000;
        
        // Predicate 2: salary + bonus BETWEEN 700 AND 800  
        let pred2 = salary_plus_bonus >= 700 && salary_plus_bonus <= 800;
        
        let satisfies_query = pred1 && pred2;
        
        println!("ID {}: salary={}, hours={}, bonus={}, salary*hours={}, salary+bonus={}, satisfies={}",
                 record.id, record.salary, record.work_hours, record.bonus, 
                 salary_times_hours, salary_plus_bonus, satisfies_query);
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
