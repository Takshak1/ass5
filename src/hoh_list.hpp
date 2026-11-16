// Hand-over-hand (lock coupling) sorted linked list - Lecture 17-18, slides 24-26.
// Every node has its own mutex. A traversal holds the locks of two adjacent
// nodes (pred, curr) and locks the next node before unlocking the one behind it.
#pragma once

#include <climits>
#include <mutex>

class HohList {
    struct Node {
        int key;
        Node* next;
        std::mutex lock;
        Node(int k, Node* n) : key(k), next(n) {}
    };

    Node* head;  // sentinel, key = INT_MIN

public:
    static constexpr const char* name = "hand-over-hand";

    HohList() {
        Node* tail = new Node(INT_MAX, nullptr);  // sentinel, key = INT_MAX
        head = new Node(INT_MIN, tail);
    }

    ~HohList() {
        while (head) {
            Node* next = head->next;
            delete head;
            head = next;
        }
    }

    HohList(const HohList&) = delete;
    HohList& operator=(const HohList&) = delete;

    bool find(int k) {
        Node* pred = head;
        pred->lock.lock();
        Node* curr = pred->next;
        curr->lock.lock();
        while (curr->key < k) {
            pred->lock.unlock();
            pred = curr;
            curr = curr->next;
            curr->lock.lock();
        }
        bool found = (curr->key == k);
        pred->lock.unlock();
        curr->lock.unlock();
        return found;
    }

    bool insert(int k) {
        Node* pred = head;
        pred->lock.lock();
        Node* curr = pred->next;
        curr->lock.lock();
        while (curr->key < k) {
            pred->lock.unlock();
            pred = curr;
            curr = curr->next;
            curr->lock.lock();
        }
        bool found = (curr->key == k);
        if (!found) pred->next = new Node(k, curr);
        pred->lock.unlock();
        curr->lock.unlock();
        return !found;
    }

    bool remove(int k) {
        Node* pred = head;
        pred->lock.lock();
        Node* curr = pred->next;
        curr->lock.lock();
        while (curr->key < k) {
            pred->lock.unlock();
            pred = curr;
            curr = curr->next;
            curr->lock.lock();
        }
        bool found = (curr->key == k);
        if (found) {
            pred->next = curr->next;
            curr->lock.unlock();
            delete curr;
            pred->lock.unlock();
        } else {
            pred->lock.unlock();
            curr->lock.unlock();
        }
        return found;
    }

    // Single-threaded helpers for testing.
    long size() const {
        long n = 0;
        for (Node* c = head->next; c->key != INT_MAX; c = c->next) n++;
        return n;
    }

    bool is_sorted() const {
        for (Node* c = head; c->next; c = c->next)
            if (c->key >= c->next->key) return false;
        return true;
    }
};
