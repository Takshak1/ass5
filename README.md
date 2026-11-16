# Assignment 5 - Concurrent Linked Lists

Two thread-safe sorted linked lists from Lecture 17-18, in C++17:

| File | List |
|------|------|
| `src/global_lock_list.hpp` | Coarse-grained list: one global mutex held for the whole operation |
| `src/hoh_list.hpp` | Hand-over-hand (lock coupling) list: one mutex per node |

Both use sentinel head (`INT_MIN`) and tail (`INT_MAX`) nodes and support `insert`, `remove` and `find`.

## Build

```
make            # or: mingw32-make on Windows
```

This builds `test` (correctness test) and `bench` (throughput benchmark).

## Correctness test

```
./test
```

Threads run random insert/remove/find on a small key range. At the end, the list size must equal successful inserts minus successful removes, and the list must still be sorted.

## Benchmark

```
./bench <global|hoh> <threads> [duration_ms=1000] [key_range=1024] [update_pct=20]
```
