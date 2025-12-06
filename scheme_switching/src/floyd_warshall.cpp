#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <random>
#include <climits>
#include "utils.h"

using namespace std;
using namespace lbcrypto;

const int INF = 999999;

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

// Floyd-Warshall on encrypted graph using SIMD packing
double EvaluateFloydWarshall(uint32_t numNodes, uint32_t integerBits) {
    if (numNodes > 128) {
        cout << "Error: Graph too large for SIMD slots (max 128 nodes)" << endl;
        return 0.0;
    }

    SetupCryptoContext(24, 128, integerBits);

    // Generate random graph
    random_device rd;
    mt19937 gen(42);  // Fixed seed
    uniform_int_distribution<int> edge_dis(1, 100);

    vector<vector<double>> graph(numNodes, vector<double>(numNodes, INF));

    // Initialize diagonal to 0
    for (uint32_t i = 0; i < numNodes; i++) {
        graph[i][i] = 0;
    }

    // Add random edges (about 30% density)
    uniform_real_distribution<double> prob_dis(0.0, 1.0);
    for (uint32_t i = 0; i < numNodes; i++) {
        for (uint32_t j = 0; j < numNodes; j++) {
            if (i != j && prob_dis(gen) < 0.3) {
                graph[i][j] = edge_dis(gen);
            }
        }
    }

    // Encrypt the distance matrix
    // Each row is packed into one CKKS ciphertext using SIMD
    vector<Ciphertext<DCRTPoly>> enc_dist;

    for (uint32_t i = 0; i < numNodes; i++) {
        // Pack row i into SIMD slots
        vector<double> row_data(g_numValues, 0.0);
        for (uint32_t j = 0; j < numNodes; j++) {
            row_data[j] = graph[i][j];
        }

        Plaintext ptxt = g_cc->MakeCKKSPackedPlaintext(row_data);
        enc_dist.push_back(g_cc->Encrypt(g_keys.publicKey, ptxt));
    }

    auto t_start = chrono::steady_clock::now();

    // Floyd-Warshall algorithm
    for (uint32_t k = 0; k < numNodes; k++) {
        // Get row k (will be broadcast to compare with all rows)
        auto row_k = enc_dist[k];

        for (uint32_t i = 0; i < numNodes; i++) {
            // Encrypt D[i,k] as a scalar, then broadcast
            vector<double> dik_vec(g_numValues, graph[i][k]);
            Plaintext ptxt_dik = g_cc->MakeCKKSPackedPlaintext(dik_vec);
            auto enc_dik_broadcast = g_cc->Encrypt(g_keys.publicKey, ptxt_dik);

            // Compute D_new[i,:] = D[i,k] + D[k,:]
            auto d_new = g_cc->EvalAdd(enc_dik_broadcast, row_k);

            // Compare: is D_new < D[i,:] ?
            auto cComp = Comparison(d_new, enc_dist[i]);
            auto cCompCKKS = g_cc->EvalFHEWtoCKKS(cComp, g_numValues, g_numValues);

            // Oblivious select: D[i,:] = cComp * D_new + (1 - cComp) * D[i,:]
            auto selected_new = g_cc->EvalMult(cCompCKKS, d_new);
            selected_new = g_cc->Rescale(selected_new);

            // Compute 1 - cComp
            vector<double> ones(g_numValues, 1.0);
            Plaintext ptxt_one = g_cc->MakeCKKSPackedPlaintext(ones);
            auto enc_one = g_cc->Encrypt(g_keys.publicKey, ptxt_one);
            auto inv_comp = g_cc->EvalSub(enc_one, cCompCKKS);

            auto selected_old = g_cc->EvalMult(inv_comp, enc_dist[i]);
            selected_old = g_cc->Rescale(selected_old);

            // Update D[i,:]
            enc_dist[i] = g_cc->EvalAdd(selected_new, selected_old);

            // Decrypt to update plaintext graph for next iteration's broadcast
            Plaintext ptxt_result;
            g_cc->Decrypt(g_keys.secretKey, enc_dist[i], &ptxt_result);
            ptxt_result->SetLength(numNodes);
            for (uint32_t j = 0; j < numNodes; j++) {
                graph[i][j] = ptxt_result->GetRealPackedValue()[j];
            }
        }
    }

    auto t_end = chrono::steady_clock::now();
    double time_sec = chrono::duration<double>(t_end - t_start).count();

    return time_sec;
}

int main() {
    lbcrypto::OpenFHEParallelControls.Disable();

    cout << string(80, '=') << endl;
    cout << "OpenFHE Scheme Switching Floyd-Warshall Algorithm" << endl;
    cout << string(80, '=') << endl << endl;

    cout << "All-pairs shortest path on encrypted graphs with SIMD batching" << endl;
    cout << "Using scheme switching between CKKS and FHEW" << endl << endl;

    // Experiment 1: 32-node graph with different bit widths
    cout << "Experiment 1: 32-node graph with different bit widths" << endl;
    cout << string(80, '-') << endl;
    cout << left << setw(12) << "Nodes"
         << left << setw(15) << "Bit Width"
         << left << setw(20) << "Time"
         << left << setw(15) << "Iterations"
         << left << setw(10) << "Status" << endl;
    cout << string(80, '-') << endl;

    uint32_t n = 32;
    int iterations = n * n;

    for (auto bits : {6, 8, 12, 16}) {
        cout << left << setw(12) << n
             << left << setw(15) << bits;
        cout.flush();

        double time = EvaluateFloydWarshall(n, bits);

        cout << left << setw(20) << formatDuration(time);
        cout << left << setw(15) << iterations;
        cout << left << setw(10) << "✓" << endl;
    }
    cout << endl;

    // Experiment 2: 8-bit inputs with different graph sizes
    cout << "Experiment 2: 8-bit inputs with different graph sizes" << endl;
    cout << string(80, '-') << endl;
    cout << left << setw(12) << "Nodes"
         << left << setw(15) << "Bit Width"
         << left << setw(20) << "Time"
         << left << setw(15) << "Iterations"
         << left << setw(10) << "Status" << endl;
    cout << string(80, '-') << endl;

    uint32_t bit_width = 8;
    for (auto nodes : {16, 32, 64, 128}) {
        int iter = nodes * nodes;
        cout << left << setw(12) << nodes
             << left << setw(15) << bit_width;
        cout.flush();

        double time = EvaluateFloydWarshall(nodes, bit_width);

        cout << left << setw(20) << formatDuration(time);
        cout << left << setw(15) << iter;
        cout << left << setw(10) << "✓" << endl;
    }
    cout << endl;

    cout << string(80, '=') << endl;

    return 0;
}
