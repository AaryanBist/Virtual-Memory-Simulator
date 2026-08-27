#pragma once

#include <array>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "Config.h"
#include "PageTable.h"
#include "ReplacementPolicy.h"
#include "Trace.h"

// Aggregate results of running one trace through one policy.
struct SimulationStats {
    std::string policyName;
    long long totalAccesses = 0;
    long long pageFaults = 0;
    long long diskWriteBacks = 0;

    double pageFaultRate() const {
        return totalAccesses == 0 ? 0.0 : static_cast<double>(pageFaults) / totalAccesses;
    }
};

// Drives address translation, fault handling, and eviction for a single
// replacement policy over a full trace. Each call to run() starts from a
// clean slate (empty page table, all frames free).
class Simulator {
public:
    explicit Simulator(std::unique_ptr<ReplacementPolicy> policy);

    SimulationStats run(const std::vector<MemoryAccess>& trace);

private:
    void resetState();
    // Loads `vpn` into physical memory (taking a free frame if one exists,
    // otherwise evicting a victim chosen by the policy) and updates the
    // page table entry to reflect the access that triggered the fault.
    void handleFault(int vpn, bool isWrite, SimulationStats& stats);

    std::unique_ptr<ReplacementPolicy> policy_;
    PageTable pageTable_;
    std::array<int, NUM_FRAMES> frameToVPN_{};
    std::queue<int> freeFrames_;
};
