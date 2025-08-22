#include "workload.h"


double Workload_3(uint32_t integerBits) {
    cout << "Integer Bits: " << integerBits << endl;   
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
    auto t_mult_start = chrono::steady_clock::now();
    auto cMult1 = g_cc->EvalMult(c1, c2);
    auto cMult2 = g_cc->EvalMult(c3, c4);
    cMult1 = g_cc->Rescale(cMult1);
    cMult2 = g_cc->Rescale(cMult2);
    auto t_mult_end = chrono::steady_clock::now();
    double t_mult_sec = chrono::duration<double>(t_mult_end - t_mult_start).count();
    cout << "CKKS multiplication time: " << t_mult_sec << " s" << endl;

    // Comparison CKKS - FHEW
    auto cResult = Comparison(cMult1, cMult2);

    // FHEW to CKKS
    auto t_fhew2ckks_start = chrono::steady_clock::now();
    auto cSignResult = g_cc->EvalFHEWtoCKKS(cResult, g_numValues, g_numValues);
    auto t_fhew2ckks_end = chrono::steady_clock::now();
    double t_fhew2ckks_sec = chrono::duration<double>(t_fhew2ckks_end - t_fhew2ckks_start).count();
    cout << "FHEW -> CKKS switching time: " << t_fhew2ckks_sec << " s" << endl;

    auto t_end = chrono::steady_clock::now();
    double t_sec = chrono::duration<double>(t_end - t_start).count();
    cout << "Total time: " << t_sec << " s" << endl;
    return t_sec;
}

double Workload_2(uint32_t integerBits) {
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
    auto t_mult_start = chrono::steady_clock::now();
    auto cMult = g_cc->EvalMult(c1, c2);
    cMult = g_cc->Rescale(cMult);
    auto t_mult_end = chrono::steady_clock::now();
    double t_mult_sec = chrono::duration<double>(t_mult_end - t_mult_start).count();
    cout << "CKKS multiplication time: " << t_mult_sec << " s" << endl;

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
    cout << "Total time: " << t_sec << " s" << endl;
    return t_sec;
}

double Workload_1(uint32_t integerBits) {
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
    auto t_mult_start = chrono::steady_clock::now();
    auto cMult = g_cc->EvalMult(c1, c2);
    cMult = g_cc->Rescale(cMult);
    auto t_mult_end = chrono::steady_clock::now();
    double t_mult_sec = chrono::duration<double>(t_mult_end - t_mult_start).count();
    cout << "CKKS multiplication time: " << t_mult_sec << " s" << endl;

    // Comparison CKKS - FHEW
    auto cResult = Comparison(cMult, c3);
    auto t_end = chrono::steady_clock::now();
    double t_sec = chrono::duration<double>(t_end - t_start).count();
    cout << "Total time: " << t_sec << " s" << endl;
    return t_sec;
}

int main() {
    lbcrypto::OpenFHEParallelControls.Disable();

    cout << "========== Workload 1 ==========" << endl;
    Workload_1(6);
    Workload_1(8);
    Workload_1(12);
    Workload_1(16);

    cout << "========== Workload 2 ==========" << endl;
    Workload_2(6);
    Workload_2(8);
    Workload_2(12);
    Workload_2(16);

    cout << "========== Workload 3 ==========" << endl;
    Workload_3(6);
    Workload_3(8);
    Workload_3(12);
    Workload_3(16);

    return 0;
}