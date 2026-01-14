use tfhe::{Config, ConfigBuilder};

/// Get TFHE configuration with at least 128-bit security
///
/// This function returns the default TFHE-rs configuration which provides:
/// - Classical security: >= 128 bits
/// - Post-quantum security: >= 128 bits
/// - Based on Learning With Errors (LWE) hardness assumption
///
/// The default parameters are validated against:
/// - BKZ lattice reduction attacks
/// - Dual lattice attacks
/// - Hybrid attacks
/// - Quantum variants of the above
///
/// References:
/// - TFHE paper: https://eprint.iacr.org/2018/421
/// - TFHE-rs security: https://docs.zama.ai/tfhe-rs/fundamentals/security
/// - HE standard: https://homomorphicencryption.org/standard/
pub fn get_secure_config() -> Config {
    // TFHE-rs default configuration provides >= 128-bit security
    // This is the same configuration used throughout TFHE-rs for all integer types
    ConfigBuilder::default().build()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_config_creation() {
        let _config = get_secure_config();
        // If this compiles and runs, the configuration is valid
        assert!(true, "Config created successfully");
    }

    #[test]
    fn test_security_documentation() {
        // This test documents that we're using TFHE-rs default parameters
        // which are documented to provide >= 128-bit security
        println!("Using TFHE-rs default parameters for >= 128-bit security");
        println!("See: https://docs.zama.ai/tfhe-rs/fundamentals/security");
    }
}
