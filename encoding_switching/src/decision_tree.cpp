#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <random>
#include <chrono>
#include <helib/helib.h>
#include "bridge.h"
#include "ArgMapping.h"

using namespace std;
using namespace NTL;
using namespace helib;
using namespace he_bridge;

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

// Decision tree evaluation on encrypted data using encoding switching
// Evaluates complete binary trees using oblivious path selection
double EvaluateDecisionTree(const Bridge& bridge, const Context& context, const PubKey& pk,
                            const SecKey& sk, uint32_t depth, uint32_t integerBits) {
    const EncryptedArray& ea = context.getEA();
    long nslots = ea.size();
    long p = context.getP();
    long r = context.getR();
    long p2r = context.getPPowR();

    int num_internal_nodes = (1 << depth) - 1;  // 2^d - 1
    int num_leaves = 1 << depth;                 // 2^d

    // Generate random tree structure
    random_device rd;
    mt19937 gen(42);
    uniform_int_distribution<long> dis(1, (1 << (integerBits - 1)));

    vector<long> thresholds(num_internal_nodes);
    vector<long> leaf_values(num_leaves);
    vector<long> features(num_internal_nodes);

    for (int i = 0; i < num_internal_nodes; i++) {
        thresholds[i] = dis(gen);
        features[i] = dis(gen);
    }
    for (int i = 0; i < num_leaves; i++) {
        leaf_values[i] = dis(gen);
    }

    // Encrypt thresholds and features
    vector<Ctxt> enc_thresholds;
    vector<Ctxt> enc_features;
    vector<Ctxt> enc_leaves;

    for (int i = 0; i < num_internal_nodes; i++) {
        vector<long> thresh_vec(nslots, thresholds[i]);
        vector<long> feat_vec(nslots, features[i]);

        Ctxt ct_thresh(pk);
        Ctxt ct_feat(pk);
        ea.encrypt(ct_thresh, pk, thresh_vec);
        ea.encrypt(ct_feat, pk, feat_vec);

        enc_thresholds.push_back(ct_thresh);
        enc_features.push_back(ct_feat);
    }

    for (int i = 0; i < num_leaves; i++) {
        vector<long> leaf_vec(nslots, leaf_values[i]);
        Ctxt ct_leaf(pk);
        ea.encrypt(ct_leaf, pk, leaf_vec);
        enc_leaves.push_back(ct_leaf);
    }

    auto t_start = chrono::steady_clock::now();

    // Step 1: Perform comparisons at all internal nodes using encoding switching
    vector<Ctxt> comparison_results;

    for (int i = 0; i < num_internal_nodes; i++) {
        // Compute difference: feature - threshold
        Ctxt diff(pk);
        diff = enc_features[i];
        diff.addCtxt(enc_thresholds[i], true); // subtract

        // Compare: feature > threshold via encoding switching
        Ctxt comp_result(pk);
        bridge.compare(comp_result, diff);

        // Lift comparison result back to FV
        comp_result.multiplyModByP2R();
        Ctxt comp_lifted(pk);
        bridge.lift(comp_lifted, comp_result, r);

        comparison_results.push_back(comp_lifted);
    }

    // Step 2: Compute path indicator for each leaf
    vector<Ctxt> path_indicators;

    // Create constant for 1
    vector<long> ones_vec(nslots, 1);
    Ctxt ctxt_one(pk);
    ea.encrypt(ctxt_one, pk, ones_vec);

    for (int leaf_idx = 0; leaf_idx < num_leaves; leaf_idx++) {
        // Start with indicator = 1
        Ctxt indicator = ctxt_one;

        // Traverse path from root to this leaf
        int current = 0;
        int leaf_position = leaf_idx;

        for (uint32_t level = 0; level < depth; level++) {
            // Determine if we go left (0) or right (1) for this leaf
            int go_right = (leaf_position >> (depth - 1 - level)) & 1;

            if (go_right) {
                // Take right branch: multiply by comparison_result
                indicator.multiplyBy(comparison_results[current]);
            } else {
                // Take left branch: multiply by (1 - comparison_result)
                Ctxt inv_comp = ctxt_one;
                inv_comp.addCtxt(comparison_results[current], true); // 1 - comp
                indicator.multiplyBy(inv_comp);
            }

            // Move to next node in the path
            current = 2 * current + 1 + go_right;
        }

        path_indicators.push_back(indicator);
    }

    // Step 3: Oblivious selection - sum all (path_indicator * leaf_value)
    vector<long> zeros_vec(nslots, 0);
    Ctxt result(pk);
    ea.encrypt(result, pk, zeros_vec);

    for (int i = 0; i < num_leaves; i++) {
        Ctxt contribution(pk);
        contribution = path_indicators[i];
        contribution.multiplyBy(enc_leaves[i]);
        result.addCtxt(contribution);
    }

    auto t_end = chrono::steady_clock::now();
    return chrono::duration<double>(t_end - t_start).count();
}

int main(int argc, char *argv[]) {
    // Default parameters for 8-bit
    unsigned long p = 17;
    unsigned long r = 2;
    unsigned long m = 13201;
    unsigned long bits = 256;
    unsigned long c = 2;
    unsigned long t = 64;

    // Parse command line arguments
    ArgMapping amap;
    amap.arg("p", p, "the base plaintext modulus");
    amap.arg("r", r, "the lifting parameter for plaintext space p^r");
    amap.arg("m", m, "the order of the cyclotomic ring");
    amap.arg("b", bits, "the bitsize of the ciphertext modulus");
    amap.arg("c", c, "Number of columns of Key-Switching matrix");
    amap.arg("t", t, "The hamming weight of sk");
    amap.parse(argc, argv);

    cout << string(80, '=') << endl;
    cout << "HE-Bridge Encoding Switching Decision Tree Evaluation" << endl;
    cout << string(80, '=') << endl << endl;

    cout << "Parameters:" << endl;
    cout << "  m=" << m << ", p=" << p << ", r=" << r
         << ", bits=" << bits << ", c=" << c << ", skHwt=" << t << endl;
    cout << "  Plaintext space: p^r = " << (long)pow(p, r) << endl << endl;

    // Initialize context
    cout << "Initializing HE context..." << endl;
    Context context = ContextBuilder<BGV>()
        .m(m)
        .p(p)
        .r(r)
        .bits(bits)
        .c(c)
        .skHwt(t)
        .build();

    cout << "  Cyclotomic order m = " << context.getZMStar().getM() << endl;
    cout << "  ord(p) = " << context.getOrdP() << endl;
    cout << "  Number of slots = " << context.getEA().size() << endl << endl;

    // Generate keys
    cout << "Generating keys..." << endl;
    SecKey secret_key(context);
    secret_key.GenSecKey();
    addSome1DMatrices(secret_key);
    addFrbMatrices(secret_key);
    if (r > 1)
        addFrbMatrices(secret_key);
    PubKey& public_key = secret_key;

    // Initialize Bridge
    cout << "Initializing HE-Bridge..." << endl;
    unsigned long expansion_len = 1;
    bool verbose = false;
    CircuitType type = UNI;
    Bridge bridge(context, type, r, expansion_len, secret_key, verbose);
    cout << endl;

    // Compute integer bit width
    int integerBits = static_cast<int>(ceil(log2(pow(p, r))));

    // Experiment: Different depths with different bit widths
    vector<uint32_t> depths = {2, 4, 6, 8};
    vector<uint32_t> bit_widths = {6, 8};  // Removed 12, 16 due to memory constraints (>32GB needed)

    cout << "Evaluating decision trees with encoding switching" << endl << endl;

    for (auto d : depths) {
        int num_nodes = (1 << d) - 1;
        int num_leaves = 1 << d;

        cout << "Decision Tree Depth " << d
             << " (" << num_nodes << " nodes, " << num_leaves << " leaves)" << endl;
        cout << string(80, '-') << endl;
        cout << left << setw(15) << "Bit Width"
             << left << setw(20) << "Time"
             << left << setw(15) << "Comparisons"
             << left << setw(10) << "Status" << endl;
        cout << string(80, '-') << endl;

        // For now, just run with current bit width
        cout << left << setw(15) << integerBits;
        cout.flush();

        double time = EvaluateDecisionTree(bridge, context, public_key, secret_key, d, integerBits);

        cout << left << setw(20) << formatDuration(time);
        cout << left << setw(15) << num_nodes;
        cout << left << setw(10) << "✓" << endl;

        cout << endl;
    }

    cout << string(80, '=') << endl;

    return 0;
}
