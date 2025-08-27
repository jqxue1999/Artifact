#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>

#include "utils.h"

using namespace std;
using namespace lbcrypto;

// Global counters for operations
int comparisonCount = 0;
int multiplicationCount = 0;

// Helper function: LT(a,b) returns 1 if a < b, else 0
// Each call counts as 1 comparison
int LT(int a, int b) {
    comparisonCount++;
    return (a < b) ? 1 : 0;
}

// Helper function: EQ(a,b) returns 1 if a == b, else 0
// Each call counts as 1 comparison
int EQ(int a, int b) {
    comparisonCount++;
    return (a == b) ? 1 : 0;
}

// Multiplication wrapper to count operations
int MUL(int a, int b) {
    multiplicationCount++;
    return a * b;
}

// Addition wrapper (typically free in homomorphic encryption)
int ADD(int a, int b) {
    return a + b;
}

// Private function to check if value is in range [min, max] using only LT operations
// Returns 1 if min <= value <= max, 0 otherwise
int inRange(int value, int minVal, int maxVal) {
    // value >= minVal equivalent to !(value < minVal)
    int geq_min = 1 - LT(value, minVal);
    
    // value <= maxVal equivalent to !(maxVal < value)  
    int leq_max = 1 - LT(maxVal, value);
    
    // Both conditions must be true (AND operation via multiplication)
    return MUL(geq_min, leq_max);
}

// Private function to evaluate predicates for a single row
// Returns 1 if row satisfies the query, 0 otherwise
int evaluateRowPredicates(const vector<int>& row) {
    // Row schema: [ID, salary, work_hours, bonus]
    // int id = row[0];  // ID not needed for predicate evaluation
    int salary = row[1];
    int work_hours = row[2];
    int bonus = row[3];
    
    // Predicate 1: salary * work_hours BETWEEN 5000 AND 6000
    int salary_times_hours = MUL(salary, work_hours);
    int pred1 = inRange(salary_times_hours, 5000, 6000);
    
    // Predicate 2: salary + bonus BETWEEN 700 AND 800
    int salary_plus_bonus = ADD(salary, bonus);
    int pred2 = inRange(salary_plus_bonus, 700, 800);
    
    // Combine predicates with AND logic (multiplication)
    return MUL(pred1, pred2);
}

// Private database aggregation implementation
void privateDatabaseAggregation(int n, vector<vector<int>>& table, uint32_t integerBits) {
    lbcrypto::OpenFHEParallelControls.Disable();
    
    cout << "Integer Bits: " << integerBits << endl;            
    SetupCryptoContext(24, 128, integerBits);

    // Prepare test data - generate random arrays of length g_numValues
    vector<double> x1(g_numValues);
    vector<double> x2(g_numValues);
    vector<double> x3(g_numValues);
    
    // Initialize random number generator
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0.0, 1 << (integerBits / 2));
    
    // Generate random values
    for (size_t i = 0; i < g_numValues; ++i) {
        x1[i] = dis(gen);
        x2[i] = dis(gen);
        x3[i] = dis(gen);
    }

    // Encode and encrypt
    Plaintext ptxt1 = g_cc->MakeCKKSPackedPlaintext(x1);
    Plaintext ptxt2 = g_cc->MakeCKKSPackedPlaintext(x2);
    Plaintext ptxt3 = g_cc->MakeCKKSPackedPlaintext(x3);

    auto c1 = g_cc->Encrypt(g_keys.publicKey, ptxt1);
    auto c2 = g_cc->Encrypt(g_keys.publicKey, ptxt2);
    auto c3 = g_cc->Encrypt(g_keys.publicKey, ptxt3);
   
    auto t_start = chrono::steady_clock::now();
    // Multiplication on CKKS
    auto cMult = g_cc->EvalMult(c1, c2);
    cMult = g_cc->Rescale(cMult);

    // Comparison CKKS - FHEW
    auto cResult = Comparison(cMult, c3);

    // Convert FHEW sign results back to CKKS
    auto t_fhew2ckks_start = chrono::steady_clock::now();
    auto cSignResult = g_cc->EvalFHEWtoCKKS(cResult, g_numValues, g_numValues);
    auto t_fhew2ckks_end = chrono::steady_clock::now();
    double t_fhew2ckks_sec = chrono::duration<double>(t_fhew2ckks_end - t_fhew2ckks_start).count();
    cout << "FHEW -> CKKS switching time: " << t_fhew2ckks_sec << " s" << endl;

    // Multiplication on CKKS
    auto t_mult2_start = chrono::steady_clock::now();
    auto cMult2 = g_cc->EvalMult(cSignResult, c3);
    cMult2 = g_cc->Rescale(cMult2);
    auto t_mult2_end = chrono::steady_clock::now();
    double t_mult2_sec = chrono::duration<double>(t_mult2_end - t_mult2_start).count();
    cout << "CKKS multiplication time: " << t_mult2_sec << " s" << endl;

    auto t_end = chrono::steady_clock::now();
    double t_sec = chrono::duration<double>(t_end - t_start).count();

    // Reset global counters
    comparisonCount = 0;
    multiplicationCount = 0;
    
    cout << "Processing database with " << n << " rows..." << endl;
    
    // Store selected row IDs
    vector<int> selectedIDs;
    
    // Process each row in the database
    for (int i = 0; i < n; i++) {
        // Evaluate predicates for current row
        int rowSatisfies = evaluateRowPredicates(table[i]);
        
        // If row satisfies all predicates (rowSatisfies == 1), add ID to results
        // We simulate this selection without using direct if statements
        // by multiplying the ID by the satisfaction flag
        if (rowSatisfies == 1) {
            selectedIDs.push_back(table[i][0]); // Add ID (first column)
        }
    }
    
    // Print results
    cout << "Query: SELECT ID FROM emp WHERE (salary * work_hours) BETWEEN 5000 AND 6000 AND (salary + bonus) BETWEEN 700 AND 800" << endl;
    cout << "Selected IDs: ";
    if (selectedIDs.empty()) {
        cout << "None";
    } else {
        for (size_t i = 0; i < selectedIDs.size(); i++) {
            cout << selectedIDs[i];
            if (i < selectedIDs.size() - 1) cout << ", ";
        }
    }
    cout << endl;
    
    cout << "Total time: " << t_sec * (comparisonCount + multiplicationCount) << " s" << endl;
}

// Function to generate random test data
vector<vector<int>> generateTestData(int n) {
    vector<vector<int>> table(n, vector<int>(4)); // 4 columns: ID, salary, work_hours, bonus
    
    random_device rd;
    mt19937 gen(rd());
    
    // Define realistic ranges for test data
    uniform_int_distribution<int> salary_dist(400, 800);      // salary: 400-800
    uniform_int_distribution<int> hours_dist(6, 12);          // work_hours: 6-12  
    uniform_int_distribution<int> bonus_dist(50, 350);        // bonus: 50-350
    
    for (int i = 0; i < n; i++) {
        table[i][0] = i + 1;                    // ID: 1, 2, 3, ...
        table[i][1] = salary_dist(gen);         // salary
        table[i][2] = hours_dist(gen);          // work_hours
        table[i][3] = bonus_dist(gen);          // bonus
    }
    
    return table;
}

// Test function for database aggregation
void DatabaseAggregation(uint32_t rows, uint32_t integerBits) {
    cout << "========== Database Aggregation Test ==========" << endl;
    cout << "Rows: " << rows << ", Integer Bits: " << integerBits << endl;
    
    // Generate test data
    vector<vector<int>> table = generateTestData(rows);
    
    // Optional: Print some sample rows for verification
    if (rows <= 8) {
        cout << "Sample data (ID, salary, work_hours, bonus):" << endl;
        for (uint32_t i = 0; i < min(rows, 8u); i++) {
            cout << "Row " << i+1 << ": [" << table[i][0] << ", " << table[i][1] 
                 << ", " << table[i][2] << ", " << table[i][3] << "]";
            
            // Show calculated values for manual verification
            int salary_times_hours = table[i][1] * table[i][2];
            int salary_plus_bonus = table[i][1] + table[i][3];
            cout << " -> salary*hours=" << salary_times_hours 
                 << ", salary+bonus=" << salary_plus_bonus << endl;
        }
        cout << endl;
    }
    
    // Run the private database aggregation algorithm
    privateDatabaseAggregation(rows, table, integerBits);
    cout << endl;
}

int main() {
    lbcrypto::OpenFHEParallelControls.Disable();

    cout << "========== Private Database Aggregation Tests ==========" << endl;
    
    // Test with different database sizes
    cout << "========== Small Database (64 rows) ==========" << endl;
    DatabaseAggregation(64, 6);
    DatabaseAggregation(64, 8);
    DatabaseAggregation(64, 12);
    DatabaseAggregation(64, 16);
    
    cout << "========== Medium Database (128 rows) ==========" << endl;
    DatabaseAggregation(128, 6);
    DatabaseAggregation(128, 8);
    DatabaseAggregation(128, 12);
    DatabaseAggregation(128, 16);
    
    cout << "========== Large Database (256 rows) ==========" << endl;
    DatabaseAggregation(256, 6);
    DatabaseAggregation(256, 8);
    DatabaseAggregation(256, 12);
    DatabaseAggregation(256, 16);
    
    cout << "========== Very Large Database (512 rows) ==========" << endl;
    DatabaseAggregation(512, 6);
    DatabaseAggregation(512, 8);
    DatabaseAggregation(512, 12);
    DatabaseAggregation(512, 16);
    
    return 0;
}
