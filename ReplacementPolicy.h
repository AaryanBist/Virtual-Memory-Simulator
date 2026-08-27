#pragma once

#include <string>

// Base interface every eviction policy implements. The Simulator drives a
// policy through three events:
//
//   notifyAccess(frame) - a page already resident in `frame` was touched
//                          (a "hit"). Used by policies that care about
//                          recency (LRU) or reference bits (Clock).
//   notifyLoad(frame)   - a brand-new page was just placed into `frame`,
//                          either because it was free or because it was
//                          just evicted and reused. The policy must start
//                          tracking this frame from scratch.
//   selectVictim()      - all frames are full; the policy must pick a
//                          frame to evict and return it. The Simulator is
//                          responsible for writing the evicted page back
//                          to the page table; the policy only tracks
//                          frames.
class ReplacementPolicy {
public:
    virtual ~ReplacementPolicy() = default;

    virtual void reset(int numFrames) = 0;
    virtual void notifyAccess(int frame) = 0;
    virtual void notifyLoad(int frame) = 0;
    virtual int selectVictim() = 0;
    virtual std::string name() const = 0;
};
