use tfhe::{generate_keys, set_server_key};
use std::time::Duration;
use crate::real_db_query::*;
use crate::security_params::get_secure_config;

/// Run comprehensive private database benchmarks following the paper's experiments
pub fn run_real_db_benchmarks() {
    println!("\n{}", "=".repeat(80));
    println!("REAL Private Database Query Evaluation Benchmark");
    println!("{}\n", "=".repeat(80));

    println!("This benchmark evaluates Query 1 on encrypted database records.");
    println!("Query: SELECT ID FROM emp");
    println!("       WHERE salary * work_hours BETWEEN 5000 AND 6000");
    println!("       AND salary + bonus BETWEEN 700 AND 800");
    println!();
    println!("All operations use >= 128-bit secure TFHE parameters.\n");

    // Following the paper's experiments: 64, 128, 256, 512 rows with 8-bit precision
    let row_counts = vec![64, 128, 256, 512];
    let bit_width = 8;

    println!("{:<10} {:<12} {:<15} {:<15} {:<10}",
             "Rows", "Bit Width", "Time", "Matches", "Status");
    println!("{}", "-".repeat(70));

    for &num_rows in &row_counts {
        print!("{:<10} {:<12} ", num_rows, bit_width);

        let result = benchmark_query_u8(num_rows);

        match result {
            Ok((duration, num_matches, correct)) => {
                let status = if correct { "✓" } else { "✗ Wrong" };
                println!("{:<15} {:<15} {:<10}",
                         format_duration(duration),
                         num_matches,
                         status);
            }
            Err(e) => {
                println!("{:<15} {:<15} {:<10}",
                         "-",
                         "-",
                         format!("✗ {}", e));
            }
        }
    }

    println!();
    println!("{}", "=".repeat(80));
}

/// Benchmark Query 1 with 8-bit encryption
fn benchmark_query_u8(num_rows: usize) -> Result<(Duration, usize, bool), String> {
    let config = get_secure_config();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    // Generate database
    let records = generate_sample_database(num_rows);

    // Execute encrypted query
    let (encrypted_result, duration) = execute_query1_encrypted_u8(&records, &client_key);

    // Verify correctness
    let plaintext_result = execute_query1_plaintext(&records);

    let correct = encrypted_result == plaintext_result;
    let num_matches = encrypted_result.len();

    Ok((duration, num_matches, correct))
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
