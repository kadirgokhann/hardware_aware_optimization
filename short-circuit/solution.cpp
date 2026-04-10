#include <string.h>
#include <iostream>
#include <vector>
#include "benchmark/benchmark.h"

void BM_false_branch(benchmark::State& state) {
    srand(1);
    const unsigned int N = state.range(0);
    std::vector<unsigned long> v1(N), v2(N);
    std::vector<int> c1(N), c2(N);
    for (size_t i = 0; i < N; ++i) {
        v1[i] = rand();
        v2[i] = rand();
        c1[i] = rand() & 0x1;
        c2[i] = !c1[i];
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    int* b1 = c1.data();
    int* b2 = c2.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            if (b1[i] | b2[i]) {
                a1 += p1[i];
            } else {
                a1 *= p2[i];
            }
        }
        benchmark::DoNotOptimize(a1);
        benchmark::DoNotOptimize(a2);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(N * state.iterations());
}

BENCHMARK(BM_false_branch)->Range(1 << 10, 1 << 20);
BENCHMARK_MAIN();

/*
g++ false_branch_solution.cpp  -lbenchmark -lpthread -O2 -o solution
----------------------------------------------------------------------------------
Benchmark                        Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------
BM_false_branch/1024           313 ns          313 ns      2373287 items_per_second=3.26832G/s
BM_false_branch/4096          1490 ns         1491 ns       501672 items_per_second=2.74772G/s
BM_false_branch/32768        12466 ns        12473 ns        56838 items_per_second=2.62709G/s
BM_false_branch/262144      101675 ns       101731 ns         6213 items_per_second=2.57683G/s
BM_false_branch/1048576     518700 ns       518997 ns         1534 items_per_second=2.02039G/s
*/

