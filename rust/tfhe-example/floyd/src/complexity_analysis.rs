use std::time::Duration;

/// Analysis of Floyd-Warshall algorithm complexity in terms of comparison and addition operations
#[derive(Debug, Clone)]
pub struct FloydWarshallComplexity {
    pub algorithm_name: String,
    pub node_count: usize,
    pub comparison_count: usize,
    pub addition_count: usize,
    pub total_cp_mul_operations: usize,
    pub total_mul_cp_operations: usize,
}

impl FloydWarshallComplexity {
    /// Estimate total execution time based on single cp_mul and mul_cp operation times
    pub fn estimate_time(&self, cp_mul_time: Duration, mul_cp_time: Duration) -> Duration {
        let cp_mul_total = cp_mul_time * self.total_cp_mul_operations as u32;
        let mul_cp_total = mul_cp_time * self.total_mul_cp_operations as u32;
        cp_mul_total + mul_cp_total
    }
    
    /// Print detailed analysis
    pub fn print_analysis(&self, cp_mul_time: Duration, mul_cp_time: Duration) {
        println!("Algorithm: {}", self.algorithm_name);
        println!("  Node count: {}", self.node_count);
        println!("  Comparisons: {}", self.comparison_count);
        println!("  Additions: {}", self.addition_count);
        println!("  Total cp_mul ops: {}", self.total_cp_mul_operations);
        println!("  Total mul_cp ops: {}", self.total_mul_cp_operations);
        println!("  Estimated time: {:.2?}", self.estimate_time(cp_mul_time, mul_cp_time));
        println!();
    }
}

/// Analyze Floyd-Warshall algorithm complexity
/// For each k, i, j: 
/// - 1 comparison: (dist[i][k] + dist[k][j] < dist[i][j])
/// - 1 conditional assignment: if true, update dist[i][j]
/// 
/// In TFHE terms:
/// - Comparison: (a + b < c) requires: 1 addition + 1 comparison = 1 mul_cp + 1 cp_mul
/// - Conditional assignment: requires 1 cp_mul operation
pub fn analyze_floyd_warshall(n: usize) -> FloydWarshallComplexity {
    // Floyd-Warshall has triple nested loops: O(n³)
    let total_iterations = n * n * n;
    
    // Each iteration performs:
    // 1. Addition: dist[i][k] + dist[k][j] -> 1 mul_cp operation
    // 2. Comparison: (sum < dist[i][j]) -> 1 cp_mul operation  
    // 3. Conditional update: dist[i][j] = min(dist[i][j], sum) -> 1 cp_mul operation
    
    let comparison_count = total_iterations;  // One comparison per iteration
    let addition_count = total_iterations;    // One addition per iteration
    let total_cp_mul_operations = total_iterations * 2; // 1 for comparison + 1 for conditional update
    let total_mul_cp_operations = total_iterations;     // 1 for addition
    
    FloydWarshallComplexity {
        algorithm_name: "Floyd-Warshall".to_string(),
        node_count: n,
        comparison_count,
        addition_count,
        total_cp_mul_operations,
        total_mul_cp_operations,
    }
}

/// Simple Floyd-Warshall analysis output
pub fn analyze_floyd_warshall_simple(n: usize, cp_mul_time: Duration, mul_cp_time: Duration, bit_width: u8) {
    let floyd_analysis = analyze_floyd_warshall(n);
    let total_time = floyd_analysis.estimate_time(cp_mul_time, mul_cp_time);
    println!("Floyd-Warshall: {} nodes, {}-bit: {}", n, bit_width, format_duration(total_time));
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
