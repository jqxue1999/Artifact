#include <iostream>
#include <vector>
#include <iomanip>

#include "utils.h"

using namespace std;
using namespace lbcrypto;

// Global counters for operations
int comparison_count = 0;
int multiplication_count = 0;

// Helper function: LT(a,b) returns 1 if a < b, else 0
// Each call counts as 1 comparison
int LT(int a, int b) {
    comparison_count++;
    return (a < b) ? 1 : 0;
}

// Helper function: EQ(a,b) returns 1 if a == b, else 0
// Each call counts as 1 comparison
int EQ(int a, int b) {
    comparison_count++;
    return (a == b) ? 1 : 0;
}

// Multiplication wrapper to count operations
int MUL(int a, int b) {
    multiplication_count++;
    return a * b;
}

// Private sorting algorithm implementation
vector<int> private_sort(vector<int>& X) {
    int m = X.size();
    
    // Reset counters
    comparison_count = 0;
    multiplication_count = 0;
    
    cout << "Input array: [";
    for (int i = 0; i < m; i++) {
        cout << X[i];
        if (i < m - 1) cout << ", ";
    }
    cout << "]" << endl;
    
    // Step 1: Construct comparison matrix L
    // L[i][j] = 1 if X[i] < X[j], 0 otherwise (for i != j)
    // This tells us for each pair which element is larger
    cout << "\nStep 1: Constructing comparison matrix L..." << endl;
    vector<vector<int>> L(m, vector<int>(m, 0));
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            if (i < j) {
                L[i][j] = LT(X[i], X[j]);
            } else if (i == j) {
                L[i][j] = 0;
            } else { // i > j
                L[i][j] = 1 - LT(X[j], X[i]);
            }
        }
    }
    
    // Print comparison matrix
    cout << "Comparison matrix L:" << endl;
    for (int i = 0; i < m; i++) {
        cout << "[";
        for (int j = 0; j < m; j++) {
            cout << L[i][j];
            if (j < m - 1) cout << " ";
        }
        cout << "]" << endl;
    }
    
    // Step 2: Compute index for each element
    // I[i] = number of elements larger than X[i] (rank from largest)
    cout << "\nStep 2: Computing indices..." << endl;
    vector<int> I(m);
    
    for (int i = 0; i < m; i++) {
        I[i] = 0;
        for (int j = 0; j < m; j++) {
            I[i] += L[i][j];
        }
    }
    
    // Print indices
    cout << "Indices: [";
    for (int i = 0; i < m; i++) {
        cout << I[i];
        if (i < m - 1) cout << ", ";
    }
    cout << "]" << endl;
    
    // Step 3: Reconstruct sorted array S using indices
    cout << "\nStep 3: Reconstructing sorted array..." << endl;
    vector<int> S(m, 0);
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            // The index I[j] tells us how many elements are larger than X[j]
            // So X[j] should be placed at position (m-1-I[j]) for ascending order
            S[i] += MUL(X[j], EQ(i, m - 1 - I[j]));
        }
    }
    
    return S;
}

void Sorting(uint32_t length, uint32_t integerBits) {
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
    comparison_count = 0;
    multiplication_count = 0;
    
    uint32_t m = length;
    
    // Create a dummy array for simulation
    vector<int> X(m);
    for (uint32_t i = 0; i < m; i++) {
        X[i] = i + 1; // Simple array [1, 2, 3, ..., m]
    }
    
    // Step 1: Comparison matrix L construction
    vector<vector<int>> L(m, vector<int>(m, 0));
    
    for (uint32_t i = 0; i < m; i++) {
        for (uint32_t j = 0; j < m; j++) {
            if (i < j) {
                L[i][j] = LT(X[i], X[j]);
            } else if (i == j) {
                L[i][j] = 0;
            } else { // i > j
                L[i][j] = 1 - LT(X[j], X[i]);
            }
        }
    }
    
    // Step 2: Index computation
    vector<int> I(m);
    for (uint32_t i = 0; i < m; i++) {
        I[i] = 0;
        for (uint32_t j = 0; j < m; j++) {
            I[i] += L[i][j];
        }
    }
    
    // Step 3: Reconstruction with counting
    vector<int> S(m, 0);
    for (uint32_t i = 0; i < m; i++) {
        for (uint32_t j = 0; j < m; j++) {
            // Each multiplication and EQ call is counted
            S[i] += MUL(X[j], EQ(i, m - 1 - I[j]));
        }
    }

    cout << "Total time: " << t_sec * (comparison_count + multiplication_count) / length << " s" << endl;
}

int main() {
    lbcrypto::OpenFHEParallelControls.Disable();

    cout << "========== Sorting Length 8 ==========" << endl;
    Sorting(8, 6);
    Sorting(8, 8);
    Sorting(8, 12);
    Sorting(8, 16);

    cout << "========== Sorting Length 16 ==========" << endl;
    Sorting(16, 6);
    Sorting(16, 8);
    Sorting(16, 12);
    Sorting(16, 16);
    
    cout << "========== Sorting Length 32 ==========" << endl;
    Sorting(32, 6);
    Sorting(32, 8);
    Sorting(32, 12);
    Sorting(32, 16);
    
    cout << "========== Sorting Length 64 ==========" << endl;
    Sorting(64, 6);
    Sorting(64, 8);
    Sorting(64, 12);
    Sorting(64, 16);
    
    return 0;
}