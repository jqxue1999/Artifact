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

// Workload-1: (a*b) compare c
// Pattern: Linear operation followed by non-linear comparison
// Common in: Database queries, range checks
double Workload1(const Bridge& bridge, const Context& context, const PubKey& pk, const SecKey& sk, int integerBits) {
    const EncryptedArray& ea = context.getEA();
    long nslots = ea.size();
    long p2r = context.getPPowR();

    // Generate random inputs
    random_device rd;
    mt19937 gen(42);
    uniform_int_distribution<long> dis(1, (1 << (integerBits - 1)));

    vector<long> a_vec(nslots);
    vector<long> b_vec(nslots);
    vector<long> c_vec(nslots);

    for (int i = 0; i < nslots; i++) {
        a_vec[i] = dis(gen);
        b_vec[i] = dis(gen);
        c_vec[i] = dis(gen);
    }

    // Encrypt inputs
    Ctxt ctxt_a(pk);
    Ctxt ctxt_b(pk);
    Ctxt ctxt_c(pk);
    ea.encrypt(ctxt_a, pk, a_vec);
    ea.encrypt(ctxt_b, pk, b_vec);
    ea.encrypt(ctxt_c, pk, c_vec);

    auto t_start = chrono::steady_clock::now();

    // Step 1: Linear operation - multiplication a*b (in FV)
    Ctxt ctxt_product(pk);
    ctxt_product = ctxt_a;
    ctxt_product.multiplyBy(ctxt_b);

    // Step 2: Compute difference: (a*b) - c
    Ctxt ctxt_diff(pk);
    ctxt_diff = ctxt_product;
    ctxt_diff.addCtxt(ctxt_c, true); // true means subtract

    // Step 3: Non-linear operation - comparison via encoding switching
    // Compare: (a*b) > c  <==>  (a*b - c) > 0
    Ctxt ctxt_result(pk);
    bridge.compare(ctxt_result, ctxt_diff);

    auto t_end = chrono::steady_clock::now();
    return chrono::duration<double>(t_end - t_start).count();
}

// Workload-2: (a compare b) * c
// Pattern: Non-linear comparison followed by linear operation
// Common in: Decision trees, conditional computations
double Workload2(const Bridge& bridge, const Context& context, const PubKey& pk, const SecKey& sk, int integerBits) {
    const EncryptedArray& ea = context.getEA();
    long nslots = ea.size();
    long p2r = context.getPPowR();
    long p = context.getP();
    long r = context.getR();

    // Generate random inputs
    random_device rd;
    mt19937 gen(42);
    uniform_int_distribution<long> dis(1, (1 << (integerBits - 1)));

    vector<long> a_vec(nslots);
    vector<long> b_vec(nslots);
    vector<long> c_vec(nslots);

    for (int i = 0; i < nslots; i++) {
        a_vec[i] = dis(gen);
        b_vec[i] = dis(gen);
        c_vec[i] = dis(gen);
    }

    // Encrypt inputs
    Ctxt ctxt_a(pk);
    Ctxt ctxt_b(pk);
    Ctxt ctxt_c(pk);
    ea.encrypt(ctxt_a, pk, a_vec);
    ea.encrypt(ctxt_b, pk, b_vec);
    ea.encrypt(ctxt_c, pk, c_vec);

    auto t_start = chrono::steady_clock::now();

    // Step 1: Non-linear operation - comparison a > b
    Ctxt ctxt_diff(pk);
    ctxt_diff = ctxt_a;
    ctxt_diff.addCtxt(ctxt_b, true); // a - b

    Ctxt ctxt_comp_result(pk);
    bridge.compare(ctxt_comp_result, ctxt_diff); // Result in beFV (mod p)

    // Step 2: Lift comparison result back to FV
    ctxt_comp_result.multiplyModByP2R();
    Ctxt ctxt_comp_lifted(pk);
    bridge.lift(ctxt_comp_lifted, ctxt_comp_result, r);

    // Step 3: Linear operation - multiply result by c (in FV)
    ctxt_comp_lifted.multiplyBy(ctxt_c);

    auto t_end = chrono::steady_clock::now();
    return chrono::duration<double>(t_end - t_start).count();
}

// Workload-3: (a*b) compare (c*d)
// Pattern: Linear ops followed by comparison of two products
// Common in: Neural networks, complex conditionals
double Workload3(const Bridge& bridge, const Context& context, const PubKey& pk, const SecKey& sk, int integerBits) {
    const EncryptedArray& ea = context.getEA();
    long nslots = ea.size();
    long p2r = context.getPPowR();

    // Generate random inputs
    random_device rd;
    mt19937 gen(42);
    uniform_int_distribution<long> dis(1, (1 << (integerBits - 1)));

    vector<long> a_vec(nslots);
    vector<long> b_vec(nslots);
    vector<long> c_vec(nslots);
    vector<long> d_vec(nslots);

    for (int i = 0; i < nslots; i++) {
        a_vec[i] = dis(gen);
        b_vec[i] = dis(gen);
        c_vec[i] = dis(gen);
        d_vec[i] = dis(gen);
    }

    // Encrypt inputs
    Ctxt ctxt_a(pk);
    Ctxt ctxt_b(pk);
    Ctxt ctxt_c(pk);
    Ctxt ctxt_d(pk);
    ea.encrypt(ctxt_a, pk, a_vec);
    ea.encrypt(ctxt_b, pk, b_vec);
    ea.encrypt(ctxt_c, pk, c_vec);
    ea.encrypt(ctxt_d, pk, d_vec);

    auto t_start = chrono::steady_clock::now();

    // Step 1: Linear operations - two multiplications
    Ctxt ctxt_prod1(pk);
    ctxt_prod1 = ctxt_a;
    ctxt_prod1.multiplyBy(ctxt_b); // a*b

    Ctxt ctxt_prod2(pk);
    ctxt_prod2 = ctxt_c;
    ctxt_prod2.multiplyBy(ctxt_d); // c*d

    // Step 2: Compute difference
    Ctxt ctxt_diff(pk);
    ctxt_diff = ctxt_prod1;
    ctxt_diff.addCtxt(ctxt_prod2, true); // (a*b) - (c*d)

    // Step 3: Non-linear operation - comparison
    // Compare: (a*b) > (c*d)  <==>  (a*b - c*d) > 0
    Ctxt ctxt_result(pk);
    bridge.compare(ctxt_result, ctxt_diff);

    auto t_end = chrono::steady_clock::now();
    return chrono::duration<double>(t_end - t_start).count();
}

int main(int argc, char *argv[]) {
    cout << string(80, '=') << endl;
    cout << "HE-Bridge Encoding Switching Workload Benchmarks" << endl;
    cout << string(80, '=') << endl << endl;

    // Parameter configurations for different bit widths
    struct ParamSet {
        string name;
        unsigned long p, r, m, bits;
        int intBits;
    };

    vector<ParamSet> param_sets = {
        {"6-bit",  3,   4, 16151, 320,  6},
        {"8-bit",  17,  2, 13201, 256,  8}
        // 12-bit and 16-bit removed due to memory constraints (require >32GB)
        // {"12-bit", 67,  2, 31159, 690,  12},
        // {"16-bit", 257, 2, 77641, 1000, 16}
    };

    unsigned long c = 2;   // Key-switching columns
    unsigned long t = 64;  // Hamming weight of secret key

    cout << "Testing workloads with bit widths: 6, 8" << endl;
    cout << "Each configuration uses different parameters (p, r, m)" << endl << endl;

    // Workload 1
    cout << "Workload-1: (a*b) compare c" << endl;
    cout << string(80, '-') << endl;
    cout << left << setw(15) << "Bit Width"
         << left << setw(25) << "Parameters (p, r)"
         << left << setw(20) << "Time"
         << left << setw(10) << "Status" << endl;
    cout << string(80, '-') << endl;

    for (const auto& ps : param_sets) {
        cout << left << setw(15) << ps.name
             << left << setw(25) << ("p=" + to_string(ps.p) + ", r=" + to_string(ps.r));
        cout.flush();

        // Initialize context
        Context context = ContextBuilder<BGV>()
            .m(ps.m).p(ps.p).r(ps.r).bits(ps.bits).c(c).skHwt(t).build();

        // Generate keys
        SecKey secret_key(context);
        secret_key.GenSecKey();
        addSome1DMatrices(secret_key);
        addFrbMatrices(secret_key);
        if (ps.r > 1) addFrbMatrices(secret_key);
        PubKey& public_key = secret_key;

        // Initialize Bridge
        Bridge bridge(context, UNI, ps.r, 1, secret_key, false);

        double time = Workload1(bridge, context, public_key, secret_key, ps.intBits);

        cout << left << setw(20) << formatDuration(time)
             << left << setw(10) << "✓" << endl;
    }
    cout << endl;

    // Workload 2
    cout << "Workload-2: (a compare b) * c" << endl;
    cout << string(80, '-') << endl;
    cout << left << setw(15) << "Bit Width"
         << left << setw(25) << "Parameters (p, r)"
         << left << setw(20) << "Time"
         << left << setw(10) << "Status" << endl;
    cout << string(80, '-') << endl;

    for (const auto& ps : param_sets) {
        cout << left << setw(15) << ps.name
             << left << setw(25) << ("p=" + to_string(ps.p) + ", r=" + to_string(ps.r));
        cout.flush();

        Context context = ContextBuilder<BGV>()
            .m(ps.m).p(ps.p).r(ps.r).bits(ps.bits).c(c).skHwt(t).build();

        SecKey secret_key(context);
        secret_key.GenSecKey();
        addSome1DMatrices(secret_key);
        addFrbMatrices(secret_key);
        if (ps.r > 1) addFrbMatrices(secret_key);
        PubKey& public_key = secret_key;

        Bridge bridge(context, UNI, ps.r, 1, secret_key, false);

        double time = Workload2(bridge, context, public_key, secret_key, ps.intBits);
        cout << left << setw(20) << formatDuration(time)
             << left << setw(10) << "✓" << endl;
    }
    cout << endl;

    // Workload 3
    cout << "Workload-3: (a*b) compare (c*d)" << endl;
    cout << string(80, '-') << endl;
    cout << left << setw(15) << "Bit Width"
         << left << setw(25) << "Parameters (p, r)"
         << left << setw(20) << "Time"
         << left << setw(10) << "Status" << endl;
    cout << string(80, '-') << endl;

    for (const auto& ps : param_sets) {
        cout << left << setw(15) << ps.name
             << left << setw(25) << ("p=" + to_string(ps.p) + ", r=" + to_string(ps.r));
        cout.flush();

        Context context = ContextBuilder<BGV>()
            .m(ps.m).p(ps.p).r(ps.r).bits(ps.bits).c(c).skHwt(t).build();

        SecKey secret_key(context);
        secret_key.GenSecKey();
        addSome1DMatrices(secret_key);
        addFrbMatrices(secret_key);
        if (ps.r > 1) addFrbMatrices(secret_key);
        PubKey& public_key = secret_key;

        Bridge bridge(context, UNI, ps.r, 1, secret_key, false);

        double time = Workload3(bridge, context, public_key, secret_key, ps.intBits);
        cout << left << setw(20) << formatDuration(time)
             << left << setw(10) << "✓" << endl;
    }
    cout << endl;

    cout << string(80, '=') << endl;
    cout << "Note: Each bit width uses optimized parameters for that precision" << endl;
    cout << "      6-bit:  p=3,   r=4 (plaintext space = 81)" << endl;
    cout << "      8-bit:  p=17,  r=2 (plaintext space = 289)" << endl;
    cout << "      12-bit: p=67,  r=2 (plaintext space = 4489)" << endl;
    cout << "      16-bit: p=257, r=2 (plaintext space = 66049)" << endl;

    return 0;
}
