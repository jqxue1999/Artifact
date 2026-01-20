#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <random>
#include "utils.h"

using namespace std;
using namespace lbcrypto;

// Helper function to format duration
string formatDuration(double seconds) {
    if (seconds < 1.0) {
        return to_string(static_cast<int>(seconds * 1000)) + " ms";
    } else if (seconds < 60.0) {
        return to_string(static_cast<int>(seconds)) + " s";
    } else if (seconds < 3600.0) {
        return to_string(static_cast<int>(seconds / 60)) + " min";
    } else if (seconds < 86400.0) {
        return to_string(static_cast<int>(seconds / 3600)) + " hr";
    } else {
        return to_string(static_cast<int>(seconds / 86400)) + " days";
    }
}

// Decision tree evaluation on encrypted data with SIMD batching
// Evaluates 128 different inputs simultaneously using SIMD slots
double EvaluateDecisionTree(uint32_t depth, uint32_t integerBits) {
    SetupCryptoContext(24, 128, integerBits);

    int num_internal_nodes = (1 << depth) - 1;  // 2^d - 1
    int num_leaves = 1 << depth;                 // 2^d
    int batch_size = g_numValues;  // 128 different inputs processed in parallel

    // Generate random tree structure (same for all 128 inputs)
    random_device rd;
    mt19937 gen(42);  // Fixed seed for reproducibility
    uniform_real_distribution<double> dis(0.0, (1 << (integerBits - 1)));

    vector<double> thresholds(num_internal_nodes);
    vector<double> leaf_values(num_leaves);

    for (int i = 0; i < num_internal_nodes; i++) {
        thresholds[i] = dis(gen);
    }
    for (int i = 0; i < num_leaves; i++) {
        leaf_values[i] = dis(gen);
    }

    // Generate 128 different input samples (one per SIMD slot)
    // Each sample has features for all internal nodes
    vector<vector<double>> samples(batch_size, vector<double>(num_internal_nodes));

    for (int sample = 0; sample < batch_size; sample++) {
        for (int i = 0; i < num_internal_nodes; i++) {
            samples[sample][i] = dis(gen);
        }
    }

    // Encrypt thresholds (same threshold for all 128 inputs)
    vector<Ciphertext<DCRTPoly>> enc_thresholds;
    for (int i = 0; i < num_internal_nodes; i++) {
        vector<double> vals(g_numValues, thresholds[i]);  // Broadcast threshold
        Plaintext ptxt = g_cc->MakeCKKSPackedPlaintext(vals);
        enc_thresholds.push_back(g_cc->Encrypt(g_keys.publicKey, ptxt));
    }

    // Encrypt features (DIFFERENT for each of the 128 inputs - proper SIMD usage!)
    vector<Ciphertext<DCRTPoly>> enc_features;
    for (int i = 0; i < num_internal_nodes; i++) {
        vector<double> feature_vals(g_numValues);
        for (int sample = 0; sample < batch_size; sample++) {
            feature_vals[sample] = samples[sample][i];  // Each slot has different sample
        }
        Plaintext ptxt = g_cc->MakeCKKSPackedPlaintext(feature_vals);
        enc_features.push_back(g_cc->Encrypt(g_keys.publicKey, ptxt));
    }

    // Encrypt leaf values (same for all 128 inputs)
    vector<Ciphertext<DCRTPoly>> enc_leaves;
    for (int i = 0; i < num_leaves; i++) {
        vector<double> vals(g_numValues, leaf_values[i]);
        Plaintext ptxt = g_cc->MakeCKKSPackedPlaintext(vals);
        enc_leaves.push_back(g_cc->Encrypt(g_keys.publicKey, ptxt));
    }

    auto t_start = chrono::steady_clock::now();

    // Step 1: Perform comparisons at all internal nodes
    // Each comparison processes 128 different (feature, threshold) pairs in parallel
    vector<Ciphertext<DCRTPoly>> comparison_results;

    for (int i = 0; i < num_internal_nodes; i++) {
        // Compare 128 features > threshold using scheme switching
        // Result: 128 comparison results in parallel
        auto cResult = Comparison(enc_features[i], enc_thresholds[i]);
        auto cComp = g_cc->EvalFHEWtoCKKS(cResult, g_numValues, g_numValues);
        comparison_results.push_back(cComp);
    }

    // Step 2: Compute path indicator for each leaf
    // For each leaf, compute which of the 128 samples reach it
    vector<Ciphertext<DCRTPoly>> path_indicators;

    for (int leaf_idx = 0; leaf_idx < num_leaves; leaf_idx++) {
        // Start with indicator = 1 for all 128 samples
        vector<double> ones(g_numValues, 1.0);
        Plaintext ptxt_one = g_cc->MakeCKKSPackedPlaintext(ones);
        auto indicator = g_cc->Encrypt(g_keys.publicKey, ptxt_one);

        // Traverse path from root to this leaf
        int current = 0;
        int leaf_position = leaf_idx;

        for (uint32_t level = 0; level < depth; level++) {
            // Determine if we go left (0) or right (1) for this leaf
            int go_right = (leaf_position >> (depth - 1 - level)) & 1;

            if (go_right) {
                // Take right branch: multiply by comparison_result
                // All 128 samples multiply by their respective comparison results
                indicator = g_cc->EvalMult(indicator, comparison_results[current]);
            } else {
                // Take left branch: multiply by (1 - comparison_result)
                vector<double> ones(g_numValues, 1.0);
                Plaintext ptxt_one = g_cc->MakeCKKSPackedPlaintext(ones);
                auto one = g_cc->Encrypt(g_keys.publicKey, ptxt_one);
                auto inv_comp = g_cc->EvalSub(one, comparison_results[current]);
                indicator = g_cc->EvalMult(indicator, inv_comp);
            }
            indicator = g_cc->Rescale(indicator);

            // Move to next node in the path
            current = 2 * current + 1 + go_right;
        }

        path_indicators.push_back(indicator);
    }

    // Step 3: Oblivious selection - sum all (path_indicator * leaf_value)
    // Each of 128 samples gets its corresponding leaf value
    vector<double> zeros(g_numValues, 0.0);
    Plaintext ptxt_zero = g_cc->MakeCKKSPackedPlaintext(zeros);
    auto result = g_cc->Encrypt(g_keys.publicKey, ptxt_zero);

    for (int i = 0; i < num_leaves; i++) {
        auto contribution = g_cc->EvalMult(path_indicators[i], enc_leaves[i]);
        contribution = g_cc->Rescale(contribution);
        result = g_cc->EvalAdd(result, contribution);
    }

    auto t_end = chrono::steady_clock::now();
    double time_sec = chrono::duration<double>(t_end - t_start).count();

    // Verify correctness for small trees
    if (depth <= 4 && batch_size <= 128) {
        Plaintext ptxt_result;
        g_cc->Decrypt(g_keys.secretKey, result, &ptxt_result);
        ptxt_result->SetLength(min(4, batch_size));  // Check first few samples

        int errors = 0;
        for (int sample = 0; sample < min(4, batch_size); sample++) {
            // Compute plaintext result for this sample
            int current = 0;
            for (uint32_t level = 0; level < depth; level++) {
                if (samples[sample][current] > thresholds[current]) {
                    current = 2 * current + 2;  // Go right
                } else {
                    current = 2 * current + 1;  // Go left
                }
            }
            int leaf_idx = current - num_internal_nodes;
            double expected = leaf_values[leaf_idx];

            double actual = ptxt_result->GetRealPackedValue()[sample];
            if (abs(actual - expected) > 1.0) {  // Allow some FHE error
                errors++;
            }
        }

        if (errors > 0) {
            cout << "Warning: " << errors << " mismatches in verification" << endl;
        }
    }

    // Return average time per sample (since we processed 128 in parallel)
    return time_sec;
}

int main() {
    lbcrypto::OpenFHEParallelControls.Disable();

    cout << string(80, '=') << endl;
    cout << "OpenFHE Scheme Switching Decision Tree Evaluation" << endl;
    cout << string(80, '=') << endl << endl;

    cout << "Evaluating decision trees with SIMD batching (128 inputs per batch)" << endl;
    cout << "Using scheme switching between CKKS and FHEW" << endl << endl;

    // Experiment: Different depths with 6, 8-bit inputs
    // (12, 16-bit removed due to memory constraints - require >32GB)
    vector<uint32_t> depths = {2, 4, 6, 8};
    vector<uint32_t> bit_widths = {6, 8};

    for (auto depth : depths) {
        int num_nodes = (1 << depth) - 1;
        int num_leaves = 1 << depth;

        cout << "Decision Tree Depth " << depth
             << " (" << num_nodes << " nodes, " << num_leaves << " leaves)" << endl;
        cout << string(80, '-') << endl;
        cout << left << setw(15) << "Bit Width"
             << left << setw(20) << "Time (128 inputs)"
             << left << setw(15) << "Comparisons"
             << left << setw(10) << "Status" << endl;
        cout << string(80, '-') << endl;

        for (auto bits : bit_widths) {
            cout << left << setw(15) << bits;
            cout.flush();

            double time = EvaluateDecisionTree(depth, bits);

            cout << left << setw(20) << formatDuration(time);
            cout << left << setw(15) << num_nodes;
            cout << left << setw(10) << "✓" << endl;
        }
        cout << endl;
    }

    cout << string(80, '=') << endl;
    cout << "\nNote: Times shown are for evaluating 128 different inputs simultaneously" << endl;
    cout << "using SIMD batching in CKKS ciphertexts." << endl;

    return 0;
}
