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

// Floyd-Warshall all-pairs shortest path on encrypted graph
double EvaluateFloydWarshall(const Bridge& bridge, const Context& context, const PubKey& pk,
                             const SecKey& sk, uint32_t numNodes, uint32_t integerBits) {
    const EncryptedArray& ea = context.getEA();
    long nslots = ea.size();
    long p = context.getP();
    long r = context.getR();

    // Generate random weighted graph
    random_device rd;
    mt19937 gen(42);
    uniform_int_distribution<long> dis(1, (1 << (integerBits - 2)));
    long inf_value = (1 << (integerBits - 1)) - 1;

    vector<vector<long>> graph(numNodes, vector<long>(numNodes, inf_value));

    // Initialize diagonal to 0
    for (uint32_t i = 0; i < numNodes; i++) {
        graph[i][i] = 0;
    }

    // Add random edges
    for (uint32_t i = 0; i < numNodes; i++) {
        for (uint32_t j = 0; j < numNodes; j++) {
            if (i != j && gen() % 3 == 0) { // 33% edge density
                graph[i][j] = dis(gen);
            }
        }
    }

    // Encrypt distance matrix
    vector<vector<Ctxt>> enc_dist(numNodes);
    for (uint32_t i = 0; i < numNodes; i++) {
        for (uint32_t j = 0; j < numNodes; j++) {
            vector<long> dist_vec(nslots, graph[i][j]);
            Ctxt ct(pk);
            ea.encrypt(ct, pk, dist_vec);
            enc_dist[i].push_back(ct);
        }
    }

    auto t_start = chrono::steady_clock::now();

    // Floyd-Warshall algorithm
    for (uint32_t k = 0; k < numNodes; k++) {
        for (uint32_t i = 0; i < numNodes; i++) {
            for (uint32_t j = 0; j < numNodes; j++) {
                // Compute new distance: d[i][k] + d[k][j]
                Ctxt d_new(pk);
                d_new = enc_dist[i][k];
                d_new.addCtxt(enc_dist[k][j]);

                // Compare: d_new < d[i][j]
                Ctxt diff(pk);
                diff = d_new;
                diff.addCtxt(enc_dist[i][j], true); // d_new - d[i][j]

                Ctxt comp_result(pk);
                bridge.compare(comp_result, diff);

                // Lift back to FV
                comp_result.multiplyModByP2R();
                Ctxt comp_lifted(pk);
                bridge.lift(comp_lifted, comp_result, r);

                // Oblivious selection: result = comp ? d_new : d[i][j]
                // result = comp * d_new + (1-comp) * d[i][j]
                Ctxt term1(pk);
                term1 = comp_lifted;
                term1.multiplyBy(d_new);

                vector<long> ones_vec(nslots, 1);
                Ctxt one(pk);
                ea.encrypt(one, pk, ones_vec);

                Ctxt inv_comp(pk);
                inv_comp = one;
                inv_comp.addCtxt(comp_lifted, true);

                Ctxt term2(pk);
                term2 = inv_comp;
                term2.multiplyBy(enc_dist[i][j]);

                Ctxt result(pk);
                result = term1;
                result.addCtxt(term2);

                enc_dist[i][j] = result;
            }
        }
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
    cout << "HE-Bridge Encoding Switching Floyd-Warshall" << endl;
    cout << string(80, '=') << endl << endl;

    cout << "Parameters: m=" << m << ", p=" << p << ", r=" << r << ", bits=" << bits << endl << endl;

    Context context = ContextBuilder<BGV>().m(m).p(p).r(r).bits(bits).c(c).skHwt(t).build();

    cout << "Generating keys..." << endl;
    SecKey secret_key(context);
    secret_key.GenSecKey();
    addSome1DMatrices(secret_key);
    addFrbMatrices(secret_key);
    if (r > 1) addFrbMatrices(secret_key);
    PubKey& public_key = secret_key;

    Bridge bridge(context, UNI, r, 1, secret_key, false);
    cout << endl;

    int integerBits = static_cast<int>(ceil(log2(pow(p, r))));

    vector<uint32_t> node_counts = {4, 8, 16, 32};

    cout << "Floyd-Warshall Shortest Path with Encoding Switching" << endl;
    cout << string(80, '-') << endl;
    cout << left << setw(15) << "Nodes"
         << left << setw(15) << "Bit Width"
         << left << setw(20) << "Time"
         << left << setw(10) << "Status" << endl;
    cout << string(80, '-') << endl;

    for (auto nodes : node_counts) {
        cout << left << setw(15) << nodes
             << left << setw(15) << integerBits;
        cout.flush();

        double time = EvaluateFloydWarshall(bridge, context, public_key, secret_key, nodes, integerBits);

        cout << left << setw(20) << formatDuration(time)
             << left << setw(10) << "✓" << endl;
    }

    cout << endl << string(80, '=') << endl;
    return 0;
}
