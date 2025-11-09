// Source - https://stackoverflow.com/q
// Posted by GManNickG, modified by community. See post 'Timeline' for change history
// Retrieved 2025-11-09, License - CC BY-SA 4.0

#include <algorithm>
#include <ctime>
#include <iostream>

int main()
{
    // Generate data
    const unsigned arraySize = 32768;
    int data[arraySize];

    for (unsigned c = 0; c < arraySize; ++c)
        data[c] = std::rand() % 256;

   
/////////////////////////////////////////////////////////////
// If you do not perform this sorting, the whole code runs 5 secs on average on my Macbook Air M2.
// In reverse, the code takes more than 20 secs on my computer. Hold on, wasn't sorting expensive in terms of complexity and
// don't all the perf-guys say 'run away from sortings'? That's called 'pre-mature optimization' where all beginners fall.
// I leave the reason to the reader.
    std::sort(data, data + arraySize);
////////////////////////////////////////////////////////////

    // Test
    clock_t start = clock();
    long long sum = 0;
    for (unsigned i = 0; i < 100000; ++i)
    {
        for (unsigned c = 0; c < arraySize; ++c)
        {   // Primary loop.
            if (data[c] >= 128)
                sum += data[c];
        }
    }

    double elapsedTime = static_cast<double>(clock()-start) / CLOCKS_PER_SEC;

    std::cout << elapsedTime << '\n';
    std::cout << "sum = " << sum << '\n';
}
