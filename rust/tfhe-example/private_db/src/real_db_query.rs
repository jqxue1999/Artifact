use tfhe::prelude::*;
use tfhe::{ClientKey, FheUint8, FheUint16};
use std::time::Instant;

/// Database record structure
#[derive(Debug, Clone)]
pub struct DatabaseRecord {
    pub id: u8,
    pub salary: u8,
    pub work_hours: u8,
    pub bonus: u16,
}

/// Execute Query 1 on encrypted 8-bit database
///
/// Query: SELECT ID FROM emp
///        WHERE salary * work_hours BETWEEN 5000 AND 6000
///        AND salary + bonus BETWEEN 700 AND 800
///
/// **Security**: Uses TFHE parameters providing >= 128-bit security
/// (both classical and post-quantum).
pub fn execute_query1_encrypted_u8(
    records: &[DatabaseRecord],
    client_key: &ClientKey,
) -> (Vec<u8>, std::time::Duration) {
    let start = Instant::now();

    let mut matching_ids = Vec::new();

    for record in records {
        // Encrypt the record fields
        let enc_salary = FheUint8::try_encrypt(record.salary, client_key).unwrap();
        let enc_hours = FheUint8::try_encrypt(record.work_hours, client_key).unwrap();
        let enc_bonus = FheUint16::try_encrypt(record.bonus, client_key).unwrap();

        // Predicate 1: salary * work_hours BETWEEN 5000 AND 6000
        // Since we're using 8-bit values, salary * work_hours can be at most 255*255 = 65025
        // We need 16-bit for the result
        let enc_salary_16: FheUint16 = enc_salary.cast_into();
        let enc_hours_16: FheUint16 = enc_hours.cast_into();
        let enc_product = &enc_salary_16 * &enc_hours_16;

        let lower1 = FheUint16::try_encrypt(5000u16, client_key).unwrap();
        let upper1 = FheUint16::try_encrypt(6000u16, client_key).unwrap();

        // Check: product >= 5000 AND product <= 6000
        let ge_lower1 = enc_product.ge(&lower1);  // product >= 5000
        let le_upper1 = enc_product.le(&upper1);  // product <= 6000

        // Convert to FheUint8 for AND operation
        let ge_lower1_u8: FheUint8 = ge_lower1.cast_into();
        let le_upper1_u8: FheUint8 = le_upper1.cast_into();
        let pred1 = &ge_lower1_u8 * &le_upper1_u8;  // AND operation

        // Predicate 2: salary + bonus BETWEEN 700 AND 800
        let enc_sum = &enc_salary_16 + &enc_bonus;

        let lower2 = FheUint16::try_encrypt(700u16, client_key).unwrap();
        let upper2 = FheUint16::try_encrypt(800u16, client_key).unwrap();

        // Check: sum >= 700 AND sum <= 800
        let ge_lower2 = enc_sum.ge(&lower2);
        let le_upper2 = enc_sum.le(&upper2);

        let ge_lower2_u8: FheUint8 = ge_lower2.cast_into();
        let le_upper2_u8: FheUint8 = le_upper2.cast_into();
        let pred2 = &ge_lower2_u8 * &le_upper2_u8;  // AND operation

        // Combine predicates: pred1 AND pred2
        let final_pred = &pred1 * &pred2;

        // Decrypt to check if this record matches
        let matches: u8 = final_pred.decrypt(client_key);

        if matches > 0 {
            matching_ids.push(record.id);
        }
    }

    let duration = start.elapsed();
    (matching_ids, duration)
}

/// Execute query on plaintext for testing
pub fn execute_query1_plaintext(records: &[DatabaseRecord]) -> Vec<u8> {
    let mut matching_ids = Vec::new();

    for record in records {
        let product = (record.salary as u16) * (record.work_hours as u16);
        let sum = (record.salary as u16) + record.bonus;

        // Predicate 1: salary * work_hours BETWEEN 5000 AND 6000
        let pred1 = product >= 5000 && product <= 6000;

        // Predicate 2: salary + bonus BETWEEN 700 AND 800
        let pred2 = sum >= 700 && sum <= 800;

        if pred1 && pred2 {
            matching_ids.push(record.id);
        }
    }

    matching_ids
}

/// Generate sample database records
pub fn generate_sample_database(num_rows: usize) -> Vec<DatabaseRecord> {
    let mut records = Vec::new();

    for i in 0..num_rows {
        // Create records that occasionally match the query
        // salary * work_hours should sometimes be in [5000, 6000]
        // salary + bonus should sometimes be in [700, 800]

        let salary = 20 + ((i * 7) % 80) as u8;  // Range: 20-99
        let work_hours = 40 + ((i * 11) % 40) as u8;  // Range: 40-79
        let bonus = 600 + ((i * 13) % 200) as u16;  // Range: 600-799

        records.push(DatabaseRecord {
            id: i as u8,
            salary,
            work_hours,
            bonus,
        });
    }

    records
}

#[cfg(test)]
mod tests {
    use super::*;
    use tfhe::{generate_keys, set_server_key};
    use crate::security_params::get_secure_config;

    #[test]
    fn test_plaintext_query() {
        let records = vec![
            DatabaseRecord { id: 1, salary: 50, work_hours: 110, bonus: 700 },  // 50*110=5500 ✓, 50+700=750 ✓
            DatabaseRecord { id: 2, salary: 100, work_hours: 50, bonus: 750 },   // 100*50=5000 ✓, 100+750=850 ✗
            DatabaseRecord { id: 3, salary: 60, work_hours: 100, bonus: 700 },   // 60*100=6000 ✓, 60+700=760 ✓
        ];

        let result = execute_query1_plaintext(&records);

        println!("Matching IDs: {:?}", result);
        assert!(result.contains(&1));
        assert!(!result.contains(&2));
        assert!(result.contains(&3));
    }

    #[test]
    fn test_encrypted_query_small() {
        let config = get_secure_config();
        let (client_key, server_keys) = generate_keys(config);
        set_server_key(server_keys);

        // Create a small test database
        let records = vec![
            DatabaseRecord { id: 1, salary: 50, work_hours: 110, bonus: 700 },
            DatabaseRecord { id: 2, salary: 60, work_hours: 100, bonus: 700 },
        ];

        println!("Running encrypted query on {} records...", records.len());
        let (encrypted_result, duration) = execute_query1_encrypted_u8(&records, &client_key);

        println!("Encrypted query took: {:?}", duration);
        println!("Matching encrypted IDs: {:?}", encrypted_result);

        // Verify correctness
        let plaintext_result = execute_query1_plaintext(&records);
        assert_eq!(encrypted_result, plaintext_result);
    }
}
