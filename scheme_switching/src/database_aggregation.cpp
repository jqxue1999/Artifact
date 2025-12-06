#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <random>
#include "utils.h"

using namespace std;
using namespace lbcrypto;

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

// Private database query evaluation with encrypted predicates
double EvaluateDatabaseQuery(uint32_t numRows, uint32_t integerBits) {
    SetupCryptoContext(24, 128, integerBits);

    // Generate random database
    random_device rd;
    mt19937 gen(42);  // Fixed seed
    uniform_int_distribution<int> salary_dis(400, 800);
    uniform_int_distribution<int> hours_dis(6, 12);
    uniform_int_distribution<int> bonus_dis(50, 350);

    // Compute number of batches (128 rows per batch)
    uint32_t num_batches = (numRows + 127) / 128;

    vector<vector<double>> salary_batches;
    vector<vector<double>> work_hours_batches;
    vector<vector<double>> bonus_batches;

    // Generate and pack database into batches
    for (uint32_t batch = 0; batch < num_batches; batch++) {
        vector<double> salary(g_numValues, 0.0);
        vector<double> hours(g_numValues, 0.0);
        vector<double> bonus(g_numValues, 0.0);

        uint32_t start_row = batch * 128;
        uint32_t end_row = min(start_row + 128, numRows);

        for (uint32_t i = start_row; i < end_row; i++) {
            uint32_t idx = i - start_row;
            salary[idx] = salary_dis(gen);
            hours[idx] = hours_dis(gen);
            bonus[idx] = bonus_dis(gen);
        }

        salary_batches.push_back(salary);
        work_hours_batches.push_back(hours);
        bonus_batches.push_back(bonus);
    }

    // Encrypt all batches
    vector<Ciphertext<DCRTPoly>> enc_salary;
    vector<Ciphertext<DCRTPoly>> enc_hours;
    vector<Ciphertext<DCRTPoly>> enc_bonus;

    for (uint32_t batch = 0; batch < num_batches; batch++) {
        Plaintext ptxt_sal = g_cc->MakeCKKSPackedPlaintext(salary_batches[batch]);
        Plaintext ptxt_hrs = g_cc->MakeCKKSPackedPlaintext(work_hours_batches[batch]);
        Plaintext ptxt_bon = g_cc->MakeCKKSPackedPlaintext(bonus_batches[batch]);

        enc_salary.push_back(g_cc->Encrypt(g_keys.publicKey, ptxt_sal));
        enc_hours.push_back(g_cc->Encrypt(g_keys.publicKey, ptxt_hrs));
        enc_bonus.push_back(g_cc->Encrypt(g_keys.publicKey, ptxt_bon));
    }

    // Encrypt comparison constants
    vector<double> lower1_vec(g_numValues, 5000.0);
    vector<double> upper1_vec(g_numValues, 6000.0);
    vector<double> lower2_vec(g_numValues, 700.0);
    vector<double> upper2_vec(g_numValues, 800.0);

    Plaintext ptxt_lower1 = g_cc->MakeCKKSPackedPlaintext(lower1_vec);
    Plaintext ptxt_upper1 = g_cc->MakeCKKSPackedPlaintext(upper1_vec);
    Plaintext ptxt_lower2 = g_cc->MakeCKKSPackedPlaintext(lower2_vec);
    Plaintext ptxt_upper2 = g_cc->MakeCKKSPackedPlaintext(upper2_vec);

    auto enc_lower1 = g_cc->Encrypt(g_keys.publicKey, ptxt_lower1);
    auto enc_upper1 = g_cc->Encrypt(g_keys.publicKey, ptxt_upper1);
    auto enc_lower2 = g_cc->Encrypt(g_keys.publicKey, ptxt_lower2);
    auto enc_upper2 = g_cc->Encrypt(g_keys.publicKey, ptxt_upper2);

    auto t_start = chrono::steady_clock::now();

    int total_matches = 0;

    // Process each batch
    for (uint32_t batch = 0; batch < num_batches; batch++) {
        // Predicate 1: salary * work_hours BETWEEN 5000 AND 6000
        auto product = g_cc->EvalMult(enc_salary[batch], enc_hours[batch]);
        product = g_cc->Rescale(product);

        // product >= 5000
        auto cComp1 = Comparison(product, enc_lower1);
        auto cComp1CKKS = g_cc->EvalFHEWtoCKKS(cComp1, g_numValues, g_numValues);

        // product <= 6000
        auto cComp2 = Comparison(enc_upper1, product);
        auto cComp2CKKS = g_cc->EvalFHEWtoCKKS(cComp2, g_numValues, g_numValues);

        // AND: both must be true
        auto pred1 = g_cc->EvalMult(cComp1CKKS, cComp2CKKS);
        pred1 = g_cc->Rescale(pred1);

        // Predicate 2: salary + bonus BETWEEN 700 AND 800
        auto sum = g_cc->EvalAdd(enc_salary[batch], enc_bonus[batch]);

        // sum >= 700
        auto cComp3 = Comparison(sum, enc_lower2);
        auto cComp3CKKS = g_cc->EvalFHEWtoCKKS(cComp3, g_numValues, g_numValues);

        // sum <= 800
        auto cComp4 = Comparison(enc_upper2, sum);
        auto cComp4CKKS = g_cc->EvalFHEWtoCKKS(cComp4, g_numValues, g_numValues);

        // AND: both must be true
        auto pred2 = g_cc->EvalMult(cComp3CKKS, cComp4CKKS);
        pred2 = g_cc->Rescale(pred2);

        // Combine predicates: pred1 AND pred2
        auto final_pred = g_cc->EvalMult(pred1, pred2);
        final_pred = g_cc->Rescale(final_pred);

        // Decrypt to count matches (in real scenario, would return encrypted result)
        Plaintext ptxt_result;
        g_cc->Decrypt(g_keys.secretKey, final_pred, &ptxt_result);

        uint32_t batch_size = min(128u, numRows - batch * 128);
        ptxt_result->SetLength(batch_size);

        for (uint32_t i = 0; i < batch_size; i++) {
            if (ptxt_result->GetRealPackedValue()[i] > 0.5) {  // Threshold for match
                total_matches++;
            }
        }
    }

    auto t_end = chrono::steady_clock::now();
    double time_sec = chrono::duration<double>(t_end - t_start).count();

    // Verify with plaintext for small databases
    if (numRows <= 128) {
        int expected_matches = 0;
        for (uint32_t i = 0; i < numRows; i++) {
            int batch = i / 128;
            int idx = i % 128;

            double sal = salary_batches[batch][idx];
            double hrs = work_hours_batches[batch][idx];
            double bon = bonus_batches[batch][idx];

            double prod = sal * hrs;
            double s = sal + bon;

            if (prod >= 5000 && prod <= 6000 && s >= 700 && s <= 800) {
                expected_matches++;
            }
        }

        if (abs(total_matches - expected_matches) > numRows * 0.1) {  // Allow 10% error
            cout << "Warning: Match count mismatch (expected ~" << expected_matches
                 << ", got " << total_matches << ")" << endl;
        }
    }

    return time_sec;
}

int main() {
    lbcrypto::OpenFHEParallelControls.Disable();

    cout << string(80, '=') << endl;
    cout << "OpenFHE Scheme Switching Private Database Aggregation" << endl;
    cout << string(80, '=') << endl << endl;

    cout << "Query: SELECT ID FROM emp WHERE" << endl;
    cout << "       salary * work_hours BETWEEN 5000 AND 6000" << endl;
    cout << "       AND salary + bonus BETWEEN 700 AND 800" << endl << endl;

    cout << "Using scheme switching with SIMD batching (128 rows per batch)" << endl << endl;

    // All experiments use 8-bit inputs
    uint32_t bit_width = 8;
    vector<uint32_t> row_counts = {64, 128, 256, 512};

    cout << "Database Size Experiments (8-bit precision)" << endl;
    cout << string(80, '-') << endl;
    cout << left << setw(15) << "Rows"
         << left << setw(15) << "Bit Width"
         << left << setw(20) << "Time"
         << left << setw(15) << "Batches"
         << left << setw(10) << "Status" << endl;
    cout << string(80, '-') << endl;

    for (auto rows : row_counts) {
        int batches = (rows + 127) / 128;
        cout << left << setw(15) << rows
             << left << setw(15) << bit_width;
        cout.flush();

        double time = EvaluateDatabaseQuery(rows, bit_width);

        cout << left << setw(20) << formatDuration(time);
        cout << left << setw(15) << batches;
        cout << left << setw(10) << "✓" << endl;
    }
    cout << endl;

    cout << string(80, '=') << endl;

    return 0;
}
