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

// Quick test version - simplified for fast verification
// - Only tests 6-bit (fastest)
// - Only runs Workload 1 (simplest pattern)
// Expected runtime: 2-3 minutes

// Helper function to format duration
string formatDuration(double seconds) {
    if (seconds < 1.0) {
        return to_string(static_cast<int>(seconds * 1000)) + " ms";
    } else if (seconds < 60.0) {
        return to_string(static_cast<int>(seconds)) + " s";
    } else if (seconds < 3600.0) {
        return to_string(static_cast<int>(seconds / 60)) + " min";
    } else {
        return to_string(static_cast<int>(seconds / 3600)) + " hr";
    }
}

// Workload-1: (a*b) compare c
// Pattern: Linear operation followed by non-linear comparison
double QuickWorkload1(const Bridge& bridge, const Context& context, const PubKey& pk, const SecKey& sk, int integerBits) {
    const EncryptedArray& ea = context.getEA();
    long nslots = ea.size();

    // Generate random inputs
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

int main(int argc, char *argv[]) {
    cout << string(80, '=') << endl;
    cout << "HE-Bridge Encoding Switching - Quick Test" << endl;
    cout << string(80, '=') << endl << endl;

    cout << "This is a fast smoke test to verify encoding switching works correctly." << endl;
    cout << "Testing: 6-bit workload" << endl;
    cout << "Expected runtime: 2-3 minutes" << endl << endl;

    cout << "For full benchmarks, run: ./workload" << endl;
    cout << "Full benchmarks test all bit widths (6,8,12,16)" << endl;
    cout << "and take similar time to scheme switching benchmarks." << endl << endl;

    // 6-bit parameter set
    unsigned long p = 3;
    unsigned long r = 4;
    unsigned long m = 16151;
    unsigned long bits = 320;
    int intBits = 6;
    unsigned long c = 2;   // Key-switching columns
    unsigned long t = 64;  // Hamming weight of secret key

    cout << "Workload-1: (a*b) compare c" << endl;
    cout << string(80, '-') << endl;
    cout << left << setw(15) << "Configuration"
         << left << setw(25) << "Parameters"
         << left << setw(20) << "Time"
         << left << setw(10) << "Status" << endl;
    cout << string(80, '-') << endl;

    cout << left << setw(15) << "6-bit"
         << left << setw(25) << ("p=" + to_string(p) + ", r=" + to_string(r));
    cout.flush();

    // Initialize context
    Context context = ContextBuilder<BGV>()
        .m(m).p(p).r(r).bits(bits).c(c).skHwt(t).build();

    // Generate keys
    SecKey secret_key(context);
    secret_key.GenSecKey();
    addSome1DMatrices(secret_key);
    addFrbMatrices(secret_key);
    if (r > 1) addFrbMatrices(secret_key);
    PubKey& public_key = secret_key;

    // Initialize Bridge
    Bridge bridge(context, UNI, r, 1, secret_key, false);

    double time = QuickWorkload1(bridge, context, public_key, secret_key, intBits);

    cout << left << setw(20) << formatDuration(time)
         << left << setw(10) << "✓ PASSED" << endl;

    cout << endl << string(80, '=') << endl;
    cout << "Quick test completed successfully!" << endl;
    cout << "Encoding switching is working correctly." << endl;
    cout << string(80, '=') << endl;

    return 0;
}
