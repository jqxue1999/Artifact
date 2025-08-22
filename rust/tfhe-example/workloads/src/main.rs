mod benchmarks;

use benchmarks::*;
fn main() {
    println!("=== TFHE Homomorphic Encryption Benchmark ===\n");
    
    // Workload-1: (a*b) compare c
    println!("Workload-1: (a*b) compare c");
    
    match fhe_mul_cmp_6bit(3, 4, 15) {
        Ok((_, duration)) => println!("6-bit: {:.2?}", duration),
        Err(_) => println!("6-bit: Error"),
    }

    match fhe_mul_cmp_8bit(15, 16, 250) {
        Ok((_, duration)) => println!("8-bit: {:.2?}", duration),
        Err(_) => println!("8-bit: Error"),
    }

    match fhe_mul_cmp_16bit(100, 200, 25000) {
        Ok((_, duration)) => println!("16-bit: {:.2?}", duration),
        Err(_) => println!("16-bit: Error"),
    }
    println!();

    // Workload-2: (a compare b) * c
    println!("Workload-2: (a compare b) * c");
    
    match fhe_cmp_mul_6bit(5, 8, 10) {
        Ok((_, duration)) => println!("6-bit: {:.2?}", duration),
        Err(_) => println!("6-bit: Error"),
    }

    match fhe_cmp_mul_8bit(20, 15, 25) {
        Ok((_, duration)) => println!("8-bit: {:.2?}", duration),
        Err(_) => println!("8-bit: Error"),
    }

    match fhe_cmp_mul_16bit(100, 200, 500) {
        Ok((_, duration)) => println!("16-bit: {:.2?}", duration),
        Err(_) => println!("16-bit: Error"),
    }
    println!();

    // Workload-3: (a*b) compare (c*d)
    println!("Workload-3: (a*b) compare (c*d)");
    
    match fhe_mul_cmp_mul_6bit(3, 4, 2, 6) {
        Ok((_, duration)) => println!("6-bit: {:.2?}", duration),
        Err(_) => println!("6-bit: Error"),
    }

    match fhe_mul_cmp_mul_8bit(15, 16, 12, 20) {
        Ok((_, duration)) => println!("8-bit: {:.2?}", duration),
        Err(_) => println!("8-bit: Error"),
    }

    match fhe_mul_cmp_mul_16bit(100, 200, 150, 130) {
        Ok((_, duration)) => println!("16-bit: {:.2?}", duration),
        Err(_) => println!("16-bit: Error"),
    }
}
