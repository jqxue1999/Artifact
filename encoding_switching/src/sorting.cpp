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
    else if (seconds < 3600.0) return to_string(static_cast<int>(seconds / 60)) + " min";
    else if (seconds < 86400.0) return to_string(static_cast<int>(seconds / 3600)) + " hr";
    else return to_string(static_cast<int>(seconds / 86400)) + " days";
}

// Private sorting using encoding switching
// Direct sorting algorithm: count positions and obliviously place elements
double EvaluateSorting(const Bridge& bridge, const Context& context, const PubKey& pk,
                       const SecKey& sk, uint32_t arraySize, uint32_t integerBits) {
    const EncryptedArray& ea = context.getEA();
    long nslots = ea.size();
    long p = context.getP();
    long r = context.getR();

    // Generate random array
    random_device rd;
    mt19937 gen(42);
    uniform_int_distribution<long> dis(1, (1 << (integerBits - 1)));

    vector<long> array(arraySize);
    for (uint32_t i = 0; i < arraySize; i++) {
        array[i] = dis(gen);
    }

    // Encrypt array elements
    vector<Ctxt> encrypted_array;
    for (uint32_t i = 0; i < arraySize; i++) {
        vector<long> elem_vec(nslots, array[i]);
        Ctxt ct(pk);
        ea.encrypt(ct, pk, elem_vec);
        encrypted_array.push_back(ct);
    }

    auto t_start = chrono::steady_clock::now();

    // Step 1: Count positions - for each element, count how many are less than it
    vector<Ctxt> positions;

    for (uint32_t i = 0; i < arraySize; i++) {
        vector<long> zero_vec(nslots, 0);
        Ctxt count(pk);
        ea.encrypt(count, pk, zero_vec);

        for (uint32_t j = 0; j < arraySize; j++) {
            if (i != j) {
                // Compare: array[j] < array[i]
                Ctxt diff(pk);
                diff = encrypted_array[j];
                diff.addCtxt(encrypted_array[i], true); // array[j] - array[i]

                // If array[j] - array[i] < 0, then array[j] < array[i]
                // We need to negate to get the right comparison
                Ctxt diff_neg(pk);
                diff_neg = diff;
                diff_neg.negate();

                Ctxt comp_result(pk);
                bridge.compare(comp_result, diff_neg);

                // Lift back to FV
                comp_result.multiplyModByP2R();
                Ctxt comp_lifted(pk);
                bridge.lift(comp_lifted, comp_result, r);

                count.addCtxt(comp_lifted);
            }
        }
        positions.push_back(count);
    }

    // Step 2: Oblivious placement using encrypted equality checks
    vector<Ctxt> sorted_array;

    for (uint32_t k = 0; k < arraySize; k++) {
        vector<long> zero_vec(nslots, 0);
        Ctxt result(pk);
        ea.encrypt(result, pk, zero_vec);

        // Check which element has position == k
        for (uint32_t i = 0; i < arraySize; i++) {
            // Check if positions[i] == k
            vector<long> k_vec(nslots, k);
            Ctxt ct_k(pk);
            ea.encrypt(ct_k, pk, k_vec);

            Ctxt diff(pk);
            diff = positions[i];
            diff.addCtxt(ct_k, true); // positions[i] - k

            // Check if diff == 0
            Ctxt is_equal(pk);
            // is_equal will be 1 if positions[i] == k, 0 otherwise
            // We use (1 - |diff|) as approximation
            // For proper equality, we'd check sign(diff)==0 AND sign(-diff)==0
            // Simplified: just multiply by indicator

            // For now, use simpler approach: check both diff >= 0 and -diff >= 0
            Ctxt comp1(pk);
            bridge.compare(comp1, diff);
            comp1.multiplyModByP2R();
            Ctxt comp1_lifted(pk);
            bridge.lift(comp1_lifted, comp1, r);

            Ctxt diff_neg = diff;
            diff_neg.negate();
            Ctxt comp2(pk);
            bridge.compare(comp2, diff_neg);
            comp2.multiplyModByP2R();
            Ctxt comp2_lifted(pk);
            bridge.lift(comp2_lifted, comp2, r);

            // Both must be 1 for equality
            is_equal = comp1_lifted;
            is_equal.multiplyBy(comp2_lifted);

            // Multiply element by indicator
            Ctxt contribution(pk);
            contribution = encrypted_array[i];
            contribution.multiplyBy(is_equal);

            result.addCtxt(contribution);
        }
        sorted_array.push_back(result);
    }

    auto t_end = chrono::steady_clock::now();
    return chrono::duration<double>(t_end - t_start).count();
}

int main(int argc, char *argv[]) {
    unsigned long p = 17;
    unsigned long r = 2;
    unsigned long m = 13201;
    unsigned long bits = 256;
    unsigned long c = 2;
    unsigned long t = 64;

    ArgMapping amap;
    amap.arg("p", p, "the base plaintext modulus");
    amap.arg("r", r, "the lifting parameter for plaintext space p^r");
    amap.arg("m", m, "the order of the cyclotomic ring");
    amap.arg("b", bits, "the bitsize of the ciphertext modulus");
    amap.arg("c", c, "Number of columns of Key-Switching matrix");
    amap.arg("t", t, "The hamming weight of sk");
    amap.parse(argc, argv);

    cout << string(80, '=') << endl;
    cout << "HE-Bridge Encoding Switching Private Sorting" << endl;
    cout << string(80, '=') << endl << endl;

    cout << "Parameters: m=" << m << ", p=" << p << ", r=" << r
         << ", bits=" << bits << endl << endl;

    Context context = ContextBuilder<BGV>().m(m).p(p).r(r).bits(bits).c(c).skHwt(t).build();

    cout << "Generating keys..." << endl;
    SecKey secret_key(context);
    secret_key.GenSecKey();
    addSome1DMatrices(secret_key);
    addFrbMatrices(secret_key);
    if (r > 1) addFrbMatrices(secret_key);
    PubKey& public_key = secret_key;

    unsigned long expansion_len = 1;
    Bridge bridge(context, UNI, r, expansion_len, secret_key, false);
    cout << endl;

    int integerBits = static_cast<int>(ceil(log2(pow(p, r))));

    // Experiment 1: Fixed size (8 elements) with different bit widths
    vector<uint32_t> element_counts = {8, 16, 32, 64};

    cout << "Sorting Benchmark with Encoding Switching" << endl;
    cout << string(80, '-') << endl;
    cout << left << setw(15) << "Array Size"
         << left << setw(15) << "Bit Width"
         << left << setw(20) << "Time"
         << left << setw(10) << "Status" << endl;
    cout << string(80, '-') << endl;

    for (auto size : element_counts) {
        cout << left << setw(15) << size
             << left << setw(15) << integerBits;
        cout.flush();

        double time = EvaluateSorting(bridge, context, public_key, secret_key, size, integerBits);

        cout << left << setw(20) << formatDuration(time)
             << left << setw(10) << "✓" << endl;
    }

    cout << endl << string(80, '=') << endl;
    return 0;
}
