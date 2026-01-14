use tfhe::prelude::*;
use tfhe::{ClientKey, FheUint6, FheUint8, FheUint12, FheUint16};
use std::time::Instant;

/// Private sorting on encrypted 8-bit array
///
/// **Security**: Uses TFHE parameters providing >= 128-bit security
/// (both classical and post-quantum).
///
/// Uses comparison-based sorting with oblivious swaps to ensure
/// data-independent execution time.
pub fn sort_encrypted_u8(
    mut arr: Vec<FheUint8>,
    client_key: &ClientKey,
) -> (Vec<FheUint8>, std::time::Duration) {
    let start = Instant::now();
    let n = arr.len();

    // Bubble sort with oblivious swaps
    for _ in 0..n {
        for j in 0..n-1 {
            // Compare arr[j] and arr[j+1]
            let should_swap = arr[j].gt(&arr[j+1]);
            let should_swap_uint: FheUint8 = should_swap.cast_into();

            // Oblivious swap:
            // If should_swap: arr[j] gets arr[j+1], arr[j+1] gets arr[j]
            // Otherwise: no change
            let one = FheUint8::try_encrypt(1u8, client_key).unwrap();
            let keep_same = &one - &should_swap_uint;

            // Calculate new values
            let j_new = &should_swap_uint * &arr[j+1] + &keep_same * &arr[j];
            let j1_new = &should_swap_uint * &arr[j] + &keep_same * &arr[j+1];

            arr[j] = j_new;
            arr[j+1] = j1_new;
        }
    }

    let duration = start.elapsed();
    (arr, duration)
}

/// Private sorting on encrypted 6-bit array
pub fn sort_encrypted_u6(
    mut arr: Vec<FheUint6>,
    client_key: &ClientKey,
) -> (Vec<FheUint6>, std::time::Duration) {
    let start = Instant::now();
    let n = arr.len();

    for _ in 0..n {
        for j in 0..n-1 {
            let should_swap = arr[j].gt(&arr[j+1]);
            let should_swap_uint: FheUint6 = should_swap.cast_into();

            let one = FheUint6::try_encrypt(1u8, client_key).unwrap();
            let keep_same = &one - &should_swap_uint;

            let j_new = &should_swap_uint * &arr[j+1] + &keep_same * &arr[j];
            let j1_new = &should_swap_uint * &arr[j] + &keep_same * &arr[j+1];

            arr[j] = j_new;
            arr[j+1] = j1_new;
        }
    }

    let duration = start.elapsed();
    (arr, duration)
}

/// Private sorting on encrypted 12-bit array
pub fn sort_encrypted_u12(
    mut arr: Vec<FheUint12>,
    client_key: &ClientKey,
) -> (Vec<FheUint12>, std::time::Duration) {
    let start = Instant::now();
    let n = arr.len();

    for _ in 0..n {
        for j in 0..n-1 {
            let should_swap = arr[j].gt(&arr[j+1]);
            let should_swap_uint: FheUint12 = should_swap.cast_into();

            let one = FheUint12::try_encrypt(1u16, client_key).unwrap();
            let keep_same = &one - &should_swap_uint;

            let j_new = &should_swap_uint * &arr[j+1] + &keep_same * &arr[j];
            let j1_new = &should_swap_uint * &arr[j] + &keep_same * &arr[j+1];

            arr[j] = j_new;
            arr[j+1] = j1_new;
        }
    }

    let duration = start.elapsed();
    (arr, duration)
}

/// Private sorting on encrypted 16-bit array
pub fn sort_encrypted_u16(
    mut arr: Vec<FheUint16>,
    client_key: &ClientKey,
) -> (Vec<FheUint16>, std::time::Duration) {
    let start = Instant::now();
    let n = arr.len();

    for _ in 0..n {
        for j in 0..n-1 {
            let should_swap = arr[j].gt(&arr[j+1]);
            let should_swap_uint: FheUint16 = should_swap.cast_into();

            let one = FheUint16::try_encrypt(1u16, client_key).unwrap();
            let keep_same = &one - &should_swap_uint;

            let j_new = &should_swap_uint * &arr[j+1] + &keep_same * &arr[j];
            let j1_new = &should_swap_uint * &arr[j] + &keep_same * &arr[j+1];

            arr[j] = j_new;
            arr[j+1] = j1_new;
        }
    }

    let duration = start.elapsed();
    (arr, duration)
}

/// Plain sorting for testing correctness
pub fn sort_plaintext(mut arr: Vec<u16>) -> Vec<u16> {
    arr.sort();
    arr
}

/// Create a sample unsorted array for testing
pub fn create_sample_array(n: usize) -> Vec<u16> {
    // Create a reverse-sorted array for worst-case testing
    (0..n).map(|i| (n - i - 1) as u16).collect()
}

#[cfg(test)]
mod tests {
    use super::*;
    use tfhe::{generate_keys, set_server_key};
    use crate::security_params::get_secure_config;

    #[test]
    fn test_plaintext_sort() {
        let arr = vec![5, 2, 8, 1, 9];
        let sorted = sort_plaintext(arr);
        assert_eq!(sorted, vec![1, 2, 5, 8, 9]);
    }

    #[test]
    fn test_encrypted_sort_small() {
        let config = get_secure_config();
        let (client_key, server_keys) = generate_keys(config);
        set_server_key(server_keys);

        let arr_plain = vec![5u16, 2, 8, 1, 9];

        // Encrypt
        let arr_encrypted: Vec<FheUint8> = arr_plain
            .iter()
            .map(|&val| FheUint8::try_encrypt(val as u8, &client_key).unwrap())
            .collect();

        println!("Running encrypted sort on {} elements...", arr_plain.len());
        let (result_encrypted, duration) = sort_encrypted_u8(arr_encrypted, &client_key);

        // Decrypt
        let result_decrypted: Vec<u8> = result_encrypted
            .iter()
            .map(|val| val.decrypt(&client_key))
            .collect();

        println!("Decrypted result: {:?}", result_decrypted);
        println!("Encrypted computation took: {:?}", duration);

        // Verify correctness
        let mut expected = arr_plain.clone();
        expected.sort();
        for i in 0..arr_plain.len() {
            assert_eq!(
                result_decrypted[i] as u16,
                expected[i],
                "Mismatch at index {}", i
            );
        }
    }
}
