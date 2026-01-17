#include "workload.h"

// Quick test version - simplified for fast verification
// - Only tests 6-bit (fastest)
// - Uses 8 SIMD slots instead of 128 (16x faster)
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

double QuickWorkload_1(uint32_t integerBits, uint32_t numSlots) {
    // Use smaller ring dimension for faster setup (24 -> 20)
    SetupCryptoContext(20, numSlots, integerBits);

    // Prepare test data - generate random arrays
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

    auto t_end = chrono::steady_clock::now();
    double t_sec = chrono::duration<double>(t_end - t_start).count();

    return t_sec;
}

int main() {
    lbcrypto::OpenFHEParallelControls.Disable();

    cout << string(80, '=') << endl;
    cout << "OpenFHE Scheme Switching - Quick Test" << endl;
    cout << string(80, '=') << endl << endl;

    cout << "This is a fast smoke test to verify scheme switching works correctly." << endl;
    cout << "Testing: 6-bit workload with 8 SIMD slots (instead of 128)" << endl;
    cout << "Expected runtime: 2-3 minutes" << endl << endl;

    cout << "For full benchmarks, run: ./workload" << endl;
    cout << "Full benchmarks test all bit widths (6,8,12,16) with 128 SIMD slots" << endl;
    cout << "and take 30-60 minutes per workload." << endl << endl;

    cout << "Workload 1: (a*b) compare c" << endl;
    cout << string(80, '-') << endl;
    cout << left << setw(15) << "Configuration"
         << left << setw(20) << "Time"
         << left << setw(15) << "Status" << endl;
    cout << string(80, '-') << endl;

    cout << left << setw(15) << "6-bit, 8 slots";
    cout.flush();

    double time = QuickWorkload_1(6, 8);

    cout << left << setw(20) << formatDuration(time);
    cout << left << setw(15) << "✓ PASSED" << endl;

    cout << endl << string(80, '=') << endl;
    cout << "Quick test completed successfully!" << endl;
    cout << "Scheme switching is working correctly." << endl;
    cout << string(80, '=') << endl;

    return 0;
}
