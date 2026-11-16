// Coarse-grained (global lock) sorted linked list - Lecture 17-18, slides 12-13.
// One mutex L protects the whole list.
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

    bool insert(int k) {
        std::lock_guard<std::mutex> g(L);
        Node* pred = head;
        Node* curr = pred->next;
        while (curr->key < k) {
            pred = curr;
            curr = curr->next;
        }
        bool result = (curr->key != k);
        if (result) pred->next = new Node(k, curr);
        return result;
    }

    bool remove(int k) {
        std::lock_guard<std::mutex> g(L);
        Node* pred = head;
        Node* curr = pred->next;
        while (curr->key < k) {
            pred = curr;
            curr = curr->next;
        }
        bool result = (curr->key == k);
        if (result) {
            pred->next = curr->next;
            delete curr;
        }
        return result;
    }

    bool find(int k) {
        std::lock_guard<std::mutex> g(L);
        Node* curr = head;
        while (curr->key < k) curr = curr->next;
        return curr->key == k;
    }
};
