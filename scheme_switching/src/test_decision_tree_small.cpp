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
    } else {
        return to_string(static_cast<int>(seconds / 60)) + " min";
    }
}

// Simplified decision tree test
double TestDecisionTree(uint32_t depth, uint32_t integerBits) {
    SetupCryptoContext(24, 128, integerBits);

    int num_internal_nodes = (1 << depth) - 1;
    int num_leaves = 1 << depth;

    cout << "  Tree: " << num_internal_nodes << " nodes, " << num_leaves << " leaves" << endl;

    // Generate random tree
    random_device rd;
    mt19937 gen(42);
    uniform_real_distribution<double> dis(0.0, (1 << (integerBits - 1)));

    int batch_size = g_numValues;  // 128 inputs

    vector<double> thresholds(num_internal_nodes);
    vector<double> leaf_values(num_leaves);

    // Generate 128 different input samples (SIMD batching!)
    vector<vector<double>> samples(batch_size, vector<double>(num_internal_nodes));

    for (int i = 0; i < num_internal_nodes; i++) {
        thresholds[i] = dis(gen);
    }
    for (int i = 0; i < num_leaves; i++) {
        leaf_values[i] = dis(gen);
    }
    for (int sample = 0; sample < batch_size; sample++) {
        for (int i = 0; i < num_internal_nodes; i++) {
            samples[sample][i] = dis(gen);
        }
    }

    // Encrypt data
    cout << "  Encrypting " << batch_size << " samples using SIMD..." << endl;
    vector<Ciphertext<DCRTPoly>> enc_thresholds;
    vector<Ciphertext<DCRTPoly>> enc_features;
    vector<Ciphertext<DCRTPoly>> enc_leaves;

    for (int i = 0; i < num_internal_nodes; i++) {
        // Thresholds: broadcast same value to all slots
        vector<double> vals(g_numValues, thresholds[i]);
        Plaintext ptxt = g_cc->MakeCKKSPackedPlaintext(vals);
        enc_thresholds.push_back(g_cc->Encrypt(g_keys.publicKey, ptxt));

        // Features: different value per slot (SIMD!)
        vector<double> feature_vals(g_numValues);
        for (int sample = 0; sample < batch_size; sample++) {
            feature_vals[sample] = samples[sample][i];
        }
        ptxt = g_cc->MakeCKKSPackedPlaintext(feature_vals);
        enc_features.push_back(g_cc->Encrypt(g_keys.publicKey, ptxt));
    }

    for (int i = 0; i < num_leaves; i++) {
        vector<double> vals(g_numValues, leaf_values[i]);
        Plaintext ptxt = g_cc->MakeCKKSPackedPlaintext(vals);
        enc_leaves.push_back(g_cc->Encrypt(g_keys.publicKey, ptxt));
    }

    cout << "  Performing encrypted comparisons..." << endl;
    auto t_start = chrono::steady_clock::now();

    // Step 1: Comparisons
    vector<Ciphertext<DCRTPoly>> comparison_results;
    for (int i = 0; i < num_internal_nodes; i++) {
        auto cResult = Comparison(enc_features[i], enc_thresholds[i]);
        auto cComp = g_cc->EvalFHEWtoCKKS(cResult, g_numValues, g_numValues);
        comparison_results.push_back(cComp);
        cout << "    Comparison " << (i+1) << "/" << num_internal_nodes << " done" << endl;
    }

    cout << "  Computing path indicators..." << endl;
    // Step 2: Simplified - just use first leaf
    vector<Ciphertext<DCRTPoly>> path_indicators;
    for (int leaf_idx = 0; leaf_idx < num_leaves; leaf_idx++) {
        vector<double> ones(g_numValues, 1.0);
        Plaintext ptxt_one = g_cc->MakeCKKSPackedPlaintext(ones);
        auto indicator = g_cc->Encrypt(g_keys.publicKey, ptxt_one);

        int current = 0;
        int leaf_position = leaf_idx;

        for (uint32_t level = 0; level < depth; level++) {
            int go_right = (leaf_position >> (depth - 1 - level)) & 1;

            if (go_right) {
                indicator = g_cc->EvalMult(indicator, comparison_results[current]);
            } else {
                vector<double> ones(g_numValues, 1.0);
                Plaintext ptxt_one = g_cc->MakeCKKSPackedPlaintext(ones);
                auto one = g_cc->Encrypt(g_keys.publicKey, ptxt_one);
                auto inv_comp = g_cc->EvalSub(one, comparison_results[current]);
                indicator = g_cc->EvalMult(indicator, inv_comp);
            }
            indicator = g_cc->Rescale(indicator);

            current = 2 * current + 1 + go_right;
        }

        path_indicators.push_back(indicator);
    }

    cout << "  Oblivious selection..." << endl;
    // Step 3: Oblivious selection
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

    cout << "  ✓ Evaluation completed" << endl;

    return time_sec;
}

int main() {
    lbcrypto::OpenFHEParallelControls.Disable();

    cout << string(80, '=') << endl;
    cout << "Testing Decision Tree - Depth 2, 6-bit" << endl;
    cout << string(80, '=') << endl << endl;

    double time = TestDecisionTree(2, 6);

    cout << "\nTime: " << formatDuration(time) << endl;
    cout << "\n✓ Decision tree test completed successfully!" << endl;

    return 0;
}
