use tfhe::prelude::*;
use tfhe::{ClientKey, FheUint6, FheUint8, FheUint12, FheUint16};
use std::time::Instant;

/// A simple binary decision tree node
#[derive(Clone)]
pub struct TreeNode {
    pub feature_idx: usize,
    pub threshold: u16,
    pub left_class: u8,   // Class label if going left (and it's a leaf)
    pub right_class: u8,  // Class label if going right (and it's a leaf)
    pub is_leaf_left: bool,
    pub is_leaf_right: bool,
    pub left_child: Option<Box<TreeNode>>,
    pub right_child: Option<Box<TreeNode>>,
}

/// Evaluate decision tree on encrypted features (8-bit)
///
/// **Security**: Uses TFHE parameters providing >= 128-bit security
/// (both classical and post-quantum).
///
/// Traverses the tree obliviously by evaluating all paths and selecting
/// the correct one based on encrypted comparisons.
pub fn evaluate_tree_u8(
    features: &[FheUint8],
    tree: &TreeNode,
    client_key: &ClientKey,
) -> (FheUint8, std::time::Duration) {
    let start = Instant::now();

    let result = evaluate_node_u8(features, tree, client_key);

    let duration = start.elapsed();
    (result, duration)
}

fn evaluate_node_u8(
    features: &[FheUint8],
    node: &TreeNode,
    client_key: &ClientKey,
) -> FheUint8 {
    // Compare feature with threshold
    let feature_val = &features[node.feature_idx];
    let threshold_enc = FheUint8::try_encrypt(node.threshold as u8, client_key).unwrap();

    // go_left = (feature < threshold)
    let go_left = feature_val.lt(&threshold_enc);
    let go_left_uint: FheUint8 = go_left.cast_into();

    // go_right = 1 - go_left
    let one = FheUint8::try_encrypt(1u8, client_key).unwrap();
    let go_right = &one - &go_left_uint;

    // Get results from left and right paths
    let left_result = if node.is_leaf_left {
        FheUint8::try_encrypt(node.left_class, client_key).unwrap()
    } else {
        evaluate_node_u8(features, node.left_child.as_ref().unwrap(), client_key)
    };

    let right_result = if node.is_leaf_right {
        FheUint8::try_encrypt(node.right_class, client_key).unwrap()
    } else {
        evaluate_node_u8(features, node.right_child.as_ref().unwrap(), client_key)
    };

    // Obliviously select: result = go_left * left_result + go_right * right_result
    &go_left_uint * &left_result + &go_right * &right_result
}

/// Evaluate decision tree on encrypted features (6-bit)
pub fn evaluate_tree_u6(
    features: &[FheUint6],
    tree: &TreeNode,
    client_key: &ClientKey,
) -> (FheUint6, std::time::Duration) {
    let start = Instant::now();

    let result = evaluate_node_u6(features, tree, client_key);

    let duration = start.elapsed();
    (result, duration)
}

fn evaluate_node_u6(
    features: &[FheUint6],
    node: &TreeNode,
    client_key: &ClientKey,
) -> FheUint6 {
    let feature_val = &features[node.feature_idx];
    let threshold_enc = FheUint6::try_encrypt(
        if node.threshold > 63 { 63 } else { node.threshold as u8 },
        client_key
    ).unwrap();

    let go_left = feature_val.lt(&threshold_enc);
    let go_left_uint: FheUint6 = go_left.cast_into();

    let one = FheUint6::try_encrypt(1u8, client_key).unwrap();
    let go_right = &one - &go_left_uint;

    let left_result = if node.is_leaf_left {
        FheUint6::try_encrypt(node.left_class, client_key).unwrap()
    } else {
        evaluate_node_u6(features, node.left_child.as_ref().unwrap(), client_key)
    };

    let right_result = if node.is_leaf_right {
        FheUint6::try_encrypt(node.right_class, client_key).unwrap()
    } else {
        evaluate_node_u6(features, node.right_child.as_ref().unwrap(), client_key)
    };

    &go_left_uint * &left_result + &go_right * &right_result
}

/// Evaluate decision tree on encrypted features (12-bit)
pub fn evaluate_tree_u12(
    features: &[FheUint12],
    tree: &TreeNode,
    client_key: &ClientKey,
) -> (FheUint12, std::time::Duration) {
    let start = Instant::now();

    let result = evaluate_node_u12(features, tree, client_key);

    let duration = start.elapsed();
    (result, duration)
}

fn evaluate_node_u12(
    features: &[FheUint12],
    node: &TreeNode,
    client_key: &ClientKey,
) -> FheUint12 {
    let feature_val = &features[node.feature_idx];
    let threshold_enc = FheUint12::try_encrypt(node.threshold, client_key).unwrap();

    let go_left = feature_val.lt(&threshold_enc);
    let go_left_uint: FheUint12 = go_left.cast_into();

    let one = FheUint12::try_encrypt(1u16, client_key).unwrap();
    let go_right = &one - &go_left_uint;

    let left_result = if node.is_leaf_left {
        FheUint12::try_encrypt(node.left_class as u16, client_key).unwrap()
    } else {
        evaluate_node_u12(features, node.left_child.as_ref().unwrap(), client_key)
    };

    let right_result = if node.is_leaf_right {
        FheUint12::try_encrypt(node.right_class as u16, client_key).unwrap()
    } else {
        evaluate_node_u12(features, node.right_child.as_ref().unwrap(), client_key)
    };

    &go_left_uint * &left_result + &go_right * &right_result
}

/// Evaluate decision tree on encrypted features (16-bit)
pub fn evaluate_tree_u16(
    features: &[FheUint16],
    tree: &TreeNode,
    client_key: &ClientKey,
) -> (FheUint16, std::time::Duration) {
    let start = Instant::now();

    let result = evaluate_node_u16(features, tree, client_key);

    let duration = start.elapsed();
    (result, duration)
}

fn evaluate_node_u16(
    features: &[FheUint16],
    node: &TreeNode,
    client_key: &ClientKey,
) -> FheUint16 {
    let feature_val = &features[node.feature_idx];
    let threshold_enc = FheUint16::try_encrypt(node.threshold, client_key).unwrap();

    let go_left = feature_val.lt(&threshold_enc);
    let go_left_uint: FheUint16 = go_left.cast_into();

    let one = FheUint16::try_encrypt(1u16, client_key).unwrap();
    let go_right = &one - &go_left_uint;

    let left_result = if node.is_leaf_left {
        FheUint16::try_encrypt(node.left_class as u16, client_key).unwrap()
    } else {
        evaluate_node_u16(features, node.left_child.as_ref().unwrap(), client_key)
    };

    let right_result = if node.is_leaf_right {
        FheUint16::try_encrypt(node.right_class as u16, client_key).unwrap()
    } else {
        evaluate_node_u16(features, node.right_child.as_ref().unwrap(), client_key)
    };

    &go_left_uint * &left_result + &go_right * &right_result
}

/// Plain evaluation for testing
pub fn evaluate_tree_plain(features: &[u16], tree: &TreeNode) -> u8 {
    let feature_val = features[tree.feature_idx];

    if feature_val < tree.threshold {
        if tree.is_leaf_left {
            tree.left_class
        } else {
            evaluate_tree_plain(features, tree.left_child.as_ref().unwrap())
        }
    } else {
        if tree.is_leaf_right {
            tree.right_class
        } else {
            evaluate_tree_plain(features, tree.right_child.as_ref().unwrap())
        }
    }
}

/// Create a sample tree for testing (depth 3, binary classification)
pub fn create_sample_tree() -> TreeNode {
    // Tree structure:
    //          [feature 0 < 50]
    //           /           \
    //    [f1 < 30]          [f1 < 70]
    //     /    \             /     \
    //   cls0  cls1         cls1   cls0

    TreeNode {
        feature_idx: 0,
        threshold: 50,
        left_class: 0,
        right_class: 0,
        is_leaf_left: false,
        is_leaf_right: false,
        left_child: Some(Box::new(TreeNode {
            feature_idx: 1,
            threshold: 30,
            left_class: 0,
            right_class: 1,
            is_leaf_left: true,
            is_leaf_right: true,
            left_child: None,
            right_child: None,
        })),
        right_child: Some(Box::new(TreeNode {
            feature_idx: 1,
            threshold: 70,
            left_class: 1,
            right_class: 0,
            is_leaf_left: true,
            is_leaf_right: true,
            left_child: None,
            right_child: None,
        })),
    }
}

/// Calculate tree depth
pub fn tree_depth(node: &TreeNode) -> usize {
    let left_depth = if node.is_leaf_left {
        0
    } else {
        tree_depth(node.left_child.as_ref().unwrap())
    };

    let right_depth = if node.is_leaf_right {
        0
    } else {
        tree_depth(node.right_child.as_ref().unwrap())
    };

    1 + std::cmp::max(left_depth, right_depth)
}

#[cfg(test)]
mod tests {
    use super::*;
    use tfhe::{generate_keys, set_server_key};
    use crate::security_params::get_secure_config;

    #[test]
    fn test_plaintext_tree() {
        let tree = create_sample_tree();

        // Test case 1: feature[0]=20, feature[1]=10 -> class 0
        let features = vec![20, 10];
        assert_eq!(evaluate_tree_plain(&features, &tree), 0);

        // Test case 2: feature[0]=20, feature[1]=40 -> class 1
        let features = vec![20, 40];
        assert_eq!(evaluate_tree_plain(&features, &tree), 1);

        // Test case 3: feature[0]=60, feature[1]=50 -> class 1
        let features = vec![60, 50];
        assert_eq!(evaluate_tree_plain(&features, &tree), 1);

        // Test case 4: feature[0]=60, feature[1]=80 -> class 0
        let features = vec![60, 80];
        assert_eq!(evaluate_tree_plain(&features, &tree), 0);
    }

    #[test]
    fn test_encrypted_tree_small() {
        let config = get_secure_config();
        let (client_key, server_keys) = generate_keys(config);
        set_server_key(server_keys);

        let tree = create_sample_tree();
        let features_plain = vec![20u16, 10];

        // Encrypt features
        let features_encrypted: Vec<FheUint8> = features_plain
            .iter()
            .map(|&val| FheUint8::try_encrypt(val as u8, &client_key).unwrap())
            .collect();

        println!("Running encrypted decision tree evaluation...");
        let (result_encrypted, duration) = evaluate_tree_u8(&features_encrypted, &tree, &client_key);

        // Decrypt
        let result_decrypted: u8 = result_encrypted.decrypt(&client_key);
        let expected = evaluate_tree_plain(&features_plain, &tree);

        println!("Encrypted result: {}", result_decrypted);
        println!("Expected result: {}", expected);
        println!("Evaluation took: {:?}", duration);

        assert_eq!(result_decrypted, expected);
    }
}
