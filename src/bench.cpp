// Throughput benchmark for the two concurrent lists.
//
// usage: bench <global|hoh> <threads> [duration_ms=1000] [key_range=1024] [update_pct=20]
//
// The list is pre-filled with key_range/2 random keys. Each thread then runs
// random operations for duration_ms: update_pct% are updates (half insert,
// half remove, so the size stays around key_range/2) and the rest are finds.
// Prints one CSV line: list,threads,key_range,update_pct,duration_s,total_ops,ops_per_sec
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <thread>
#include <vector>

#include "global_lock_list.hpp"
#include "hoh_list.hpp"

using Clock = std::chrono::steady_clock;

struct alignas(64) Counter {  // padded so threads don't share a cache line
    long ops = 0;
};

template <class List>
void run(int nthreads, int duration_ms, int key_range, int update_pct) {
    List l;

    std::mt19937_64 fill_rng(42);
    std::uniform_int_distribution<int> fill_key(1, key_range);
    long size = 0;
    while (size < key_range / 2)
        if (l.insert(fill_key(fill_rng))) size++;

    std::atomic<int> ready{0};
    std::atomic<bool> go{false}, stop{false};
    std::vector<Counter> counts(nthreads);
    std::vector<std::thread> threads;

    for (int t = 0; t < nthreads; t++) {
        threads.emplace_back([&, t] {
            std::mt19937_64 rng(1000 + t);
            std::uniform_int_distribution<int> key(1, key_range), pct(0, 99);
            long ops = 0;
            ready++;
            while (!go.load(std::memory_order_acquire)) std::this_thread::yield();
            while (!stop.load(std::memory_order_relaxed)) {
                int k = key(rng);
                int p = pct(rng);
                if (p < update_pct / 2)
                    l.insert(k);
                else if (p < update_pct)
                    l.remove(k);
                else
                    l.find(k);
                ops++;
            }
            counts[t].ops = ops;
        });
    }

    while (ready.load() < nthreads) std::this_thread::yield();
    auto start = Clock::now();
    go.store(true, std::memory_order_release);
    std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
    stop.store(true);
    for (auto& th : threads) th.join();
    double secs = std::chrono::duration<double>(Clock::now() - start).count();

    long total = 0;
    for (auto& c : counts) total += c.ops;
    std::printf("%s,%d,%d,%d,%.4f,%ld,%.0f\n", List::name, nthreads, key_range, update_pct, secs,
                total, total / secs);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr,
                     "usage: %s <global|hoh> <threads> [duration_ms] [key_range] [update_pct]\n",
                     argv[0]);
        return 1;
    }
    int nthreads = std::atoi(argv[2]);
    int duration_ms = argc > 3 ? std::atoi(argv[3]) : 1000;
    int key_range = argc > 4 ? std::atoi(argv[4]) : 1024;
    int update_pct = argc > 5 ? std::atoi(argv[5]) : 20;

    if (std::strcmp(argv[1], "global") == 0)
        run<GlobalLockList>(nthreads, duration_ms, key_range, update_pct);
    else if (std::strcmp(argv[1], "hoh") == 0)
        run<HohList>(nthreads, duration_ms, key_range, update_pct);
    else {
        std::fprintf(stderr, "unknown list type '%s'\n", argv[1]);
        return 1;
    }
    return 0;
}
