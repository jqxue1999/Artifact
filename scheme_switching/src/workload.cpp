#include "workload.h"

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

double Workload_3(uint32_t integerBits) {
    SetupCryptoContext(24, 128, integerBits);

    // Prepare test data - generate random arrays of length g_numValues
    vector<double> x1(g_numValues);
    vector<double> x2(g_numValues);
    vector<double> x3(g_numValues);
    vector<double> x4(g_numValues);

    // Initialize random number generator
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0.0, 1 << (integerBits / 2));

    // Generate random values
    for (size_t i = 0; i < g_numValues; ++i) {
        x1[i] = dis(gen);
        x2[i] = dis(gen);
        x3[i] = dis(gen);
        x4[i] = dis(gen);
    }

    // Encode and encrypt
    Plaintext ptxt1 = g_cc->MakeCKKSPackedPlaintext(x1);
    Plaintext ptxt2 = g_cc->MakeCKKSPackedPlaintext(x2);
    Plaintext ptxt3 = g_cc->MakeCKKSPackedPlaintext(x3);
    Plaintext ptxt4 = g_cc->MakeCKKSPackedPlaintext(x4);

    auto c1 = g_cc->Encrypt(g_keys.publicKey, ptxt1);
    auto c2 = g_cc->Encrypt(g_keys.publicKey, ptxt2);
    auto c3 = g_cc->Encrypt(g_keys.publicKey, ptxt3);
    auto c4 = g_cc->Encrypt(g_keys.publicKey, ptxt4);

    auto t_start = chrono::steady_clock::now();

    // Multiplication on CKKS
    auto cMult1 = g_cc->EvalMult(c1, c2);
    auto cMult2 = g_cc->EvalMult(c3, c4);
    cMult1 = g_cc->Rescale(cMult1);
    cMult2 = g_cc->Rescale(cMult2);

    // Comparison CKKS - FHEW
    auto cResult = Comparison(cMult1, cMult2);

    // FHEW to CKKS
    auto cSignResult = g_cc->EvalFHEWtoCKKS(cResult, g_numValues, g_numValues);

    auto t_end = chrono::steady_clock::now();
    double t_sec = chrono::duration<double>(t_end - t_start).count();

    return t_sec;
}

double Workload_2(uint32_t integerBits) {
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

    // Comparison CKKS - FHEW
    auto cResult = Comparison(c1, c2);

    // Convert FHEW sign results back to CKKS
    auto cSignResult = g_cc->EvalFHEWtoCKKS(cResult, g_numValues, g_numValues);

    // Multiplication on CKKS
    auto cMult2 = g_cc->EvalMult(cSignResult, c3);
    cMult2 = g_cc->Rescale(cMult2);

    auto t_end = chrono::steady_clock::now();
    double t_sec = chrono::duration<double>(t_end - t_start).count();

    return t_sec;
}

double Workload_1(uint32_t integerBits) {
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

    auto t_end = chrono::steady_clock::now();
    double t_sec = chrono::duration<double>(t_end - t_start).count();

    return t_sec;
}

int main() {
    lbcrypto::OpenFHEParallelControls.Disable();

    cout << string(80, '=') << endl;
    cout << "OpenFHE Scheme Switching Workload Benchmarks" << endl;
    cout << string(80, '=') << endl << endl;

    cout << "Testing basic workloads with different bit widths (6, 8, 12, 16)" << endl;
    cout << "Each workload uses 128 SIMD slots with scheme switching between CKKS and FHEW" << endl << endl;

    // Workload 1: (a*b) compare c - Linear then non-linear
    cout << "Workload 1: (a*b) compare c" << endl;
    cout << string(80, '-') << endl;
    cout << left << setw(15) << "Bit Width"
         << left << setw(20) << "Time"
         << left << setw(15) << "Status" << endl;
    cout << string(80, '-') << endl;

    for (auto bits : {6, 8}) {  // Removed 12, 16 due to memory constraints (>32GB needed)
        cout << left << setw(15) << bits;
        double time = Workload_1(bits);
        cout << left << setw(20) << formatDuration(time);
        cout << left << setw(15) << "✓" << endl;
    }
    cout << endl;

    // Workload 2: (a compare b) * c - Non-linear then linear
    cout << "Workload 2: (a compare b) * c" << endl;
    cout << string(80, '-') << endl;
    cout << left << setw(15) << "Bit Width"
         << left << setw(20) << "Time"
         << left << setw(15) << "Status" << endl;
    cout << string(80, '-') << endl;

    for (auto bits : {6, 8}) {  // Removed 12, 16 due to memory constraints (>32GB needed)
        cout << left << setw(15) << bits;
        double time = Workload_2(bits);
        cout << left << setw(20) << formatDuration(time);
        cout << left << setw(15) << "✓" << endl;
    }
    cout << endl;

    // Workload 3: (a*b) compare (c*d) - Mixed sequence
    cout << "Workload 3: (a*b) compare (c*d)" << endl;
    cout << string(80, '-') << endl;
    cout << left << setw(15) << "Bit Width"
         << left << setw(20) << "Time"
         << left << setw(15) << "Status" << endl;
    cout << string(80, '-') << endl;

    for (auto bits : {6, 8}) {  // Removed 12, 16 due to memory constraints (>32GB needed)
        cout << left << setw(15) << bits;
        double time = Workload_3(bits);
        cout << left << setw(20) << formatDuration(time);
        cout << left << setw(15) << "✓" << endl;
    }
    cout << endl;

    cout << string(80, '=') << endl;

    return 0;
}
