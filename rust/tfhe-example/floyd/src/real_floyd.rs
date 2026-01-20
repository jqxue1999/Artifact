use tfhe::prelude::*;
use tfhe::{ClientKey, FheUint6, FheUint8, FheUint12, FheUint16};
use std::time::Instant;
use std::io::{self, Write};

/// Floyd-Warshall algorithm on encrypted 8-bit distance matrix
///
/// **Security**: Uses TFHE parameters providing >= 128-bit security
/// (both classical and post-quantum).
///
/// The algorithm computes all-pairs shortest paths on an encrypted graph.
/// All operations are performed homomorphically without decryption.
pub fn floyd_warshall_encrypted_u8(
    mut dist: Vec<Vec<FheUint8>>,
    n: usize,
    client_key: &ClientKey,
) -> (Vec<Vec<FheUint8>>, std::time::Duration) {
    let start = Instant::now();
    let total_iterations = n * n * n;
    let mut completed = 0;
    let report_interval = if n <= 32 { 100 } else { 1000 };

    // Pre-encrypt the constant "1" outside the loop for efficiency
    let one = FheUint8::try_encrypt(1u8, client_key).unwrap();

    // Floyd-Warshall: for each intermediate vertex k
    for k in 0..n {
        // For each source vertex i
        for i in 0..n {
            // For each destination vertex j
            for j in 0..n {
                // Compute potential new distance: dist[i][k] + dist[k][j]
                let new_dist = &dist[i][k] + &dist[k][j];

                // Compare: is new_dist < dist[i][j]?
                let is_shorter = new_dist.lt(&dist[i][j]);
                let is_shorter_uint: FheUint8 = is_shorter.cast_into();

                // Oblivious selection:
                // result = is_shorter * new_dist + (1 - is_shorter) * old_dist
                let keep_old = &one - &is_shorter_uint;

                let select_new = &is_shorter_uint * &new_dist;
                let select_old = &keep_old * &dist[i][j];

                dist[i][j] = &select_new + &select_old;

                completed += 1;
                if completed % report_interval == 0 {
                    let elapsed = start.elapsed().as_secs_f64();
                    let percent = (completed as f64 / total_iterations as f64) * 100.0;
                    let avg_per_iter = elapsed / completed as f64;
                    let remaining = ((total_iterations - completed) as f64 * avg_per_iter) / 60.0;
                    println!("  Progress: {}/{} ({:.1}%) | Elapsed: {:.1}min | Est. remaining: {:.1}min",
                           completed, total_iterations, percent, elapsed / 60.0, remaining);
                    io::stdout().flush().unwrap();
                }
            }
        }
    }
    print!("\r");
    io::stdout().flush().unwrap();

    let duration = start.elapsed();
    (dist, duration)
}

/// Floyd-Warshall algorithm on encrypted 6-bit distance matrix
pub fn floyd_warshall_encrypted_u6(
    mut dist: Vec<Vec<FheUint6>>,
    n: usize,
    client_key: &ClientKey,
) -> (Vec<Vec<FheUint6>>, std::time::Duration) {
    let start = Instant::now();
    let total_iterations = n * n * n;
    let mut completed = 0;
    let report_interval = if n <= 32 { 100 } else { 1000 };

    // Pre-encrypt the constant "1" outside the loop for efficiency
    let one = FheUint6::try_encrypt(1u8, client_key).unwrap();

    for k in 0..n {
        for i in 0..n {
            for j in 0..n {
                let new_dist = &dist[i][k] + &dist[k][j];
                let is_shorter = new_dist.lt(&dist[i][j]);
                let is_shorter_uint: FheUint6 = is_shorter.cast_into();

                let keep_old = &one - &is_shorter_uint;

                let select_new = &is_shorter_uint * &new_dist;
                let select_old = &keep_old * &dist[i][j];

                dist[i][j] = &select_new + &select_old;

                completed += 1;
                if completed % report_interval == 0 {
                    let elapsed = start.elapsed().as_secs_f64();
                    let percent = (completed as f64 / total_iterations as f64) * 100.0;
                    let avg_per_iter = elapsed / completed as f64;
                    let remaining = ((total_iterations - completed) as f64 * avg_per_iter) / 60.0;
                    println!("  Progress: {}/{} ({:.1}%) | Elapsed: {:.1}min | Est. remaining: {:.1}min",
                           completed, total_iterations, percent, elapsed / 60.0, remaining);
                    io::stdout().flush().unwrap();
                }
            }
        }
    }

    let duration = start.elapsed();
    (dist, duration)
}

/// Floyd-Warshall algorithm on encrypted 12-bit distance matrix
pub fn floyd_warshall_encrypted_u12(
    mut dist: Vec<Vec<FheUint12>>,
    n: usize,
    client_key: &ClientKey,
) -> (Vec<Vec<FheUint12>>, std::time::Duration) {
    let start = Instant::now();
    let total_iterations = n * n * n;
    let mut completed = 0;
    let report_interval = if n <= 32 { 100 } else { 1000 };

    // Pre-encrypt the constant "1" outside the loop for efficiency
    let one = FheUint12::try_encrypt(1u16, client_key).unwrap();

    for k in 0..n {
        for i in 0..n {
            for j in 0..n {
                let new_dist = &dist[i][k] + &dist[k][j];
                let is_shorter = new_dist.lt(&dist[i][j]);
                let is_shorter_uint: FheUint12 = is_shorter.cast_into();

                let keep_old = &one - &is_shorter_uint;

                let select_new = &is_shorter_uint * &new_dist;
                let select_old = &keep_old * &dist[i][j];

                dist[i][j] = &select_new + &select_old;

                completed += 1;
                if completed % report_interval == 0 {
                    let elapsed = start.elapsed().as_secs_f64();
                    let percent = (completed as f64 / total_iterations as f64) * 100.0;
                    let avg_per_iter = elapsed / completed as f64;
                    let remaining = ((total_iterations - completed) as f64 * avg_per_iter) / 60.0;
                    println!("  Progress: {}/{} ({:.1}%) | Elapsed: {:.1}min | Est. remaining: {:.1}min",
                           completed, total_iterations, percent, elapsed / 60.0, remaining);
                    io::stdout().flush().unwrap();
                }
            }
        }
    }
    print!("\r");
    io::stdout().flush().unwrap();

    let duration = start.elapsed();
    (dist, duration)
}

/// Floyd-Warshall algorithm on encrypted 16-bit distance matrix
pub fn floyd_warshall_encrypted_u16(
    mut dist: Vec<Vec<FheUint16>>,
    n: usize,
    client_key: &ClientKey,
) -> (Vec<Vec<FheUint16>>, std::time::Duration) {
    let start = Instant::now();
    let total_iterations = n * n * n;
    let mut completed = 0;
    let report_interval = if n <= 32 { 100 } else { 1000 };

    // Pre-encrypt the constant "1" outside the loop for efficiency
    let one = FheUint16::try_encrypt(1u16, client_key).unwrap();

    for k in 0..n {
        for i in 0..n {
            for j in 0..n {
                let new_dist = &dist[i][k] + &dist[k][j];
                let is_shorter = new_dist.lt(&dist[i][j]);
                let is_shorter_uint: FheUint16 = is_shorter.cast_into();

                let keep_old = &one - &is_shorter_uint;

                let select_new = &is_shorter_uint * &new_dist;
                let select_old = &keep_old * &dist[i][j];

                dist[i][j] = &select_new + &select_old;

                completed += 1;
                if completed % report_interval == 0 {
                    let elapsed = start.elapsed().as_secs_f64();
                    let percent = (completed as f64 / total_iterations as f64) * 100.0;
                    let avg_per_iter = elapsed / completed as f64;
                    let remaining = ((total_iterations - completed) as f64 * avg_per_iter) / 60.0;
                    println!("  Progress: {}/{} ({:.1}%) | Elapsed: {:.1}min | Est. remaining: {:.1}min",
                           completed, total_iterations, percent, elapsed / 60.0, remaining);
                    io::stdout().flush().unwrap();
                }
            }
        }
    }
    print!("\r");
    io::stdout().flush().unwrap();

    let duration = start.elapsed();
    (dist, duration)
}

/// Floyd-Warshall on plaintext for testing correctness
pub fn floyd_warshall_plaintext(mut dist: Vec<Vec<u16>>, n: usize) -> Vec<Vec<u16>> {
    for k in 0..n {
        for i in 0..n {
            for j in 0..n {
                let new_dist = dist[i][k].saturating_add(dist[k][j]);
                if new_dist < dist[i][j] {
                    dist[i][j] = new_dist;
                }
            }
        }
    }
    dist
}

/// Create a sample distance matrix for testing
/// Uses infinity (255 for u8, 63 for u6) for no edge
pub fn create_sample_graph(n: usize) -> Vec<Vec<u16>> {
    let inf = 99; // Use 99 as "infinity" to avoid overflow
    let mut dist = vec![vec![inf; n]; n];

    // Distance from vertex to itself is 0
    for i in 0..n {
        dist[i][i] = 0;
    }

    // Add some sample edges (simple path graph with some shortcuts)
    for i in 0..n-1 {
        dist[i][i+1] = 1; // Edge to next vertex
        dist[i+1][i] = 1; // Bidirectional
    }

    // Add some diagonal shortcuts
    if n >= 4 {
        dist[0][2] = 3; // Shortcut
        dist[2][0] = 3;
    }
    if n >= 6 {
        dist[1][4] = 5;
        dist[4][1] = 5;
    }

    dist
}

#[cfg(test)]
mod tests {
    use super::*;
    use tfhe::{generate_keys, set_server_key};
    use crate::security_params::get_secure_config;

    #[test]
    fn test_plaintext_floyd() {
        let n = 4;
        let dist = create_sample_graph(n);

        println!("Initial distance matrix:");
        for row in &dist {
            println!("{:?}", row);
        }

        let result = floyd_warshall_plaintext(dist, n);

        println!("After Floyd-Warshall:");
        for row in &result {
            println!("{:?}", row);
        }

        // Verify some properties
        assert_eq!(result[0][0], 0); // Distance to self
        assert!(result[0][3] <= 3); // Should find shortest path
    }

    #[test]
    fn test_encrypted_floyd_small() {
        let config = get_secure_config();
        let (client_key, server_keys) = generate_keys(config);
        set_server_key(server_keys);

        let n = 3;
        let dist_plain = create_sample_graph(n);

        // Encrypt the distance matrix
        let dist_encrypted: Vec<Vec<FheUint8>> = dist_plain
            .iter()
            .map(|row| {
                row.iter()
                    .map(|&val| FheUint8::try_encrypt(val as u8, &client_key).unwrap())
                    .collect()
            })
            .collect();

        println!("Running encrypted Floyd-Warshall on {}x{} graph...", n, n);
        let (result_encrypted, duration) = floyd_warshall_encrypted_u8(dist_encrypted, n, &client_key);

        // Decrypt result
        let result_decrypted: Vec<Vec<u8>> = result_encrypted
            .iter()
            .map(|row| {
                row.iter()
                    .map(|val| val.decrypt(&client_key))
                    .collect()
            })
            .collect();

        println!("Decrypted result:");
        for row in &result_decrypted {
            println!("{:?}", row);
        }

        println!("Encrypted computation took: {:?}", duration);

        // Verify correctness by comparing with plaintext
        let expected = floyd_warshall_plaintext(dist_plain, n);
        for i in 0..n {
            for j in 0..n {
                assert_eq!(
                    result_decrypted[i][j] as u16,
                    expected[i][j],
                    "Mismatch at [{}, {}]", i, j
                );
            }
        }
    }
}
