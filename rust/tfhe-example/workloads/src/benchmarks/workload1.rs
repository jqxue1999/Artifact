use tfhe::prelude::*;
use tfhe::{generate_keys, set_server_key, ConfigBuilder, FheUint6, FheUint12, FheUint32, FheUint8, FheUint16};
use std::time::Instant;

// 6位: (a*b) compare c
pub fn fhe_mul_cmp_6bit(clear_a: u8, clear_b: u8, clear_c: u16) -> Result<(bool, std::time::Duration), Box<dyn std::error::Error>> {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint6::try_encrypt(clear_a, &client_key)?;
    let encrypted_b = FheUint6::try_encrypt(clear_b, &client_key)?;
    let encrypted_c = FheUint12::try_encrypt(clear_c, &client_key)?;

    set_server_key(server_keys);

    let encrypted_a_12: FheUint12 = encrypted_a.cast_into();
    let encrypted_b_12: FheUint12 = encrypted_b.cast_into();

    let start = Instant::now();

    // 计算 (a * b) 和比较 (a*b) <= c
    let encrypted_product = &encrypted_a_12 * &encrypted_b_12;
    let encrypted_cmp = encrypted_product.le(&encrypted_c);

    let duration = start.elapsed();
    let clear_result: bool = encrypted_cmp.decrypt(&client_key);

    Ok((clear_result, duration))
}

// 8位: (a*b) compare c
pub fn fhe_mul_cmp_8bit(clear_a: u8, clear_b: u8, clear_c: u16) -> Result<(bool, std::time::Duration), Box<dyn std::error::Error>> {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint8::try_encrypt(clear_a, &client_key)?;
    let encrypted_b = FheUint8::try_encrypt(clear_b, &client_key)?;
    let encrypted_c = FheUint16::try_encrypt(clear_c, &client_key)?;

    set_server_key(server_keys);

    let encrypted_a_16: FheUint16 = encrypted_a.cast_into();
    let encrypted_b_16: FheUint16 = encrypted_b.cast_into();

    let start = Instant::now();

    // 计算 (a * b) 和比较 (a*b) <= c
    let encrypted_product = &encrypted_a_16 * &encrypted_b_16;
    let encrypted_cmp = encrypted_product.le(&encrypted_c);

    let duration = start.elapsed();
    let clear_result: bool = encrypted_cmp.decrypt(&client_key);

    Ok((clear_result, duration))
}

// 16位: (a*b) compare c
pub fn fhe_mul_cmp_16bit(clear_a: u16, clear_b: u16, clear_c: u32) -> Result<(bool, std::time::Duration), Box<dyn std::error::Error>> {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint16::try_encrypt(clear_a, &client_key)?;
    let encrypted_b = FheUint16::try_encrypt(clear_b, &client_key)?;
    let encrypted_c = FheUint32::try_encrypt(clear_c, &client_key)?;

    set_server_key(server_keys);

    let encrypted_a_32: FheUint32 = encrypted_a.cast_into();
    let encrypted_b_32: FheUint32 = encrypted_b.cast_into();

    let start = Instant::now();

    // 计算 (a * b) 和比较 (a*b) <= c
    let encrypted_product = &encrypted_a_32 * &encrypted_b_32;
    let encrypted_cmp = encrypted_product.le(&encrypted_c);

    let duration = start.elapsed();
    let clear_result: bool = encrypted_cmp.decrypt(&client_key);

    Ok((clear_result, duration))
}
