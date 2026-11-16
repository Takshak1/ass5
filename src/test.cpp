// Tests for the linked lists.
#include <cstdio>

#include "global_lock_list.hpp"
#include "hoh_list.hpp"

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

int main() {
    bool ok = true;
    ok &= sequential_test<GlobalLockList>();
    ok &= sequential_test<HohList>();
    return ok ? 0 : 1;
}
