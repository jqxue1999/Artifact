#include "utils.h"


// Define globals declared in utils.h
CryptoContext<DCRTPoly> g_cc;
KeyPair<DCRTPoly> g_keys;
std::shared_ptr<BinFHEContext> g_ccLWE;
LWEPrivateKey g_privateKeyFHEW;
uint32_t g_numValues;
uint32_t g_integerBits;

// Setup function to initialize crypto context and keys
void SetupCryptoContext(uint32_t depth, uint32_t numValues, uint32_t integerBits) {
    // CKKS parameters for different integer bit-lengths
    uint32_t scaleModSize = 40;  // Reduced from 50 to stay within OpenFHE limits
    uint32_t firstModSize = scaleModSize + integerBits;  // Dynamic based on integer bits
    
    // Dynamically set logQ_ccLWE and depth based on integerBits
    uint32_t logQ_ccLWE;
    switch (integerBits) {
        case 6:  logQ_ccLWE = 15; break;
        case 8:  logQ_ccLWE = 17; break;
        case 12: logQ_ccLWE = 21; break;
        case 16: logQ_ccLWE = 25; break;
        default: logQ_ccLWE = 25; break; // fallback to largest value
    }

    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(depth);
    parameters.SetScalingModSize(scaleModSize);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetBatchSize(numValues);
    parameters.SetSecurityLevel(HEStd_128_classic);
    // parameters.SetSecurityLevel(HEStd_NotSet);
    // parameters.SetRingDim(8192);

    g_cc = GenCryptoContext(parameters);

    // Enable required features
    g_cc->Enable(PKE);
    g_cc->Enable(KEYSWITCH);
    g_cc->Enable(LEVELEDSHE);
    g_cc->Enable(ADVANCEDSHE);
    g_cc->Enable(SCHEMESWITCH);

    // Generate keys
    g_keys = g_cc->KeyGen();

    // Setup scheme switching
    SchSwchParams params;
    params.SetSecurityLevelCKKS(HEStd_128_classic);
    params.SetSecurityLevelFHEW(STD128);
    // params.SetSecurityLevelCKKS(HEStd_NotSet);
    // params.SetSecurityLevelFHEW(TOY);
    params.SetCtxtModSizeFHEWLargePrec(logQ_ccLWE);
    params.SetNumSlotsCKKS(numValues);
    params.SetNumValues(numValues);
    
    g_privateKeyFHEW = g_cc->EvalSchemeSwitchingSetup(params);
    g_cc->EvalSchemeSwitchingKeyGen(g_keys, g_privateKeyFHEW);
    
    g_ccLWE = g_cc->GetBinCCForSchemeSwitch();
    
    // Setup for FHEW to CKKS switching
    g_cc->EvalFHEWtoCKKSSetup(g_ccLWE, numValues, logQ_ccLWE);
    g_cc->EvalFHEWtoCKKSKeyGen(g_keys, g_privateKeyFHEW);
    
    // Precompute for comparison
    auto modulus_LWE = 1 << logQ_ccLWE;
    auto beta        = g_ccLWE->GetBeta().ConvertToInt();
    auto pLWE        = modulus_LWE / (2 * beta);
    double scaleSignFHEW = 1.0;

    g_cc->EvalCompareSwitchPrecompute(pLWE, scaleSignFHEW);

    g_numValues = numValues;
    g_integerBits = integerBits;
}

// Common function for CKKS difference, CKKS to FHEW switching, and FHEW sign
vector<LWECiphertext> Comparison(Ciphertext<DCRTPoly>& a, Ciphertext<DCRTPoly>& b) {
    // Difference on CKKS
    auto cDiff = g_cc->EvalSub(a, b);

    // CKKS to FHEW
    auto LWECiphertexts = g_cc->EvalCKKStoFHEW(cDiff, g_numValues);

    // Sign on FHEW
    vector<LWECiphertext> LWESign(LWECiphertexts.size());
    for (uint32_t i = 0; i < LWECiphertexts.size(); ++i) {
        LWESign[i] = g_ccLWE->EvalSign(LWECiphertexts[i]);
    }

    return LWESign;
}