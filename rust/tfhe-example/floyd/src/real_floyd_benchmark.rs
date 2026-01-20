use tfhe::prelude::*;
use tfhe::{generate_keys, set_server_key, FheUint6, FheUint8, FheUint12, FheUint16};
use std::time::Duration;
use std::io::{self, Write};
use crate::real_floyd::*;
use crate::security_params::get_secure_config;

/// Run comprehensive Floyd-Warshall benchmarks following the paper's experiments
pub fn run_real_floyd_benchmarks() {
    println!("\n{}", "=".repeat(80));
    println!("REAL Floyd-Warshall Evaluation Benchmark");
    println!("{}\n", "=".repeat(80));

    println!("This benchmark evaluates actual Floyd-Warshall algorithm on encrypted graphs.");
    println!("All shortest paths are computed obliviously using encrypted operations.\n");

    println!("Experiment 1: 32-node graph with different bit widths");
    println!("{}", "-".repeat(80));
    println!("{:<10} {:<12} {:<15} {:<15} {:<10}",
             "Nodes", "Bit Width", "Time", "Iterations", "Status");
    println!("{}", "-".repeat(80));

    let n = 32;
    let bit_widths = vec![6, 8, 12, 16];

    for &bit_width in &bit_widths {
        let iterations = n * n * n;

        print!("{:<10} {:<12} ", n, bit_width);
        io::stdout().flush().unwrap();

        let result = match bit_width {
            6 => benchmark_floyd_u6(n),
            8 => benchmark_floyd_u8(n),
            12 => benchmark_floyd_u12(n),
            16 => benchmark_floyd_u16(n),
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
                         iterations,
                         status);
                io::stdout().flush().unwrap();
            }
            Err(e) => {
                println!("{:<15} {:<15} {:<10}",
                         "-",
                         iterations,
                         format!("✗ {}", e));
                io::stdout().flush().unwrap();
            }
        }
    }

    println!();
    println!("Experiment 2: 8-bit inputs with different graph sizes");
    println!("{}", "-".repeat(80));
    println!("{:<10} {:<12} {:<15} {:<15} {:<10}",
             "Nodes", "Bit Width", "Time", "Iterations", "Status");
    println!("{}", "-".repeat(80));

    let node_counts = vec![16, 32, 64, 128];
    let bit_width = 8;

    for &n in &node_counts {
        let iterations = n * n * n;

        print!("{:<10} {:<12} ", n, bit_width);
        io::stdout().flush().unwrap();

        let result = benchmark_floyd_u8(n);

        match result {
            Ok((duration, correct)) => {
                let status = if correct { "✓" } else { "✗ Wrong" };
                println!("{:<15} {:<15} {:<10}",
                         format_duration(duration),
                         iterations,
                         status);
                io::stdout().flush().unwrap();
            }
            Err(e) => {
                println!("{:<15} {:<15} {:<10}",
                         "-",
                         iterations,
                         format!("✗ {}", e));
                io::stdout().flush().unwrap();
            }
        }
    }

    println!();
    println!("{}", "=".repeat(80));
}

/// Benchmark Floyd-Warshall with 8-bit encryption
fn benchmark_floyd_u8(n: usize) -> Result<(Duration, bool), String> {
    let config = get_secure_config();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let dist_plain = create_sample_graph(n);

    // Encrypt
    let dist_encrypted: Vec<Vec<FheUint8>> = dist_plain
        .iter()
        .map(|row| {
            row.iter()
                .map(|&val| FheUint8::try_encrypt(val as u8, &client_key).unwrap())
                .collect()
        })
        .collect();

    // Compute
    let (result_encrypted, duration) = floyd_warshall_encrypted_u8(dist_encrypted, n, &client_key);

    // Decrypt
    let result_decrypted: Vec<Vec<u8>> = result_encrypted
        .iter()
        .map(|row| {
            row.iter()
                .map(|val| val.decrypt(&client_key))
                .collect()
        })
        .collect();

    // Verify correctness
    let expected = floyd_warshall_plaintext(dist_plain, n);
    let mut correct = true;
    for i in 0..n {
        for j in 0..n {
            if result_decrypted[i][j] as u16 != expected[i][j] {
                correct = false;
                break;
            }
        }
        if !correct {
            break;
        }
    }

    Ok((duration, correct))
}

/// Benchmark Floyd-Warshall with 6-bit encryption
fn benchmark_floyd_u6(n: usize) -> Result<(Duration, bool), String> {
    let config = get_secure_config();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let dist_plain = create_sample_graph(n);

    // Encrypt (clamp to 6-bit)
    let dist_encrypted: Vec<Vec<FheUint6>> = dist_plain
        .iter()
        .map(|row| {
            row.iter()
                .map(|&val| {
                    let clamped = if val > 63 { 63 } else { val };
                    FheUint6::try_encrypt(clamped as u8, &client_key).unwrap()
                })
                .collect()
        })
        .collect();

    // Compute
    let (result_encrypted, duration) = floyd_warshall_encrypted_u6(dist_encrypted, n, &client_key);

    // Decrypt
    let result_decrypted: Vec<Vec<u8>> = result_encrypted
        .iter()
        .map(|row| {
            row.iter()
                .map(|val| val.decrypt(&client_key))
                .collect()
        })
        .collect();

    // Verify (with 6-bit clamping)
    let expected = floyd_warshall_plaintext(dist_plain, n);
    let mut correct = true;
    for i in 0..n {
        for j in 0..n {
            let expected_val = if expected[i][j] > 63 { 63 } else { expected[i][j] as u8 };
            if result_decrypted[i][j] != expected_val {
                correct = false;
                break;
            }
        }
        if !correct {
            break;
        }
    }

    Ok((duration, correct))
}

/// Benchmark Floyd-Warshall with 12-bit encryption
fn benchmark_floyd_u12(n: usize) -> Result<(Duration, bool), String> {
    let config = get_secure_config();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let dist_plain = create_sample_graph(n);

    // Encrypt (clamp to 12-bit)
    let dist_encrypted: Vec<Vec<FheUint12>> = dist_plain
        .iter()
        .map(|row| {
            row.iter()
                .map(|&val| {
                    let clamped = if val > 4095 { 4095 } else { val };
                    FheUint12::try_encrypt(clamped, &client_key).unwrap()
                })
                .collect()
        })
        .collect();

    // Compute
    let (result_encrypted, duration) = floyd_warshall_encrypted_u12(dist_encrypted, n, &client_key);

    // Decrypt
    let result_decrypted: Vec<Vec<u16>> = result_encrypted
        .iter()
        .map(|row| {
            row.iter()
                .map(|val| val.decrypt(&client_key))
                .collect()
        })
        .collect();

    // Verify (with 12-bit clamping)
    let expected = floyd_warshall_plaintext(dist_plain, n);
    let mut correct = true;
    for i in 0..n {
        for j in 0..n {
            let expected_val = if expected[i][j] > 4095 { 4095 } else { expected[i][j] };
            if result_decrypted[i][j] != expected_val {
                correct = false;
                break;
            }
        }
        if !correct {
            break;
        }
    }

    Ok((duration, correct))
}

/// Benchmark Floyd-Warshall with 16-bit encryption
fn benchmark_floyd_u16(n: usize) -> Result<(Duration, bool), String> {
    let config = get_secure_config();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let dist_plain = create_sample_graph(n);

    // Encrypt
    let dist_encrypted: Vec<Vec<FheUint16>> = dist_plain
        .iter()
        .map(|row| {
            row.iter()
                .map(|&val| FheUint16::try_encrypt(val, &client_key).unwrap())
                .collect()
        })
        .collect();

    // Compute
    let (result_encrypted, duration) = floyd_warshall_encrypted_u16(dist_encrypted, n, &client_key);

    // Decrypt
    let result_decrypted: Vec<Vec<u16>> = result_encrypted
        .iter()
        .map(|row| {
            row.iter()
                .map(|val| val.decrypt(&client_key))
                .collect()
        })
        .collect();

    // Verify
    let expected = floyd_warshall_plaintext(dist_plain, n);
    let mut correct = true;
    for i in 0..n {
        for j in 0..n {
            if result_decrypted[i][j] != expected[i][j] {
                correct = false;
                break;
            }
        }
        if !correct {
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
