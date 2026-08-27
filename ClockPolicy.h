#pragma once

#include <vector>
#include "ReplacementPolicy.h"

// Clock (Second-Chance): an approximation of LRU that avoids per-access
// list manipulation. Frames are arranged in a circular array; a "hand"
// sweeps them looking for a victim. A frame with its reference bit set
// is given a second chance (bit cleared, hand advances); the first frame
// found with a clear reference bit is evicted.
class ClockPolicy : public ReplacementPolicy {
public:
    void reset(int numFrames) override {
        referenceBit_.assign(numFrames, false);
        hand_ = 0;
    }

    void notifyAccess(int frame) override {
        referenceBit_[frame] = true;
    }

    void notifyLoad(int frame) override {
        referenceBit_[frame] = true;
    }

    int selectVictim() override {
        while (referenceBit_[hand_]) {
            referenceBit_[hand_] = false;
            advanceHand();
        }
        int victim = hand_;
        advanceHand();
        return victim;
    }

    std::string name() const override { return "Clock"; }

private:
    void advanceHand() {
        hand_ = (hand_ + 1) % static_cast<int>(referenceBit_.size());
    }

    std::vector<bool> referenceBit_;
    int hand_ = 0;
};
