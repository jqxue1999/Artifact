use tfhe::prelude::*;
use tfhe::{generate_keys, set_server_key, ConfigBuilder, FheUint6,  FheUint8, FheUint12, FheUint16, FheUint24, FheUint32};
use std::time::Instant;

pub fn fhe_cmp_mul_6bit(clear_a: u8, clear_b: u8, clear_c: u8) -> Result<(u8, std::time::Duration), Box<dyn std::error::Error>> {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint6::try_encrypt(clear_a, &client_key)?;
    let encrypted_b = FheUint6::try_encrypt(clear_b, &client_key)?;
    let encrypted_c = FheUint6::try_encrypt(clear_c, &client_key)?;

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_a.le(&encrypted_b);
    let encrypted_cmp_6: FheUint6 = encrypted_cmp.cast_into();
    let encrypted_product = &encrypted_cmp_6 * &encrypted_c;
    let duration = start.elapsed();
    let clear_product: u8 = encrypted_product.decrypt(&client_key);

    Ok((clear_product, duration))
}

pub fn fhe_cmp_mul_8bit(clear_a: u8, clear_b: u8, clear_c: u8) -> Result<(u8, std::time::Duration), Box<dyn std::error::Error>> {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint8::try_encrypt(clear_a, &client_key)?;
    let encrypted_b = FheUint8::try_encrypt(clear_b, &client_key)?;
    let encrypted_c = FheUint8::try_encrypt(clear_c, &client_key)?;

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_a.le(&encrypted_b);
    let encrypted_cmp_8: FheUint8 = encrypted_cmp.cast_into();
    let encrypted_product = &encrypted_cmp_8 * &encrypted_c;
    let duration = start.elapsed();
    let clear_product: u8 = encrypted_product.decrypt(&client_key);

    Ok((clear_product, duration))
}

pub fn fhe_cmp_mul_16bit(clear_a: u16, clear_b: u16, clear_c: u16) -> Result<(u16, std::time::Duration), Box<dyn std::error::Error>> {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint16::try_encrypt(clear_a, &client_key)?;
    let encrypted_b = FheUint16::try_encrypt(clear_b, &client_key)?;
    let encrypted_c = FheUint16::try_encrypt(clear_c, &client_key)?;

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_a.le(&encrypted_b);
    let encrypted_cmp_16: FheUint16 = encrypted_cmp.cast_into();
    let encrypted_product = &encrypted_cmp_16 * &encrypted_c;
    let duration = start.elapsed();
    let clear_product: u16 = encrypted_product.decrypt(&client_key);

    Ok((clear_product, duration))
}

pub fn fhe_cmp_mul_12bit(clear_a: u16, clear_b: u16, clear_c: u16) -> Result<(u16, std::time::Duration), Box<dyn std::error::Error>> {
    let config = ConfigBuilder::default().build();
    let (client_key, server_keys) = generate_keys(config);

    let encrypted_a = FheUint12::try_encrypt(clear_a, &client_key)?;
    let encrypted_b = FheUint12::try_encrypt(clear_b, &client_key)?;
    let encrypted_c = FheUint12::try_encrypt(clear_c, &client_key)?;

    set_server_key(server_keys);

    let start = Instant::now();
    let encrypted_cmp = encrypted_a.le(&encrypted_b);
    let encrypted_cmp_12: FheUint12 = encrypted_cmp.cast_into();
    let encrypted_product = &encrypted_cmp_12 * &encrypted_c;
    let duration = start.elapsed();
    let clear_product: u16 = encrypted_product.decrypt(&client_key);

    Ok((clear_product, duration))
}
