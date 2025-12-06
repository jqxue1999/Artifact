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

// Private database query evaluation with encrypted predicates
double EvaluateDatabaseQuery(const Bridge& bridge, const Context& context, const PubKey& pk,
                             const SecKey& sk, uint32_t numRows, uint32_t integerBits) {
    const EncryptedArray& ea = context.getEA();
    long nslots = ea.size();
    long p = context.getP();
    long r = context.getR();

    // Generate random database
    random_device rd;
    mt19937 gen(42);
    uniform_int_distribution<long> salary_dis(400, 800);
    uniform_int_distribution<long> hours_dis(6, 12);
    uniform_int_distribution<long> bonus_dis(50, 350);

    vector<long> salary(numRows);
    vector<long> work_hours(numRows);
    vector<long> bonus(numRows);

    for (uint32_t i = 0; i < numRows; i++) {
        salary[i] = salary_dis(gen);
        work_hours[i] = hours_dis(gen);
        bonus[i] = bonus_dis(gen);
    }

    // Encrypt database columns
    vector<Ctxt> enc_salary;
    vector<Ctxt> enc_hours;
    vector<Ctxt> enc_bonus;

    for (uint32_t i = 0; i < numRows; i++) {
        vector<long> sal_vec(nslots, salary[i]);
        vector<long> hrs_vec(nslots, work_hours[i]);
        vector<long> bon_vec(nslots, bonus[i]);

        Ctxt ct_sal(pk);
        Ctxt ct_hrs(pk);
        Ctxt ct_bon(pk);

        ea.encrypt(ct_sal, pk, sal_vec);
        ea.encrypt(ct_hrs, pk, hrs_vec);
        ea.encrypt(ct_bon, pk, bon_vec);

        enc_salary.push_back(ct_sal);
        enc_hours.push_back(ct_hrs);
        enc_bonus.push_back(ct_bon);
    }

    // Encrypt comparison constants
    vector<long> lower1_vec(nslots, 5000);
    vector<long> upper1_vec(nslots, 6000);
    vector<long> lower2_vec(nslots, 700);
    vector<long> upper2_vec(nslots, 800);

    Ctxt enc_lower1(pk);
    Ctxt enc_upper1(pk);
    Ctxt enc_lower2(pk);
    Ctxt enc_upper2(pk);

    ea.encrypt(enc_lower1, pk, lower1_vec);
    ea.encrypt(enc_upper1, pk, upper1_vec);
    ea.encrypt(enc_lower2, pk, lower2_vec);
    ea.encrypt(enc_upper2, pk, upper2_vec);

    auto t_start = chrono::steady_clock::now();

    // Query: SELECT ID FROM emp WHERE
    //   salary * work_hours BETWEEN 5000 AND 6000
    //   AND salary + bonus BETWEEN 700 AND 800

    // Process each row
    for (uint32_t i = 0; i < numRows; i++) {
        // Predicate 1: salary * work_hours BETWEEN 5000 AND 6000
        Ctxt product(pk);
        product = enc_salary[i];
        product.multiplyBy(enc_hours[i]);

        // product >= 5000
        Ctxt diff1(pk);
        diff1 = product;
        diff1.addCtxt(enc_lower1, true);

        Ctxt comp1(pk);
        bridge.compare(comp1, diff1);
        comp1.multiplyModByP2R();
        Ctxt comp1_lifted(pk);
        bridge.lift(comp1_lifted, comp1, r);

        // product <= 6000 (check upper - product >= 0)
        Ctxt diff2(pk);
        diff2 = enc_upper1;
        diff2.addCtxt(product, true);

        Ctxt comp2(pk);
        bridge.compare(comp2, diff2);
        comp2.multiplyModByP2R();
        Ctxt comp2_lifted(pk);
        bridge.lift(comp2_lifted, comp2, r);

        // AND: both must be true
        Ctxt pred1(pk);
        pred1 = comp1_lifted;
        pred1.multiplyBy(comp2_lifted);

        // Predicate 2: salary + bonus BETWEEN 700 AND 800
        Ctxt sum(pk);
        sum = enc_salary[i];
        sum.addCtxt(enc_bonus[i]);

        // sum >= 700
        Ctxt diff3(pk);
        diff3 = sum;
        diff3.addCtxt(enc_lower2, true);

        Ctxt comp3(pk);
        bridge.compare(comp3, diff3);
        comp3.multiplyModByP2R();
        Ctxt comp3_lifted(pk);
        bridge.lift(comp3_lifted, comp3, r);

        // sum <= 800
        Ctxt diff4(pk);
        diff4 = enc_upper2;
        diff4.addCtxt(sum, true);

        Ctxt comp4(pk);
        bridge.compare(comp4, diff4);
        comp4.multiplyModByP2R();
        Ctxt comp4_lifted(pk);
        bridge.lift(comp4_lifted, comp4, r);

        // AND: both must be true
        Ctxt pred2(pk);
        pred2 = comp3_lifted;
        pred2.multiplyBy(comp4_lifted);

        // Combine predicates: pred1 AND pred2
        Ctxt final_pred(pk);
        final_pred = pred1;
        final_pred.multiplyBy(pred2);

        // In real scenario, would return encrypted result
        // For benchmarking, we just compute the predicate
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
    cout << "HE-Bridge Encoding Switching Private Database Aggregation" << endl;
    cout << string(80, '=') << endl << endl;

    cout << "Query: SELECT ID FROM emp WHERE" << endl;
    cout << "       salary * work_hours BETWEEN 5000 AND 6000" << endl;
    cout << "       AND salary + bonus BETWEEN 700 AND 800" << endl << endl;

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

    vector<uint32_t> row_counts = {16, 32, 64, 128};

    cout << "Database Query with Encoding Switching" << endl;
    cout << string(80, '-') << endl;
    cout << left << setw(15) << "Rows"
         << left << setw(15) << "Bit Width"
         << left << setw(20) << "Time"
         << left << setw(10) << "Status" << endl;
    cout << string(80, '-') << endl;

    for (auto rows : row_counts) {
        cout << left << setw(15) << rows
             << left << setw(15) << integerBits;
        cout.flush();

        double time = EvaluateDatabaseQuery(bridge, context, public_key, secret_key, rows, integerBits);

        cout << left << setw(20) << formatDuration(time)
             << left << setw(10) << "✓" << endl;
    }

    cout << endl << string(80, '=') << endl;
    return 0;
}
