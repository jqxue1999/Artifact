#include <iostream>
#include <vector>
#include <iomanip>
#include <climits>

#include "utils.h"

using namespace std;
using namespace lbcrypto;

// Use a large but safe value for infinity to avoid overflow
const int INF = 999999;

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

// Addition wrapper (no counting needed as addition is typically free in SIMD)
int ADD(int a, int b) {
    return a + b;
}

// SIMD-style broadcast: replicate a single value across all positions in a vector
vector<int> broadcast(int value, int size) {
    vector<int> result(size, value);
    return result;
}

// SIMD-style element-wise addition of two vectors
vector<int> vectorAdd(const vector<int>& a, const vector<int>& b) {
    vector<int> result(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        result[i] = ADD(a[i], b[i]);
    }
    return result;
}

// SIMD-style element-wise comparison: returns mask vector where 1 means a[i] < b[i]
vector<int> vectorLT(const vector<int>& a, const vector<int>& b) {
    vector<int> mask(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        mask[i] = LT(a[i], b[i]);
    }
    return mask;
}

// SIMD-style element-wise multiplication with mask
vector<int> vectorMaskedSelect(const vector<int>& mask, const vector<int>& a, const vector<int>& b) {
    vector<int> result(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        // result[i] = mask[i] * a[i] + (1 - mask[i]) * b[i]
        int term1 = MUL(mask[i], a[i]);
        int inv_mask = 1 - mask[i];  // This subtraction is free
        int term2 = MUL(inv_mask, b[i]);
        result[i] = ADD(term1, term2);
    }
    return result;
}

// Private Floyd-Warshall algorithm implementation
void privateFloydWarshall(int n, vector<vector<int>>& G, uint32_t integerBits) {

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
    
    // Initialize distance matrix D and predecessor matrix P
    vector<vector<int>> D(n, vector<int>(n));
    vector<vector<int>> P(n, vector<int>(n));
    
    // Initialize distance matrix (copy of adjacency matrix)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            D[i][j] = G[i][j];
            P[i][j] = i; // Predecessor initialization
        }
    }
    
    cout << "Starting private Floyd-Warshall algorithm for " << n << " nodes..." << endl;
    
    // Main Floyd-Warshall algorithm with SIMD simulation
    for (int k = 0; k < n; k++) {        
        for (int i = 0; i < n; i++) {
            // Step 1: Broadcast D[i,k] across all slots (SIMD replication)
            vector<int> broadcasted_dik = broadcast(D[i][k], n);
            
            // Step 2: Get current row k as a vector
            vector<int> row_k(n);
            for (int j = 0; j < n; j++) {
                row_k[j] = D[k][j];
            }
            
            // Step 3: Compute candidate distances: D_new = D[i,k] + D[k,:]
            vector<int> D_new = vectorAdd(broadcasted_dik, row_k);
            
            // Step 4: Get current row i
            vector<int> current_row_i(n);
            for (int j = 0; j < n; j++) {
                current_row_i[j] = D[i][j];
            }
            
            // Step 5: Compare element-wise: M = (D_new < D[i,:])
            vector<int> mask = vectorLT(D_new, current_row_i);
            
            // Step 6: Update distances: D[i,:] = M * D_new + (1-M) * D[i,:]
            vector<int> updated_distances = vectorMaskedSelect(mask, D_new, current_row_i);
            
            // Step 7: Update predecessors with the same mask
            vector<int> current_pred_i(n);
            for (int j = 0; j < n; j++) {
                current_pred_i[j] = P[i][j];
            }
            vector<int> new_pred_i = broadcast(k, n); // If path through k is shorter, predecessor becomes k
            vector<int> updated_predecessors = vectorMaskedSelect(mask, new_pred_i, current_pred_i);
            
            // Step 8: Store updated values back to matrices
            for (int j = 0; j < n; j++) {
                D[i][j] = updated_distances[j];
                P[i][j] = updated_predecessors[j];
            }
        }
    }
    
    // Print operation counts
    // cout << "Total Comparisons = " << comparisonCount << endl;
    // cout << "Total Multiplications = " << multiplicationCount << endl;
    // cout << "Expected: Comparisons = " << n*n*n << ", Multiplications = " << 4*n*n*n << endl;

    cout << "Total time: " << t_sec * (comparisonCount + multiplicationCount) / n << " s" << endl;
}

// Test function for Floyd-Warshall
void FloydWarshall(uint32_t nodes, uint32_t integerBits) {
    lbcrypto::OpenFHEParallelControls.Disable();
    
    cout << "========== Floyd-Warshall Test ==========" << endl;
    cout << "Nodes: " << nodes << ", Integer Bits: " << integerBits << endl;
    
    // Create a test graph with some edges
    vector<vector<int>> G(nodes, vector<int>(nodes, INF));
    
    // Initialize diagonal to 0 (distance from node to itself)
    for (uint32_t i = 0; i < nodes; i++) {
        G[i][i] = 0;
    }
    
    // Add some test edges to create a connected graph
    if (nodes >= 4) {
        G[0][1] = 5;
        G[0][3] = 10;
        G[1][2] = 3;
        G[2][3] = 1;
        G[1][3] = 2;
        G[2][0] = 7;
        
        // Add more edges for larger graphs
        if (nodes >= 6) {
            G[3][4] = 6;
            G[4][5] = 4;
            G[5][0] = 8;
            G[4][1] = 1;
        }
        
        if (nodes >= 8) {
            G[5][6] = 2;
            G[6][7] = 3;
            G[7][0] = 9;
            G[6][2] = 5;
        }
    } else {
        // Simple case for small graphs
        if (nodes >= 2) G[0][1] = 3;
        if (nodes >= 3) {
            G[1][2] = 2;
            G[2][0] = 4;
        }
    }
    
    // Run the private Floyd-Warshall algorithm
    privateFloydWarshall(nodes, G, integerBits);
    cout << endl;
}

int main() {
    lbcrypto::OpenFHEParallelControls.Disable();

    cout << "========== Floyd-Warshall Algorithm Tests ==========" << endl;
    
    // Test with different graph sizes
    cout << "========== Small Graph (16 nodes) ==========" << endl;
    FloydWarshall(16, 6);
    FloydWarshall(16, 8);
    FloydWarshall(16, 12);
    FloydWarshall(16, 16);
    
    cout << "========== Medium Graph (32 nodes) ==========" << endl;
    FloydWarshall(32, 6);
    FloydWarshall(32, 8);
    FloydWarshall(32, 12);
    FloydWarshall(32, 16);
    
    cout << "========== Larger Graph (64 nodes) ==========" << endl;
    FloydWarshall(64, 6);
    FloydWarshall(64, 8);
    FloydWarshall(64, 12);
    FloydWarshall(64, 16);

    cout << "========== Small Graph (128 nodes) ==========" << endl;
    FloydWarshall(128, 6);
    FloydWarshall(128, 8);
    FloydWarshall(128, 12);
    FloydWarshall(128, 16);
    
    return 0;
}
