use tfhe::prelude::*;
use tfhe::{generate_keys, set_server_key, ConfigBuilder, FheUint6, FheUint12, FheUint32, FheUint8, FheUint16};
use std::time::Instant;


pub fn fhe_mul_cmp_mul_6bit(clear_a: u8, clear_b: u8, clear_c: u8, clear_d: u8) -> Result<(bool, std::time::Duration), Box<dyn std::error::Error>> {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint6::try_encrypt(clear_a, &client_key)?;
    let encrypted_b = FheUint6::try_encrypt(clear_b, &client_key)?;
    let encrypted_c = FheUint6::try_encrypt(clear_c, &client_key)?;
    let encrypted_d = FheUint6::try_encrypt(clear_d, &client_key)?;

    set_server_key(server_keys);

    let encrypted_a_12: FheUint12 = encrypted_a.cast_into();
    let encrypted_b_12: FheUint12 = encrypted_b.cast_into();
    let encrypted_c_12: FheUint12 = encrypted_c.cast_into();
    let encrypted_d_12: FheUint12 = encrypted_d.cast_into();

    let start = Instant::now();

    // 计算 (a * b) 和 (c * d)
    let encrypted_product_ab = &encrypted_a_12 * &encrypted_b_12;
    let encrypted_product_cd = &encrypted_c_12 * &encrypted_d_12;
    
    // 比较两个乘积: (a*b) <= (c*d)
    let encrypted_cmp = encrypted_product_ab.le(&encrypted_product_cd);

    let duration = start.elapsed();
    let clear_result: bool = encrypted_cmp.decrypt(&client_key);

    Ok((clear_result, duration))
}

pub fn fhe_mul_cmp_mul_8bit(clear_a: u8, clear_b: u8, clear_c: u8, clear_d: u8) -> Result<(bool, std::time::Duration), Box<dyn std::error::Error>> {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint8::try_encrypt(clear_a, &client_key)?;
    let encrypted_b = FheUint8::try_encrypt(clear_b, &client_key)?;
    let encrypted_c = FheUint8::try_encrypt(clear_c, &client_key)?;
    let encrypted_d = FheUint8::try_encrypt(clear_d, &client_key)?;

    set_server_key(server_keys);

    let encrypted_a_16: FheUint16 = encrypted_a.cast_into();
    let encrypted_b_16: FheUint16 = encrypted_b.cast_into();
    let encrypted_c_16: FheUint16 = encrypted_c.cast_into();
    let encrypted_d_16: FheUint16 = encrypted_d.cast_into();

    let start = Instant::now();

    // 计算 (a * b) 和 (c * d)
    let encrypted_product_ab = &encrypted_a_16 * &encrypted_b_16;
    let encrypted_product_cd = &encrypted_c_16 * &encrypted_d_16;
    
    // 比较两个乘积: (a*b) <= (c*d)
    let encrypted_cmp = encrypted_product_ab.le(&encrypted_product_cd);

    let duration = start.elapsed();
    let clear_result: bool = encrypted_cmp.decrypt(&client_key);

    Ok((clear_result, duration))
}

pub fn fhe_mul_cmp_mul_16bit(clear_a: u16, clear_b: u16, clear_c: u16, clear_d: u16) -> Result<(bool, std::time::Duration), Box<dyn std::error::Error>> {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint16::try_encrypt(clear_a, &client_key)?;
    let encrypted_b = FheUint16::try_encrypt(clear_b, &client_key)?;
    let encrypted_c = FheUint16::try_encrypt(clear_c, &client_key)?;
    let encrypted_d = FheUint16::try_encrypt(clear_d, &client_key)?;

    set_server_key(server_keys);

    let encrypted_a_32: FheUint32 = encrypted_a.cast_into();
    let encrypted_b_32: FheUint32 = encrypted_b.cast_into();
    let encrypted_c_32: FheUint32 = encrypted_c.cast_into();
    let encrypted_d_32: FheUint32 = encrypted_d.cast_into();

    let start = Instant::now();

    // 计算 (a * b) 和 (c * d)
    let encrypted_product_ab = &encrypted_a_32 * &encrypted_b_32;
    let encrypted_product_cd = &encrypted_c_32 * &encrypted_d_32;
    
    // 比较两个乘积: (a*b) <= (c*d)
    let encrypted_cmp = encrypted_product_ab.le(&encrypted_product_cd);

    let duration = start.elapsed();
    let clear_result: bool = encrypted_cmp.decrypt(&client_key);

    Ok((clear_result, duration))
}