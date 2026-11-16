// Correctness test for both lists.
//
// Many threads run random insert/remove/find on a small key range so they
// collide on the same nodes. Each thread counts its successful inserts and
// removes. If the list is correct, then at the end:
//     size == successful inserts - successful removes
// and the list is still strictly sorted. The races from slides 5-9 (lost
// inserts, deleted nodes coming back, duplicate keys) would break this.
#include <cstdio>
#include <random>
#include <thread>
#include <vector>

#include "global_lock_list.hpp"
#include "hoh_list.hpp"

constexpr int OPS_PER_THREAD = 200000;
constexpr int KEY_RANGE = 128;

template <class List>
bool sequential_test() {
    List l;
    bool ok = true;
    ok &= l.insert(5) == true;
    ok &= l.insert(3) == true;
    ok &= l.insert(5) == false;  // duplicate
    ok &= l.find(3) == true;
    ok &= l.find(4) == false;
    ok &= l.remove(3) == true;
    ok &= l.remove(3) == false;
    ok &= l.find(3) == false;
    ok &= l.size() == 1;
    ok &= l.is_sorted();
    std::printf("sequential %-15s %s\n", List::name, ok ? "PASS" : "FAIL");
    return ok;
}

template <class List>
bool concurrent_test(int nthreads) {
    List l;
    std::vector<long> ins(nthreads, 0), rem(nthreads, 0);
    std::vector<std::thread> threads;

    for (int t = 0; t < nthreads; t++) {
        threads.emplace_back([&, t] {
            std::mt19937_64 rng(1234 + t);
            std::uniform_int_distribution<int> key(1, KEY_RANGE), op(0, 2);
            long my_ins = 0, my_rem = 0;
            for (int i = 0; i < OPS_PER_THREAD; i++) {
                int k = key(rng);
                switch (op(rng)) {
                    case 0: my_ins += l.insert(k); break;
                    case 1: my_rem += l.remove(k); break;
                    default: l.find(k); break;
                }
            }
            ins[t] = my_ins;
            rem[t] = my_rem;
        });
    }
    for (auto& th : threads) th.join();

    long total_ins = 0, total_rem = 0;
    for (int t = 0; t < nthreads; t++) {
        total_ins += ins[t];
        total_rem += rem[t];
    }
    long size = l.size();
    bool sorted = l.is_sorted();
    bool ok = sorted && size == total_ins - total_rem;
    std::printf("  %-15s threads=%2d  inserts_ok=%ld removes_ok=%ld size=%ld sorted=%s  %s\n",
                List::name, nthreads, total_ins, total_rem, size, sorted ? "yes" : "NO",
                ok ? "PASS" : "FAIL");
    return ok;
}

int main() {
    bool ok = true;
    ok &= sequential_test<GlobalLockList>();
    ok &= sequential_test<HohList>();

    std::printf("concurrent stress (key range %d, %d ops/thread):\n", KEY_RANGE, OPS_PER_THREAD);
    for (int t : {1, 2, 4, 8, 16, 32}) ok &= concurrent_test<GlobalLockList>(t);
    for (int t : {1, 2, 4, 8, 16, 32}) ok &= concurrent_test<HohList>(t);

    std::printf("\n%s\n", ok ? "ALL TESTS PASSED" : "SOME TESTS FAILED");
    return ok ? 0 : 1;
}
