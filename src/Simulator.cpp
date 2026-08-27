#include "Simulator.h"

Simulator::Simulator(std::unique_ptr<ReplacementPolicy> policy)
    : policy_(std::move(policy)) {}

void Simulator::resetState() {
    pageTable_.reset();
    frameToVPN_.fill(-1);

    freeFrames_ = std::queue<int>{};
    for (int frame = 0; frame < NUM_FRAMES; ++frame) {
        freeFrames_.push(frame);
    }

    policy_->reset(NUM_FRAMES);
}

void Simulator::handleFault(int vpn, bool isWrite, SimulationStats& stats) {
    int frame;

    if (!freeFrames_.empty()) {
        frame = freeFrames_.front();
        freeFrames_.pop();
    } else {
        frame = policy_->selectVictim();

        int victimVPN = frameToVPN_[frame];
        PageTableEntry& victimEntry = pageTable_[victimVPN];
        if (victimEntry.dirty) {
            ++stats.diskWriteBacks;
        }
        victimEntry = PageTableEntry{}; // invalidate the evicted page
    }

    frameToVPN_[frame] = vpn;

    PageTableEntry& entry = pageTable_[vpn];
    entry.frame = frame;
    entry.valid = true;
    entry.dirty = isWrite;
    entry.referenced = true;

    policy_->notifyLoad(frame);
}

SimulationStats Simulator::run(const std::vector<MemoryAccess>& trace) {
    resetState();

    SimulationStats stats;
    stats.policyName = policy_->name();

    for (const MemoryAccess& access : trace) {
        ++stats.totalAccesses;

        int vpn = extractVPN(access.address);
        PageTableEntry& entry = pageTable_[vpn];

        if (entry.valid) {
            entry.referenced = true;
            if (access.isWrite) {
                entry.dirty = true;
            }
            policy_->notifyAccess(entry.frame);
        } else {
            ++stats.pageFaults;
            handleFault(vpn, access.isWrite, stats);
        }
    }

    return stats;
}
