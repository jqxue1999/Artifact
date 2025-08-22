use std::time::Duration;

/// Analysis of decision tree algorithm complexity in terms of comparison operations
#[derive(Debug, Clone)]
pub struct DecisionTreeComplexity {
    pub algorithm_name: String,
    pub tree_depth: usize,
    pub total_nodes: usize,
    pub comparison_count: usize,
    pub conditional_assignment_count: usize,
    pub total_cp_mul_operations: usize,
    pub total_mul_cp_operations: usize,
}

impl DecisionTreeComplexity {
    /// Estimate total execution time based on single cp_mul and mul_cp operation times
    pub fn estimate_time(&self, cp_mul_time: Duration, mul_cp_time: Duration) -> Duration {
        let cp_mul_total = cp_mul_time * self.total_cp_mul_operations as u32;
        let mul_cp_total = mul_cp_time * self.total_mul_cp_operations as u32;
        cp_mul_total + mul_cp_total
    }
    
    /// Print detailed analysis
    pub fn print_analysis(&self, cp_mul_time: Duration, mul_cp_time: Duration) {
        println!("Algorithm: {}", self.algorithm_name);
        println!("  Tree depth: {}", self.tree_depth);
        println!("  Total nodes: {}", self.total_nodes);
        println!("  Comparisons: {}", self.comparison_count);
        println!("  Conditional assignments: {}", self.conditional_assignment_count);
        println!("  Total cp_mul ops: {}", self.total_cp_mul_operations);
        println!("  Total mul_cp ops: {}", self.total_mul_cp_operations);
        println!("  Estimated time: {:.2?}", self.estimate_time(cp_mul_time, mul_cp_time));
        println!();
    }
}

/// Analyze decision tree complexity for a complete binary tree
/// 
/// In a decision tree traversal:
/// - At each internal node: perform a comparison to decide which branch to take
/// - The comparison result is used to conditionally select the next node
/// 
/// For a complete binary tree of depth d:
/// - Total nodes = 2^d - 1 (for depth d, counting from 1)
/// - In worst case, we traverse from root to leaf = d comparisons
/// - But since we're evaluating the entire tree structure, we consider all nodes
/// 
/// In TFHE terms:
/// - Each comparison: (feature < threshold) -> 1 cp_mul operation
/// - Each conditional path selection: requires 1 mul_cp operation for path weighting
/// - Each conditional assignment of result: 1 cp_mul operation
pub fn analyze_decision_tree(tree_depth: usize) -> DecisionTreeComplexity {
    // For a complete binary tree of depth d:
    // Total nodes = 2^d - 1
    let total_nodes = (1 << tree_depth) - 1;
    
    // Internal nodes (non-leaf nodes) = 2^(d-1) - 1
    let internal_nodes = if tree_depth > 0 { (1 << (tree_depth - 1)) - 1 } else { 0 };
    
    // Leaf nodes = 2^(d-1)  
    let leaf_nodes = if tree_depth > 0 { 1 << (tree_depth - 1) } else { 1 };
    
    // In decision tree evaluation:
    // 1. Each internal node performs one comparison: feature[i] < threshold
    // 2. Each comparison result is used for conditional path selection
    // 3. Each leaf node contributes to final result through conditional assignment
    
    let comparison_count = internal_nodes;  // One comparison per internal node
    let conditional_assignment_count = leaf_nodes;  // One conditional assignment per leaf
    
    // TFHE operation counts:
    // - cp_mul: comparisons + conditional assignments for final result
    // - mul_cp: path selection operations (each comparison result used for path weighting)
    let total_cp_mul_operations = comparison_count + conditional_assignment_count;
    let total_mul_cp_operations = comparison_count;  // Each comparison used for path selection
    
    DecisionTreeComplexity {
        algorithm_name: "Decision Tree Evaluation".to_string(),
        tree_depth,
        total_nodes,
        comparison_count,
        conditional_assignment_count,
        total_cp_mul_operations,
        total_mul_cp_operations,
    }
}

/// Simple decision tree analysis output
pub fn analyze_decision_tree_simple(tree_depth: usize, cp_mul_time: Duration, mul_cp_time: Duration, bit_width: u8) {
    let tree_analysis = analyze_decision_tree(tree_depth);
    let total_time = tree_analysis.estimate_time(cp_mul_time, mul_cp_time);
    println!("Decision Tree: depth {}, {}-bit: {}", tree_depth, bit_width, format_duration(total_time));
}

/// Format duration in a simple way
fn format_duration(duration: Duration) -> String {
    let total_secs = duration.as_secs_f64();
    
    if total_secs < 60.0 {
        format!("{:.1}s", total_secs)
    } else if total_secs < 3600.0 {
        format!("{:.1}m", total_secs / 60.0)
    } else if total_secs < 86400.0 {
        format!("{:.1}h", total_secs / 3600.0)
    } else {
        format!("{:.1}d", total_secs / 86400.0)
    }
}
