#pragma once

#include <queue>
#include "ReplacementPolicy.h"

// First-In, First-Out: evicts whichever frame has been resident the
// longest, regardless of how recently it was used.
class FIFOPolicy : public ReplacementPolicy {
public:
    void reset(int /*numFrames*/) override {
        loadOrder_ = std::queue<int>{};
    }

    void notifyAccess(int /*frame*/) override {
        // FIFO ignores accesses entirely; only load order matters.
    }

    void notifyLoad(int frame) override {
        loadOrder_.push(frame);
    }

    int selectVictim() override {
        int victim = loadOrder_.front();
        loadOrder_.pop();
        return victim;
    }

    std::string name() const override { return "FIFO"; }

private:
    std::queue<int> loadOrder_;
};
