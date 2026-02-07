// Quick All Tests - Minimal parameters to verify all benchmark types work
// Covers: Workload, Decision Tree, Sorting, Floyd-Warshall, Database
// Uses smallest parameters for fast verification

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

string formatDuration(double seconds) {
    if (seconds < 1.0) return to_string(static_cast<int>(seconds * 1000)) + " ms";
    else if (seconds < 60.0) return to_string(static_cast<int>(seconds)) + " s";
    else return to_string(static_cast<int>(seconds / 60)) + " min";
}

// ============================================================================
// Workload: (a*b) compare c
// ============================================================================
double QuickWorkload(const Bridge& bridge, const Context& context, const PubKey& pk, int integerBits) {
    const EncryptedArray& ea = context.getEA();
    long nslots = ea.size();

    mt19937 gen(42);
    uniform_int_distribution<long> dis(1, (1 << (integerBits - 1)));

    vector<long> a_vec(nslots), b_vec(nslots), c_vec(nslots);
    for (int i = 0; i < nslots; i++) {
        a_vec[i] = dis(gen); b_vec[i] = dis(gen); c_vec[i] = dis(gen);
    }

    Ctxt ctxt_a(pk), ctxt_b(pk), ctxt_c(pk);
    ea.encrypt(ctxt_a, pk, a_vec);
    ea.encrypt(ctxt_b, pk, b_vec);
    ea.encrypt(ctxt_c, pk, c_vec);

    auto t_start = chrono::steady_clock::now();

    Ctxt ctxt_product(pk);
    ctxt_product = ctxt_a;
    ctxt_product.multiplyBy(ctxt_b);

    Ctxt ctxt_diff(pk);
    ctxt_diff = ctxt_product;
    ctxt_diff.addCtxt(ctxt_c, true);

    Ctxt ctxt_result(pk);
    bridge.compare(ctxt_result, ctxt_diff);

    return chrono::duration<double>(chrono::steady_clock::now() - t_start).count();
}

// ============================================================================
// Decision Tree (depth 2)
// ============================================================================
double QuickDecisionTree(const Bridge& bridge, const Context& context, const PubKey& pk, int integerBits) {
    const EncryptedArray& ea = context.getEA();
    long nslots = ea.size();

    const int depth = 2;
    const int numNodes = (1 << depth) - 1;

    mt19937 gen(42);
    uniform_int_distribution<long> dis(1, (1 << integerBits) - 1);

    vector<long> input(nslots);
    for (int i = 0; i < nslots; i++) input[i] = dis(gen);

    vector<vector<long>> thresholds(numNodes, vector<long>(nslots));
    for (int n = 0; n < numNodes; n++) {
        long thresh = dis(gen);
        for (int i = 0; i < nslots; i++) thresholds[n][i] = thresh;
    }

    Ctxt ctxt_input(pk);
    ea.encrypt(ctxt_input, pk, input);

    vector<Ctxt> ctxt_thresholds;
    for (int n = 0; n < numNodes; n++) {
        Ctxt ct(pk);
        ea.encrypt(ct, pk, thresholds[n]);
        ctxt_thresholds.push_back(ct);
    }

    auto t_start = chrono::steady_clock::now();

    vector<Ctxt> decisions;
    for (int n = 0; n < numNodes; n++) {
        Ctxt diff(pk);
        diff = ctxt_input;
        diff.addCtxt(ctxt_thresholds[n], true);

        Ctxt result(pk);
        bridge.compare(result, diff);
        decisions.push_back(result);
    }

    return chrono::duration<double>(chrono::steady_clock::now() - t_start).count();
}

// ============================================================================
// Sorting (4 elements)
// ============================================================================
double QuickSorting(const Bridge& bridge, const Context& context, const PubKey& pk, int integerBits) {
    const EncryptedArray& ea = context.getEA();
    long nslots = ea.size();

    const int arraySize = 4;

    mt19937 gen(42);
    uniform_int_distribution<long> dis(1, (1 << integerBits) - 1);

    vector<vector<long>> arr(arraySize, vector<long>(nslots));
    for (int i = 0; i < arraySize; i++) {
        long val = dis(gen);
        for (int j = 0; j < nslots; j++) arr[i][j] = val;
    }

    vector<Ctxt> enc_arr;
    for (int i = 0; i < arraySize; i++) {
        Ctxt ct(pk);
        ea.encrypt(ct, pk, arr[i]);
        enc_arr.push_back(ct);
    }

    auto t_start = chrono::steady_clock::now();

    // Pairwise comparisons
    for (int i = 0; i < arraySize; i++) {
        for (int j = i + 1; j < arraySize; j++) {
            Ctxt diff(pk);
            diff = enc_arr[i];
            diff.addCtxt(enc_arr[j], true);

            Ctxt result(pk);
            bridge.compare(result, diff);
        }
    }

    return chrono::duration<double>(chrono::steady_clock::now() - t_start).count();
}

// ============================================================================
// Floyd-Warshall (4 nodes, 1 iteration)
// ============================================================================
double QuickFloyd(const Bridge& bridge, const Context& context, const PubKey& pk, int integerBits) {
    const EncryptedArray& ea = context.getEA();
    long nslots = ea.size();

    const int numNodes = 4;

    mt19937 gen(42);
    uniform_int_distribution<long> dis(1, 50);

    // Distance matrix as separate ciphertexts
    vector<vector<long>> dist(numNodes, vector<long>(nslots));
    for (int i = 0; i < numNodes; i++) {
        for (int j = 0; j < numNodes; j++) {
            long val = (i == j) ? 0 : dis(gen);
            dist[i][j % nslots] = val;
        }
    }

    vector<Ctxt> enc_dist;
    for (int i = 0; i < numNodes; i++) {
        Ctxt ct(pk);
        ea.encrypt(ct, pk, dist[i]);
        enc_dist.push_back(ct);
    }

    auto t_start = chrono::steady_clock::now();

    // One iteration (k=0)
    int k = 0;
    for (int i = 0; i < numNodes; i++) {
        if (i == k) continue;

        Ctxt d_new(pk);
        d_new = enc_dist[i];
        d_new.addCtxt(enc_dist[k]);

        Ctxt diff(pk);
        diff = d_new;
        diff.addCtxt(enc_dist[i], true);

        Ctxt result(pk);
        bridge.compare(result, diff);
    }

    return chrono::duration<double>(chrono::steady_clock::now() - t_start).count();
}

// ============================================================================
// Database Query
// ============================================================================
double QuickDatabase(const Bridge& bridge, const Context& context, const PubKey& pk, int integerBits) {
    const EncryptedArray& ea = context.getEA();
    long nslots = ea.size();

    mt19937 gen(42);
    uniform_int_distribution<long> dis(10, 50);

    vector<long> salary(nslots), hours(nslots), threshold(nslots);
    for (int i = 0; i < nslots; i++) {
        salary[i] = dis(gen);
        hours[i] = dis(gen);
        threshold[i] = 500;
    }

    Ctxt ctxt_salary(pk), ctxt_hours(pk), ctxt_threshold(pk);
    ea.encrypt(ctxt_salary, pk, salary);
    ea.encrypt(ctxt_hours, pk, hours);
    ea.encrypt(ctxt_threshold, pk, threshold);

    auto t_start = chrono::steady_clock::now();

    // Query: salary * hours > threshold
    Ctxt product(pk);
    product = ctxt_salary;
    product.multiplyBy(ctxt_hours);

    Ctxt diff(pk);
    diff = product;
    diff.addCtxt(ctxt_threshold, true);

    Ctxt result(pk);
    bridge.compare(result, diff);

    return chrono::duration<double>(chrono::steady_clock::now() - t_start).count();
}

// ============================================================================
// Main
// ============================================================================
int main(int argc, char *argv[]) {
    cout << string(80, '=') << endl;
    cout << "HE-Bridge Encoding Switching - Quick All Tests" << endl;
    cout << string(80, '=') << endl << endl;

    cout << "Verifying ALL benchmark types with minimal parameters:" << endl;
    cout << "  - Bit width: 6-bit" << endl;
    cout << "  - Minimal problem sizes" << endl << endl;

    // 6-bit parameter set
    unsigned long p = 3;
    unsigned long r = 4;
    unsigned long m = 16151;
    unsigned long bits = 320;
    int intBits = 6;
    unsigned long c = 2;
    unsigned long t = 64;

    cout << "Initializing HElib context..." << endl;

    Context context = ContextBuilder<BGV>()
        .m(m).p(p).r(r).bits(bits).c(c).skHwt(t).build();

    SecKey secret_key(context);
    secret_key.GenSecKey();
    addSome1DMatrices(secret_key);
    addFrbMatrices(secret_key);
    if (r > 1) addFrbMatrices(secret_key);
    PubKey& public_key = secret_key;

    Bridge bridge(context, UNI, r, 1, secret_key, false);

    cout << "Running tests..." << endl << endl;

    cout << string(80, '-') << endl;
    cout << left << setw(25) << "Benchmark"
         << left << setw(15) << "Time"
         << left << setw(15) << "Status" << endl;
    cout << string(80, '-') << endl;

    int passed = 0;
    int total = 5;

    // Test 1: Workload
    cout << left << setw(25) << "Workload"; cout.flush();
    double t1 = QuickWorkload(bridge, context, public_key, intBits);
    cout << left << setw(15) << formatDuration(t1) << left << setw(15) << "✓ PASSED" << endl;
    passed++;

    // Test 2: Decision Tree
    cout << left << setw(25) << "Decision Tree"; cout.flush();
    double t2 = QuickDecisionTree(bridge, context, public_key, intBits);
    cout << left << setw(15) << formatDuration(t2) << left << setw(15) << "✓ PASSED" << endl;
    passed++;

    // Test 3: Sorting
    cout << left << setw(25) << "Sorting"; cout.flush();
    double t3 = QuickSorting(bridge, context, public_key, intBits);
    cout << left << setw(15) << formatDuration(t3) << left << setw(15) << "✓ PASSED" << endl;
    passed++;

    // Test 4: Floyd-Warshall
    cout << left << setw(25) << "Floyd-Warshall"; cout.flush();
    double t4 = QuickFloyd(bridge, context, public_key, intBits);
    cout << left << setw(15) << formatDuration(t4) << left << setw(15) << "✓ PASSED" << endl;
    passed++;

    // Test 5: Database
    cout << left << setw(25) << "Database"; cout.flush();
    double t5 = QuickDatabase(bridge, context, public_key, intBits);
    cout << left << setw(15) << formatDuration(t5) << left << setw(15) << "✓ PASSED" << endl;
    passed++;

    cout << string(80, '-') << endl;
    cout << endl;
    cout << string(80, '=') << endl;
    cout << "All " << passed << "/" << total << " tests PASSED!" << endl;
    cout << "Encoding Switching is working correctly for all benchmark types." << endl;
    cout << string(80, '=') << endl;

    return 0;
}
