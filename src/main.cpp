#include <iostream>
#include <pthread.h>
#include <time.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <random>
#include <atomic>

using namespace std;

std::vector<size_t> NUMBERS;
std::atomic<size_t> THREAD_PART = 0;

void merge(int low, int mid, int high)
{
    std::vector<size_t> left;
    left.resize(mid - low + 1);

    std::vector<size_t> right;
    right.resize(high - mid);

    size_t n1 = mid - low + 1;
    size_t n2 = high - mid;
    size_t i;
    size_t j;

    for (i = 0; i < n1; ++i)
        left[i] = NUMBERS[i + low];

    for (i = 0; i < n2; ++i)
        right[i] = NUMBERS[i + mid + 1];

    int k = low;
    i = j = 0;

    while (i < n1 && j < n2) {
        if (left[i] <= right[j]) {
            NUMBERS[k++] = left[i++];
        }
        else {
            NUMBERS[k++] = right[j++];
        }
    }

    while (i < n1)
        NUMBERS[k++] = left[i++];

    while (j < n2)
        NUMBERS[k++] = right[j++];
}

void merge_sort(int low, int high)
{
    int mid = low + (high - low) / 2;

    if (low < high) {
        merge_sort(low, mid);
        merge_sort(mid + 1, high);
        merge(low, mid, high);
    }
}

void merge_sort(const size_t &arraySize, const size_t &countThreads)
{
    int thread_part = THREAD_PART++;
    int low = thread_part * (arraySize / countThreads);
    int high = (thread_part + 1) * (arraySize / countThreads) - 1;
    int mid = low + (high - low) / 2;

    if (low < high) {
        merge_sort(low, mid);
        merge_sort(mid + 1, high);
        merge(low, mid, high);
    }
}

int main()
{
    std::default_random_engine DRE(std::time(nullptr));
    std::uniform_int_distribution<> UID(0, 1000);

    size_t arraySize = 0;

    std::cout << "Enter size of array: " << std::flush;
    std::cin >> arraySize;

    NUMBERS.resize(arraySize, 0);

    size_t countThreads = 0;

    std::cout << "Enter count of threads: " << std::flush;
    std::cin >> countThreads;

    for (size_t i = 0; i < arraySize; i++)
        NUMBERS[i] = UID(DRE);

    const auto begin = std::chrono::steady_clock::now();

    std::vector<std::thread> threads;

    for (size_t i = 0; i < countThreads; i++)
        threads.emplace_back(std::thread([&]() {
            merge_sort(arraySize, countThreads);
        }));

    for (size_t i = 0; i < countThreads; i++)
        if (threads[i].joinable())
            threads[i].join();

    merge(0, (arraySize / 2 - 1) / 2, arraySize / 2 - 1);
    merge(arraySize / 2, arraySize / 2 + (arraySize - 1 - arraySize / 2) / 2, arraySize - 1);
    merge(0, (arraySize - 1) / 2, arraySize - 1);

    const auto end = std::chrono::steady_clock::now();
    const auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    cout << "Sorted array: ";
    for (size_t i = 0; i < arraySize; i++)
        cout << NUMBERS[i] << " ";

    std::cout << std::endl;

    std::cout << "The time in ms: " << elapsed_ms.count() << std::endl;

    return 0;
}
