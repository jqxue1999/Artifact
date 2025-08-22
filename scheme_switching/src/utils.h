#pragma once

#include "openfhe.h"
#include "binfhecontext.h"
#include <vector>
#include <memory>

using namespace std;
using namespace lbcrypto;

// Extern globals shared across translation units
extern CryptoContext<DCRTPoly> g_cc;
extern KeyPair<DCRTPoly> g_keys;
extern std::shared_ptr<BinFHEContext> g_ccLWE;
extern LWEPrivateKey g_privateKeyFHEW;
extern uint32_t g_numValues;
extern uint32_t g_integerBits;

// APIs
void SetupCryptoContext(uint32_t depth, uint32_t numValues, uint32_t integerBits);
vector<LWECiphertext> Comparison(Ciphertext<DCRTPoly>& a, Ciphertext<DCRTPoly>& b);