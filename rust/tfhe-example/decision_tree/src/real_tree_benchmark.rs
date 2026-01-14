use tfhe::prelude::*;
use tfhe::{generate_keys, set_server_key, FheUint6, FheUint8, FheUint12, FheUint16};
use std::time::Duration;
use crate::real_tree::*;
use crate::security_params::get_secure_config;

/// Run comprehensive decision tree benchmarks following the paper's experiments
pub fn run_real_tree_benchmarks() {
    println!("\n{}", "=".repeat(80));
    println!("REAL Decision Tree Evaluation Benchmark");
    println!("{}\n", "=".repeat(80));

    println!("This benchmark evaluates actual decision tree classification on encrypted features.");
    println!("All comparisons and path selections are performed obliviously.\n");

    let tree = create_sample_tree();
    let depth = tree_depth(&tree);

    println!("Tree Configuration:");
    println!("  - Depth: {}", depth);
    println!("  - Features: 2");
    println!("  - Classes: 2");
    println!();

    println!("Experiment 1: Different bit widths (2 features)");
    println!("{}", "-".repeat(80));
    println!("{:<12} {:<10} {:<15} {:<15} {:<10}",
             "Bit Width", "Depth", "Time", "Comparisons", "Status");
    println!("{}", "-".repeat(80));

    let bit_widths = vec![6, 8, 12, 16];
    let num_features = 2;

    for &bit_width in &bit_widths {
        // Number of comparisons equals depth of tree
        let comparisons = depth;

        print!("{:<12} {:<10} ", bit_width, depth);

        let result = match bit_width {
            6 => benchmark_tree_u6(&tree, num_features),
            8 => benchmark_tree_u8(&tree, num_features),
            12 => benchmark_tree_u12(&tree, num_features),
            16 => benchmark_tree_u16(&tree, num_features),
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
    println!("Experiment 2: Different tree depths (8-bit)");
    println!("{}", "-".repeat(80));
    println!("{:<12} {:<10} {:<15} {:<15} {:<10}",
             "Bit Width", "Depth", "Time", "Comparisons", "Status");
    println!("{}", "-".repeat(80));

    let bit_width = 8;
    let depths = vec![1, 2, 3, 4];

    for &d in &depths {
        let test_tree = create_tree_with_depth(d);
        let comparisons = tree_depth(&test_tree);

        print!("{:<12} {:<10} ", bit_width, d);

        let result = benchmark_tree_u8(&test_tree, num_features);

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

/// Benchmark tree evaluation with 8-bit encryption
fn benchmark_tree_u8(tree: &TreeNode, num_features: usize) -> Result<(Duration, bool), String> {
    let config = get_secure_config();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    // Create sample features: [20, 10]
    let features_plain = vec![20u16, 10];

    // Encrypt
    let features_encrypted: Vec<FheUint8> = features_plain
        .iter()
        .map(|&val| FheUint8::try_encrypt(val as u8, &client_key).unwrap())
        .collect();

    // Evaluate
    let (result_encrypted, duration) = evaluate_tree_u8(&features_encrypted, tree, &client_key);

    // Decrypt
    let result_decrypted: u8 = result_encrypted.decrypt(&client_key);

    // Verify correctness
    let expected = evaluate_tree_plain(&features_plain, tree);
    let correct = result_decrypted == expected;

    Ok((duration, correct))
}

/// Benchmark tree evaluation with 6-bit encryption
fn benchmark_tree_u6(tree: &TreeNode, num_features: usize) -> Result<(Duration, bool), String> {
    let config = get_secure_config();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let features_plain = vec![20u16, 10];

    // Encrypt (clamp to 6-bit)
    let features_encrypted: Vec<FheUint6> = features_plain
        .iter()
        .map(|&val| {
            let clamped = if val > 63 { 63 } else { val };
            FheUint6::try_encrypt(clamped as u8, &client_key).unwrap()
        })
        .collect();

    let (result_encrypted, duration) = evaluate_tree_u6(&features_encrypted, tree, &client_key);

    let result_decrypted: u8 = result_encrypted.decrypt(&client_key);
    let expected = evaluate_tree_plain(&features_plain, tree);
    let correct = result_decrypted == expected;

    Ok((duration, correct))
}

/// Benchmark tree evaluation with 12-bit encryption
fn benchmark_tree_u12(tree: &TreeNode, num_features: usize) -> Result<(Duration, bool), String> {
    let config = get_secure_config();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let features_plain = vec![20u16, 10];

    // Encrypt
    let features_encrypted: Vec<FheUint12> = features_plain
        .iter()
        .map(|&val| FheUint12::try_encrypt(val, &client_key).unwrap())
        .collect();

    let (result_encrypted, duration) = evaluate_tree_u12(&features_encrypted, tree, &client_key);

    let result_decrypted: u16 = result_encrypted.decrypt(&client_key);
    let expected = evaluate_tree_plain(&features_plain, tree);
    let correct = result_decrypted == expected as u16;

    Ok((duration, correct))
}

/// Benchmark tree evaluation with 16-bit encryption
fn benchmark_tree_u16(tree: &TreeNode, num_features: usize) -> Result<(Duration, bool), String> {
    let config = get_secure_config();
    let (client_key, server_keys) = generate_keys(config);
    set_server_key(server_keys);

    let features_plain = vec![20u16, 10];

    // Encrypt
    let features_encrypted: Vec<FheUint16> = features_plain
        .iter()
        .map(|&val| FheUint16::try_encrypt(val, &client_key).unwrap())
        .collect();

    let (result_encrypted, duration) = evaluate_tree_u16(&features_encrypted, tree, &client_key);

    let result_decrypted: u16 = result_encrypted.decrypt(&client_key);
    let expected = evaluate_tree_plain(&features_plain, tree);
    let correct = result_decrypted == expected as u16;

    Ok((duration, correct))
}

/// Create a tree with specified depth for testing
fn create_tree_with_depth(depth: usize) -> TreeNode {
    if depth == 1 {
        // Leaf node
        TreeNode {
            feature_idx: 0,
            threshold: 50,
            left_class: 0,
            right_class: 1,
            is_leaf_left: true,
            is_leaf_right: true,
            left_child: None,
            right_child: None,
        }
    } else {
        // Internal node with children
        TreeNode {
            feature_idx: 0,
            threshold: 50,
            left_class: 0,
            right_class: 0,
            is_leaf_left: false,
            is_leaf_right: false,
            left_child: Some(Box::new(create_tree_with_depth(depth - 1))),
            right_child: Some(Box::new(create_tree_with_depth(depth - 1))),
        }
    }
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
