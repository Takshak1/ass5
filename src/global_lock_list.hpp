// Coarse-grained (global lock) sorted linked list - Lecture 17-18, slides 12-13.
#pragma once

#include <climits>
#include <mutex>

class GlobalLockList {
    struct Node {
        int key;
        Node* next;
        Node(int k, Node* n) : key(k), next(n) {}
    };

    Node* head;  // sentinel, key = INT_MIN
    std::mutex L;

public:
    static constexpr const char* name = "global-lock";

    GlobalLockList() {
        Node* tail = new Node(INT_MAX, nullptr);  // sentinel, key = INT_MAX
        head = new Node(INT_MIN, tail);
    }

    ~GlobalLockList() {
        while (head) {
            Node* next = head->next;
            delete head;
            head = next;
        }
    }
};
