use tfhe::prelude::*;
use tfhe::{generate_keys, set_server_key, FheUint6, FheUint8, FheUint12, FheUint16};
use std::time::Duration;
use crate::real_sorting::*;
use crate::security_params::get_secure_config;

/// Run comprehensive sorting benchmarks following the paper's experiments
pub fn run_real_sorting_benchmarks() {
    println!("\n{}", "=".repeat(80));
    println!("REAL Sorting Evaluation Benchmark");
    println!("{}\n", "=".repeat(80));

    println!("This benchmark evaluates actual private sorting on encrypted arrays.");
    println!("All comparisons and swaps are performed obliviously.\n");

    println!("Experiment 1: 8-element array with different bit widths");
    println!("{}", "-".repeat(80));
    println!("{:<12} {:<12} {:<15} {:<15} {:<10}",
             "Array Size", "Bit Width", "Time", "Comparisons", "Status");
    println!("{}", "-".repeat(80));

    let n = 8;
    let bit_widths = vec![6, 8, 12, 16];

    for &bit_width in &bit_widths {
        let comparisons = n * (n - 1);

        print!("{:<12} {:<12} ", n, bit_width);

        let result = match bit_width {
            6 => benchmark_sort_u6(n),
            8 => benchmark_sort_u8(n),
            12 => benchmark_sort_u12(n),
            16 => benchmark_sort_u16(n),
            _ => {
                println!("Unsupported bit width");
                continue;
            }
        };

        match result {
            Ok((duration, correct)) => {
                let status = if correct { "✓" } else { "✗ Wrong" };
                println!("{:<15} {:<15} {:<10}",
                         format_duration(duration),
                         comparisons,
                         status);
            }
            Err(e) => {
                println!("{:<15} {:<15} {:<10}",
                         "-",
                         comparisons,
                         format!("✗ {}", e));
            }
        }
    }

    println!();
    println!("Experiment 2: 8-bit inputs with different array sizes");
    println!("{}", "-".repeat(80));
    println!("{:<12} {:<12} {:<15} {:<15} {:<10}",
             "Array Size", "Bit Width", "Time", "Comparisons", "Status");
    println!("{}", "-".repeat(80));

    let array_sizes = vec![4, 8, 16, 32];
    let bit_width = 8;

    for &n in &array_sizes {
        let comparisons = n * (n - 1);

        print!("{:<12} {:<12} ", n, bit_width);

        let result = benchmark_sort_u8(n);

        match result {
            Ok((duration, correct)) => {
                let status = if correct { "✓" } else { "✗ Wrong" };
                println!("{:<15} {:<15} {:<10}",
                         format_duration(duration),
                         comparisons,
                         status);
            }
            Err(e) => {
                println!("{:<15} {:<15} {:<10}",
                         "-",
                         comparisons,
                         format!("✗ {}", e));
            }
        }
    }

    println!();
    println!("{}", "=".repeat(80));
}

/// Benchmark sorting with 8-bit encryption
fn benchmark_sort_u8(n: usize) -> Result<(Duration, bool), String> {
    let config = get_secure_config();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let arr_plain = create_sample_array(n);

    // Encrypt
    let arr_encrypted: Vec<FheUint8> = arr_plain
        .iter()
        .map(|&val| FheUint8::try_encrypt(val as u8, &client_key).unwrap())
        .collect();

    // Sort
    let (result_encrypted, duration) = sort_encrypted_u8(arr_encrypted, &client_key);

    // Decrypt
    let result_decrypted: Vec<u8> = result_encrypted
        .iter()
        .map(|val| val.decrypt(&client_key))
        .collect();

    // Verify correctness
    let expected = sort_plaintext(arr_plain);
    let mut correct = true;
    for i in 0..n {
        if result_decrypted[i] as u16 != expected[i] {
            correct = false;
            break;
        }
    }

    Ok((duration, correct))
}

/// Benchmark sorting with 6-bit encryption
fn benchmark_sort_u6(n: usize) -> Result<(Duration, bool), String> {
    let config = get_secure_config();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let arr_plain = create_sample_array(n);

    // Encrypt (clamp to 6-bit)
    let arr_encrypted: Vec<FheUint6> = arr_plain
        .iter()
        .map(|&val| {
            let clamped = if val > 63 { 63 } else { val };
            FheUint6::try_encrypt(clamped as u8, &client_key).unwrap()
        })
        .collect();

    // Sort
    let (result_encrypted, duration) = sort_encrypted_u6(arr_encrypted, &client_key);

    // Decrypt
    let result_decrypted: Vec<u8> = result_encrypted
        .iter()
        .map(|val| val.decrypt(&client_key))
        .collect();

    // Verify (with 6-bit clamping)
    let mut expected = arr_plain.clone();
    for val in &mut expected {
        if *val > 63 {
            *val = 63;
        }
    }
    expected.sort();

    let mut correct = true;
    for i in 0..n {
        if result_decrypted[i] as u16 != expected[i] {
            correct = false;
            break;
        }
    }

    Ok((duration, correct))
}

/// Benchmark sorting with 12-bit encryption
fn benchmark_sort_u12(n: usize) -> Result<(Duration, bool), String> {
    let config = get_secure_config();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let arr_plain = create_sample_array(n);

    // Encrypt (clamp to 12-bit)
    let arr_encrypted: Vec<FheUint12> = arr_plain
        .iter()
        .map(|&val| {
            let clamped = if val > 4095 { 4095 } else { val };
            FheUint12::try_encrypt(clamped, &client_key).unwrap()
        })
        .collect();

    // Sort
    let (result_encrypted, duration) = sort_encrypted_u12(arr_encrypted, &client_key);

    // Decrypt
    let result_decrypted: Vec<u16> = result_encrypted
        .iter()
        .map(|val| val.decrypt(&client_key))
        .collect();

    // Verify (with 12-bit clamping)
    let mut expected = arr_plain.clone();
    for val in &mut expected {
        if *val > 4095 {
            *val = 4095;
        }
    }
    expected.sort();

    let mut correct = true;
    for i in 0..n {
        if result_decrypted[i] != expected[i] {
            correct = false;
            break;
        }
    }

    Ok((duration, correct))
}

/// Benchmark sorting with 16-bit encryption
fn benchmark_sort_u16(n: usize) -> Result<(Duration, bool), String> {
    let config = get_secure_config();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let arr_plain = create_sample_array(n);

    // Encrypt
    let arr_encrypted: Vec<FheUint16> = arr_plain
        .iter()
        .map(|&val| FheUint16::try_encrypt(val, &client_key).unwrap())
        .collect();

    // Sort
    let (result_encrypted, duration) = sort_encrypted_u16(arr_encrypted, &client_key);

    // Decrypt
    let result_decrypted: Vec<u16> = result_encrypted
        .iter()
        .map(|val| val.decrypt(&client_key))
        .collect();

    // Verify
    let expected = sort_plaintext(arr_plain);
    let mut correct = true;
    for i in 0..n {
        if result_decrypted[i] != expected[i] {
            correct = false;
            break;
        }
    }

    Ok((duration, correct))
}

/// Format duration for display
fn format_duration(duration: Duration) -> String {
    let total_secs = duration.as_secs_f64();

    if total_secs < 1.0 {
        format!("{:.0} ms", total_secs * 1000.0)
    } else if total_secs < 60.0 {
        format!("{:.1} s", total_secs)
    } else if total_secs < 3600.0 {
        format!("{:.1} min", total_secs / 60.0)
    } else if total_secs < 86400.0 {
        format!("{:.1} hr", total_secs / 3600.0)
    } else {
        format!("{:.1} days", total_secs / 86400.0)
    }
}
