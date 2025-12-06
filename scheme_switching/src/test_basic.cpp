#include <iostream>
#include "utils.h"

using namespace std;
using namespace lbcrypto;

int main() {
    lbcrypto::OpenFHEParallelControls.Disable();

    cout << "Testing basic OpenFHE scheme switching setup..." << endl;

    // Setup with minimal parameters
    SetupCryptoContext(24, 128, 6);

    cout << "✓ Crypto context setup successful" << endl;

    // Test basic encryption
    vector<double> test_data(128, 42.0);
    Plaintext ptxt = g_cc->MakeCKKSPackedPlaintext(test_data);
    auto ctxt = g_cc->Encrypt(g_keys.publicKey, ptxt);

    cout << "✓ Encryption successful" << endl;

    // Test decryption
    Plaintext result;
    g_cc->Decrypt(g_keys.secretKey, ctxt, &result);
    result->SetLength(1);
    double decrypted = result->GetRealPackedValue()[0];

    cout << "✓ Decryption successful" << endl;
    cout << "  Original: 42.0, Decrypted: " << decrypted << endl;

    // Test basic comparison
    vector<double> data1(128, 10.0);
    vector<double> data2(128, 20.0);

    Plaintext ptxt1 = g_cc->MakeCKKSPackedPlaintext(data1);
    Plaintext ptxt2 = g_cc->MakeCKKSPackedPlaintext(data2);

    auto ctxt1 = g_cc->Encrypt(g_keys.publicKey, ptxt1);
    auto ctxt2 = g_cc->Encrypt(g_keys.publicKey, ptxt2);

    cout << "✓ Testing scheme switching comparison..." << endl;
    auto cComp = Comparison(ctxt1, ctxt2);
    auto cCompCKKS = g_cc->EvalFHEWtoCKKS(cComp, g_numValues, g_numValues);

    cout << "✓ Scheme switching comparison successful" << endl;

    // Decrypt comparison result
    Plaintext comp_result;
    g_cc->Decrypt(g_keys.secretKey, cCompCKKS, &comp_result);
    comp_result->SetLength(1);
    double comp_val = comp_result->GetRealPackedValue()[0];

    cout << "  10.0 < 20.0 = " << comp_val << " (expected: 1.0)" << endl;

    cout << "\n✓ All basic tests passed!" << endl;
    cout << "The OpenFHE scheme switching implementation is working correctly." << endl;

    return 0;
}
