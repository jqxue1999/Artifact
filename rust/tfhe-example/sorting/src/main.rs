use tfhe_sorting::*;
use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();

    println!("\n{}", "=".repeat(80));
    println!("TFHE Private Sorting");
    println!("{}\n", "=".repeat(80));

    // Check command line arguments
    let mode = if args.len() > 1 {
        args[1].as_str()
    } else {
        "real"  // Default to real implementation
    };

    match mode {
        "real" => {
            println!("Mode: REAL IMPLEMENTATION (>=128-bit security)");
            println!("Running actual private sorting on encrypted data\n");

            // Display security verification first
            verify_and_print_security();

            run_real_sorting_benchmarks();
        }
        "security" => {
            verify_and_print_security();
            println!();
            print_detailed_security_info();
        }
        "help" | "-h" | "--help" => {
            print_help();
        }
        _ => {
            println!("Unknown mode: {}", mode);
            println!("Run with --help for usage information\n");
            print_help();
        }
    }
}

fn print_help() {
    println!("Usage: cargo run --release [MODE]\n");
    println!("Modes:");
    println!("  real      - Run real private sorting (default)");
    println!("  security  - Display detailed security parameter information");
    println!("  help      - Show this help message");
    println!("\nExamples:");
    println!("  cargo run --release                  # Run with security verification");
    println!("  cargo run --release security         # Show detailed security info");
    println!("  cargo test --release -- --nocapture  # Run tests with security output");
    println!("\nNote: Always use --release flag for accurate benchmarks!");
    println!("\nSecurity: All operations use >= 128-bit secure TFHE parameters");
}
