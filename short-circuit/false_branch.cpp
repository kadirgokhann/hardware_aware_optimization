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
            if (b1[i] || b2[i]) {
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
----------------------------------------------------------------------------------
Benchmark                        Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------
BM_false_branch/1024          2374 ns         2368 ns       293774 items_per_second=432.393M/s
BM_false_branch/4096          9826 ns         9803 ns        75147 items_per_second=417.832M/s
BM_false_branch/32768       211858 ns       211416 ns         3286 items_per_second=154.993M/s
BM_false_branch/262144     1943889 ns      1940387 ns          371 items_per_second=135.099M/s
BM_false_branch/1048576    7838400 ns      7825269 ns           94 items_per_second=133.999M/s
*/

