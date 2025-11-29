/*

Matrix Multiply Cache Benchmark

This program measures the performance difference between two matrix-multiplication methods:

Naive version
Accesses the second matrix column-wise (mul2[k][j]), causing poor spatial locality and heavy cache-line thrashing.

Transposed version
Transposes the second matrix once, then multiplies row-wise (mul2T[j][k]).
This makes memory access sequential and cache-friendly.

The result clearly shows how hardware-awareness (cache lines, memory layout, DRAM latency) can change runtime by several multiples on the same CPU.


Important: choose n according to your cache size

n × n × 8 bytes defines matrix size.
If n is too small, both methods fit in cache and you won’t see a difference.
If n is too large, both methods become DRAM-bound and the advantage shrinks.

Rule of thumb:
Pick n such that one matrix is ~1× to 2× your L2 or L3 cache size.

Example:

4 MB L2 → try n ≈ 1500–2500

16–32 MB L3 → try n ≈ 2500–4000

--------------------------------------------------------------------
g++ -O3 -march=native -std=c++20 matrix_product.cpp -o benchmark


-- ./benchmark
-- ./benchmark transpose

---------------------------------------------------------------------------------

  */

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <string>
#include <unistd.h>

void flushCache()
{
		size_t flushSize;
		flushSize = 300 * 1024 * 1024;

		std::vector<char> buffer(flushSize);

		volatile uint64_t sum;
		sum = 0;

		for (size_t i = 0; i < buffer.size(); i++)
				sum = sum + buffer[i];
}

void buildTranspose(const std::vector<double> &mul, std::vector<double> &mulT, int n)
{
		auto index = [&](int row, int col)
		{
				return row * n + col;
		};

		for (int i = 0; i < n; i++)
		{
				for (int j = 0; j < n; j++)
				{
						mulT[index(i, j)] = mul[index(j, i)];
				}
		}
}

void multiplyNaive(const std::vector<double> &mul1, const std::vector<double> &mul2, std::vector<double> &res, int n)
{
		auto index = [&](int row, int col)
		{
				return row * n + col;
		};

		for (int i = 0; i < n; i++)
		{
				for (int j = 0; j < n; j++)
				{
						double sum;
						sum = 0.0;

						for (int k = 0; k < n; k++)
						{
								sum += mul1[index(i, k)] * mul2[index(k, j)];
						}

						res[index(i, j)] = sum;
				}
		}
}

void multiplyWithTranspose(const std::vector<double> &mul1, const std::vector<double> &mul2T, std::vector<double> &res, int n)
{
		auto index = [&](int row, int col)
		{
				return row * n + col;
		};

		for (int i = 0; i < n; i++)
		{
				for (int j = 0; j < n; j++)
				{
						double sum;
						sum = 0.0;

						for (int k = 0; k < n; k++)
						{
								sum += mul1[index(i, k)] * mul2T[index(j, k)];
						}

						res[index(i, j)] = sum;
				}
		}
}

int main(int argc, char* argv[])
{
		bool transposeMode = false;
		for(int i = 0; i < argc; i++)
		{
			std::string s(argv[i]);
			transposeMode |= s.find("transpose") != std::string::npos;
		}

		const int n = 2'000;

		std::vector<double> mul1(n * n);
		std::vector<double> mul2(n * n);
		std::vector<double> resA(n * n);
		std::vector<double> resB(n * n);
		std::vector<double> mul2T(n * n);

		std::random_device rd;
		std::mt19937 gen(89899898);
		std::uniform_real_distribution<double> dist(0.0, 1.0);

		for (int i = 0; i < n * n; i++)
				mul1[i] = dist(gen);

		for (int i = 0; i < n * n; i++)
				mul2[i] = dist(gen);

		sleep(1);
		flushCache();
		sleep(1);

		if(not transposeMode)
		{
			auto startA = std::chrono::steady_clock::now();

			multiplyNaive(mul1, mul2, resA, n);

			auto endA = std::chrono::steady_clock::now();
			auto durationA = std::chrono::duration_cast<std::chrono::milliseconds>(endA - startA).count();

			double checksumA;
			checksumA = 0.0;
			for (int i = 0; i < n * n; i++)
			{
					checksumA += resA[i];
			}
			uint64_t bitsA;
			bitsA = *reinterpret_cast<uint64_t*>(&checksumA);

			std::cout << "naive ms: " << durationA << "\n";
			std::cout << "checksum: " << std::hex << bitsA << "\n";
		}
		else
		{
			auto startB = std::chrono::steady_clock::now();

			buildTranspose(mul2, mul2T, n);

			multiplyWithTranspose(mul1, mul2T, resB, n);

			auto endB = std::chrono::steady_clock::now();
			auto durationB = std::chrono::duration_cast<std::chrono::milliseconds>(endB - startB).count();

			double checksumB;
			checksumB = 0.0;

			for (int i = 0; i < n * n; i++)
			{
					checksumB += resB[i];
			}
			uint64_t bitsB;
			bitsB = *reinterpret_cast<uint64_t*>(&checksumB);

			std::cout << "transposed ms: " << durationB << "\n";
			std::cout << "checksum: " << std::hex << bitsB << "\n";
		}

		return 0;
}
