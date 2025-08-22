use std::time::Duration;

/// Analysis of private database aggregation complexity
#[derive(Debug, Clone)]
pub struct DatabaseAggregationComplexity {
    pub algorithm_name: String,
    pub num_rows: usize,
    pub comparison_count: usize,
    pub multiplication_count: usize,
    pub total_cp_mul_operations: usize,
    pub total_mul_cp_operations: usize,
}

impl DatabaseAggregationComplexity {
    /// Estimate total execution time based on single cp_mul and mul_cp operation times
    pub fn estimate_time(&self, cp_mul_time: Duration, mul_cp_time: Duration) -> Duration {
        let cp_mul_total = cp_mul_time * self.total_cp_mul_operations as u32;
        let mul_cp_total = mul_cp_time * self.total_mul_cp_operations as u32;
        cp_mul_total + mul_cp_total
    }
    
    /// Print detailed analysis
    pub fn print_analysis(&self, cp_mul_time: Duration, mul_cp_time: Duration) {
        println!("Algorithm: {}", self.algorithm_name);
        println!("  Number of rows: {}", self.num_rows);
        println!("  Comparisons: {}", self.comparison_count);
        println!("  Multiplications: {}", self.multiplication_count);
        println!("  Total cp_mul ops: {}", self.total_cp_mul_operations);
        println!("  Total mul_cp ops: {}", self.total_mul_cp_operations);
        println!("  Estimated time: {:.2?}", self.estimate_time(cp_mul_time, mul_cp_time));
        println!();
    }
}

/// Analyze database aggregation complexity
/// 
/// Query: SELECT ID FROM emp 
///        WHERE salary * work_hours BETWEEN 5000 AND 6000
///        AND salary + bonus BETWEEN 700 AND 800;
/// 
/// For each row:
/// 1. Predicate 1: salary * work_hours BETWEEN [5000, 6000]
///    - Multiplication: salary * work_hours (1 mul_cp)
///    - Range check: (result >= 5000) AND (result <= 6000)
///      - LT(result, 6001) AND LT(4999, result)  (2 comparisons -> 2 cp_mul)
///      - AND operation: multiply the two boolean results (1 mul_cp)
/// 
/// 2. Predicate 2: salary + bonus BETWEEN [700, 800]  
///    - Addition: salary + bonus (1 mul_cp, treating addition as multiplication for simplicity)
///    - Range check: (result >= 700) AND (result <= 800)
///      - LT(result, 801) AND LT(699, result)  (2 comparisons -> 2 cp_mul)
///      - AND operation: multiply the two boolean results (1 mul_cp)
/// 
/// 3. Combine predicates: predicate1 AND predicate2
///    - Multiply the two predicate results (1 mul_cp)
/// 
/// Total per row:
/// - Comparisons: 4 (2 for each range check)
/// - Multiplications: 4 (1 for salary*work_hours, 1 for salary+bonus, 2 for AND operations)
/// - cp_mul operations: 4 (all comparisons)
/// - mul_cp operations: 4 (all multiplications including AND operations)
pub fn analyze_database_aggregation(num_rows: usize) -> DatabaseAggregationComplexity {
    // Operations per row (as analyzed above)
    let comparisons_per_row = 4;
    let multiplications_per_row = 4;
    let cp_mul_per_row = 4;  // All comparisons
    let mul_cp_per_row = 4;  // All multiplications and AND operations
    
    // Total operations for all rows
    let comparison_count = num_rows * comparisons_per_row;
    let multiplication_count = num_rows * multiplications_per_row;
    let total_cp_mul_operations = num_rows * cp_mul_per_row;
    let total_mul_cp_operations = num_rows * mul_cp_per_row;
    
    DatabaseAggregationComplexity {
        algorithm_name: "Private Database Aggregation".to_string(),
        num_rows,
        comparison_count,
        multiplication_count,
        total_cp_mul_operations,
        total_mul_cp_operations,
    }
}

/// Simple database aggregation analysis output
pub fn analyze_database_aggregation_simple(num_rows: usize, cp_mul_time: Duration, mul_cp_time: Duration, bit_width: u8) {
    let db_analysis = analyze_database_aggregation(num_rows);
    let total_time = db_analysis.estimate_time(cp_mul_time, mul_cp_time);
    println!("Database Aggregation: {} rows, {}-bit: {}", num_rows, bit_width, format_duration(total_time));
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
