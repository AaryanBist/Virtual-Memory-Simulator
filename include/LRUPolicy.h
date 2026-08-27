#pragma once

#include <list>
#include <unordered_map>
#include "ReplacementPolicy.h"

// Least Recently Used: evicts the frame that hasn't been touched in the
// longest time. Recency order is kept in a doubly linked list (front =
// most recently used, back = least recently used) so both "touch" and
// "evict" are O(1); the hash map gives O(1) lookup of a frame's list
// position.
class LRUPolicy : public ReplacementPolicy {
public:
    void reset(int /*numFrames*/) override {
        order_.clear();
        position_.clear();
    }

    void notifyAccess(int frame) override {
        touch(frame);
    }

    void notifyLoad(int frame) override {
        touch(frame);
    }

    int selectVictim() override {
        int victim = order_.back();
        order_.pop_back();
        position_.erase(victim);
        return victim;
    }

    std::string name() const override { return "LRU"; }

private:
    void touch(int frame) {
        auto it = position_.find(frame);
        if (it != position_.end()) {
            order_.erase(it->second);
        }
        order_.push_front(frame);
        position_[frame] = order_.begin();
    }

    std::list<int> order_;
    std::unordered_map<int, std::list<int>::iterator> position_;
};
