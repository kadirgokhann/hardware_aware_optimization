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
        c1[i] = 0x1;
    }
    unsigned long* p1 = v1.data();
    unsigned long* p2 = v2.data();
    int* b1 = c1.data();
    for (auto _ : state) {
        unsigned long a1 = 0, a2 = 0;
        for (size_t i = 0; i < N; ++i) {
            if (b1[i]) {
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
g++ always_true.cpp  -lbenchmark -lpthread -O2 -o always_true
----------------------------------------------------------------------------------
Benchmark                        Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------
BM_false_branch/1024           414 ns          413 ns      1746100 items_per_second=2.47882G/s
BM_false_branch/4096          1560 ns         1558 ns       514441 items_per_second=2.62926G/s
BM_false_branch/32768        10903 ns        10890 ns        65304 items_per_second=3.00888G/s
BM_false_branch/262144       96611 ns        96951 ns         7123 items_per_second=2.70389G/s
BM_false_branch/1048576     433500 ns       435061 ns         1577 items_per_second=2.41018G/s
  */
