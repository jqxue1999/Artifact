mod benchmarks;

use benchmarks::*;
use std::time::Duration;

fn main() {
    println!("=== TFHE Homomorphic Encryption Benchmark ===\n");
    println!("Running benchmarks...\n");
    
    // Store results in a table format
    let bit_sizes = ["6-bit", "8-bit", "12-bit", "16-bit"];
    let mut results: Vec<Vec<Option<Duration>>> = vec![vec![None; 4]; 3]; // 3 workloads, 4 bit sizes
    
    // Workload-1: (a*b) compare c
    println!("Workload-1: (a*b) compare c");
    
    match fhe_mul_cmp_6bit(3, 4, 15) {
        Ok((_, duration)) => {
            results[0][0] = Some(duration);
            println!("6-bit: {:.3}s", duration.as_secs_f64());
        },
        Err(_) => println!("6-bit: Error"),
    }

    match fhe_mul_cmp_8bit(15, 16, 250) {
        Ok((_, duration)) => {
            results[0][1] = Some(duration);
            println!("8-bit: {:.3}s", duration.as_secs_f64());
        },
        Err(_) => println!("8-bit: Error"),
    }

    match fhe_mul_cmp_12bit(100, 200, 25000) {
        Ok((_, duration)) => {
            results[0][2] = Some(duration);
            println!("12-bit: {:.3}s", duration.as_secs_f64());
        },
        Err(_) => println!("12-bit: Error"),
    }

    match fhe_mul_cmp_16bit(1000, 2000, 3000000) {
        Ok((_, duration)) => {
            results[0][3] = Some(duration);
            println!("16-bit: {:.3}s", duration.as_secs_f64());
        },
        Err(_) => println!("16-bit: Error"),
    }
    println!();

    // Workload-2: (a compare b) * c
    println!("Workload-2: (a compare b) * c");
    
    match fhe_cmp_mul_6bit(5, 8, 10) {
        Ok((_, duration)) => {
            results[1][0] = Some(duration);
            println!("6-bit: {:.3}s", duration.as_secs_f64());
        },
        Err(_) => println!("6-bit: Error"),
    }

    match fhe_cmp_mul_8bit(20, 15, 25) {
        Ok((_, duration)) => {
            results[1][1] = Some(duration);
            println!("8-bit: {:.3}s", duration.as_secs_f64());
        },
        Err(_) => println!("8-bit: Error"),
    }

    match fhe_cmp_mul_12bit(100, 200, 500) {
        Ok((_, duration)) => {
            results[1][2] = Some(duration);
            println!("12-bit: {:.3}s", duration.as_secs_f64());
        },
        Err(_) => println!("12-bit: Error"),
    }

    match fhe_cmp_mul_16bit(1000, 2000, 1500) {
        Ok((_, duration)) => {
            results[1][3] = Some(duration);
            println!("16-bit: {:.3}s", duration.as_secs_f64());
        },
        Err(_) => println!("16-bit: Error"),
    }
    println!();

    // Workload-3: (a*b) compare (c*d)
    println!("Workload-3: (a*b) compare (c*d)");
    
    match fhe_mul_cmp_mul_6bit(3, 4, 2, 6) {
        Ok((_, duration)) => {
            results[2][0] = Some(duration);
            println!("6-bit: {:.3}s", duration.as_secs_f64());
        },
        Err(_) => println!("6-bit: Error"),
    }

    match fhe_mul_cmp_mul_8bit(15, 16, 12, 20) {
        Ok((_, duration)) => {
            results[2][1] = Some(duration);
            println!("8-bit: {:.3}s", duration.as_secs_f64());
        },
        Err(_) => println!("8-bit: Error"),
    }

    match fhe_mul_cmp_mul_12bit(100, 200, 150, 130) {
        Ok((_, duration)) => {
            results[2][2] = Some(duration);
            println!("12-bit: {:.3}s", duration.as_secs_f64());
        },
        Err(_) => println!("12-bit: Error"),
    }

    match fhe_mul_cmp_mul_16bit(1000, 2000, 1500, 1300) {
        Ok((_, duration)) => {
            results[2][3] = Some(duration);
            println!("16-bit: {:.3}s", duration.as_secs_f64());
        },
        Err(_) => println!("16-bit: Error"),
    }
    
    // Print results table
    println!("\n{}", "=".repeat(80));
    println!("TFHE BENCHMARK RESULTS TABLE");
    println!("{}", "=".repeat(80));
    
    // Header
    print!("{:15}", "");
    for bit_size in &bit_sizes {
        print!("{:>12}", bit_size);
    }
    println!();
    
    println!("{:15}{}", "", "-".repeat(48));
    
    // Results
    let workload_names = ["workload-1", "workload-2", "workload-3"];
    for i in 0..3 {
        print!("{:15}", workload_names[i]);
        for j in 0..4 {
            match results[i][j] {
                Some(duration) => print!("{:>9.3} s", duration.as_secs_f64()),
                None => print!("{:>12}", "Error"),
            }
        }
        println!();
    }
    println!("{}", "=".repeat(80));
}
