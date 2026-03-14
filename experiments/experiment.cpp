#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>  // For std::unique_ptr
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

#include "basehashtable.hpp"
#include "linear.hpp"
#include "robinhood.hpp"

// Utility functions (isPrime, nextPrime, generateDistinctKeys) remain the
// same...
bool isPrime(int n) {
    if (n < 2) return false;
    if (n < 4) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6)
        if (n % i == 0 || n % (i + 2) == 0) return false;
    return true;
}

int nextPrime(int n) {
    while (!isPrime(n)) n++;
    return n;
}

std::vector<int> generateDistinctKeys(int n, std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(1, 1'000'000'000);
    std::unordered_set<int>            seen;
    std::vector<int>                   keys;
    keys.reserve(n);
    while ((int)keys.size() < n) {
        int k = dist(rng);
        if (seen.insert(k).second) keys.push_back(k);
    }
    return keys;
}

// Helper to calculate variance using the Base pointer
double calculateVariance(const BaseHashTable& ht) {
    double avg   = ht.getAvgProbeLength();
    auto   dist  = ht.getProbeDistribution();
    double sumSq = 0;
    int    total = 0;
    for (auto const& [d, cnt] : dist) {
        sumSq += cnt * (d - avg) * (d - avg);
        total += cnt;
    }
    return total > 0 ? sumSq / total : 0.0;
}

// ============================================================
//  Experiment A: Search Cost vs Load Factor
// ============================================================
void experimentA() {
    std::cout << "\n========================================\n";
    std::cout << " EXPERIMENT A: Search Cost vs Load Factor\n";
    std::cout << "========================================\n\n";

    const int           M      = nextPrime(10007);
    const int           TRIALS = 5;
    std::vector<double> alphas = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};

    std::cout << std::fixed << std::setprecision(3);
    std::cout << std::setw(6) << "Alpha" << " | " << std::setw(10) << "LP Avg"
              << " " << std::setw(8) << "LP Max" << " | " << std::setw(10)
              << "RH Avg" << " " << std::setw(8) << "RH Max" << "\n";
    std::cout << std::string(60, '-') << "\n";

    for (double alpha : alphas) {
        int    n        = static_cast<int>(alpha * M);
        double lpAvgSum = 0, rhAvgSum = 0;
        int    lpMaxSum = 0, rhMaxSum = 0;

        for (int t = 0; t < TRIALS; t++) {
            std::mt19937 rng(42 + t * 1000);
            auto         keys = generateDistinctKeys(n, rng);

            // Using derived classes
            LinearProbingTable lp(M);
            RobinHoodTable     rh(M);

            for (int k : keys) {
                lp.insert(k);
                rh.insert(k);
            }

            long long lpTotal = 0, rhTotal = 0;
            int       lpMax = 0, rhMax = 0;

            for (int k : keys) {
                int pc;
                lp.search(k, pc);
                lpTotal += pc;
                lpMax = std::max(lpMax, pc);
                rh.search(k, pc);
                rhTotal += pc;
                rhMax = std::max(rhMax, pc);
            }

            lpAvgSum += (double)lpTotal / n;
            rhAvgSum += (double)rhTotal / n;
            lpMaxSum += lpMax;
            rhMaxSum += rhMax;
        }

        std::cout << std::setw(6) << alpha << " | " << std::setw(10)
                  << lpAvgSum / TRIALS << " " << std::setw(8)
                  << (double)lpMaxSum / TRIALS << " | " << std::setw(10)
                  << rhAvgSum / TRIALS << " " << std::setw(8)
                  << (double)rhMaxSum / TRIALS << "\n";
    }
}

// ============================================================
//  Experiment B: Probe Distance Distribution
// ============================================================
void experimentB() {
    std::cout << "\n========================================\n";
    std::cout << " EXPERIMENT B: Probe Distance Distribution (alpha=0.75)\n";
    std::cout << "========================================\n\n";

    const int    M = nextPrime(10007);
    int          n = static_cast<int>(0.75 * M);
    std::mt19937 rng(12345);
    auto         keys = generateDistinctKeys(n, rng);

    LinearProbingTable lp(M);
    RobinHoodTable     rh(M);

    for (int k : keys) {
        lp.insert(k);
        rh.insert(k);
    }

    auto lpDist = lp.getProbeDistribution();
    auto rhDist = rh.getProbeDistribution();

    int maxD = 0;
    for (auto const& [d, _] : lpDist) maxD = std::max(maxD, d);
    for (auto const& [d, _] : rhDist) maxD = std::max(maxD, d);

    std::cout << std::setw(8) << "Dist" << " | " << std::setw(10) << "LP Count"
              << " | " << std::setw(10) << "RH Count" << "\n";
    std::cout << std::string(38, '-') << "\n";

    for (int d = 0; d <= maxD; d++) {
        int lpC = lpDist.count(d) ? lpDist.at(d) : 0;
        int rhC = rhDist.count(d) ? rhDist.at(d) : 0;
        if (lpC == 0 && rhC == 0) continue;
        std::cout << std::setw(8) << d << " | " << std::setw(10) << lpC << " | "
                  << std::setw(10) << rhC << "\n";
    }

    std::cout << "\n  LP — Avg: " << lp.getAvgProbeLength()
              << ", Max: " << lp.getMaxProbeLength()
              << ", Var: " << calculateVariance(lp) << "\n";
    std::cout << "  RH — Avg: " << rh.getAvgProbeLength()
              << ", Max: " << rh.getMaxProbeLength()
              << ", Var: " << calculateVariance(rh) << "\n";
}

// ============================================================
//  Experiment C: Deletion Stress Test
// ============================================================
void experimentC() {
    std::cout << "\n========================================\n";
    std::cout << " EXPERIMENT C: Deletion Stress Test\n";
    std::cout << "========================================\n\n";

    const int    M = nextPrime(10007);
    int          n = static_cast<int>(0.7 * M);
    std::mt19937 rng(99999);
    auto         keys = generateDistinctKeys(n, rng);

    LinearProbingTable lp(M);
    RobinHoodTable     rh(M);

    for (int k : keys) {
        lp.insert(k);
        rh.insert(k);
    }

    std::vector<int>                   currentKeys(keys.begin(), keys.end());
    std::uniform_int_distribution<int> valDist(1, 1'000'000'000);

    const int ROUNDS = 1000;
    for (int r = 0; r < ROUNDS; r++) {
        std::uniform_int_distribution<int> idxDist(0,
                                                   (int)currentKeys.size() - 1);
        int                                idx    = idxDist(rng);
        int                                delKey = currentKeys[idx];

        lp.remove(delKey);
        rh.remove(delKey);

        int newKey;
        do {
            newKey = valDist(rng);
        } while (lp.search(newKey));

        lp.insert(newKey);
        rh.insert(newKey);
        currentKeys[idx] = newKey;
    }

    long long lpTotal = 0, rhTotal = 0;
    int       lpMax = 0, rhMax = 0;

    for (int k : currentKeys) {
        int pc;
        lp.search(k, pc);
        lpTotal += pc;
        lpMax = std::max(lpMax, pc);
        rh.search(k, pc);
        rhTotal += pc;
        rhMax = std::max(rhMax, pc);
    }

    std::cout << "After " << ROUNDS << " delete-insert rounds:\n";
    std::cout << "  LP Avg: " << (double)lpTotal / n
              << " | RH Avg: " << (double)rhTotal / n << "\n";
}

// ============================================================
//  Verification: Polymorphism in action
// ============================================================
void runVerification(BaseHashTable& ht, const std::string& label) {
    std::cout << "--- " << label << " ---\n";
    std::vector<int> testKeys = {10, 23, 36, 49, 62, 15, 28};
    for (int k : testKeys) ht.insert(k);

    std::cout << ht;  // Assuming operator<< is defined in BaseHashTable

    std::cout << "Deleting 36...\n";
    ht.remove(36);

    int  pc;
    bool found = ht.search(49, pc);
    std::cout << "Search(49): " << (found ? "FOUND" : "NOT FOUND") << " in "
              << pc << " probes\n\n";
}

int main() {
    // Demonstrating Polymorphism
    int                m = 13;
    LinearProbingTable lp(m);
    RobinHoodTable     rh(m);

    std::cout << "VERIFICATION TEST\n";
    runVerification(lp, "Linear Probing");
    runVerification(rh, "Robin Hood");

    // Running standard experiments
    experimentA();
    experimentB();
    experimentC();

    return 0;
}