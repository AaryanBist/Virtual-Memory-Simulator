#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include "ClockPolicy.h"
#include "Config.h"
#include "FIFOPolicy.h"
#include "LRUPolicy.h"
#include "Simulator.h"
#include "Trace.h"

namespace {

void printHeader() {
    std::cout << "Virtual Memory Simulator\n"
              << "  Virtual address space: " << (1 << VIRTUAL_ADDR_BITS) / 1024 << " KB ("
              << VIRTUAL_ADDR_BITS << "-bit addresses)\n"
              << "  Page size:              " << PAGE_SIZE << " bytes\n"
              << "  Physical memory:        " << PHYSICAL_MEM_SIZE / 1024 << " KB (" << NUM_FRAMES
              << " frames)\n\n";
}

void printResults(const std::vector<SimulationStats>& results) {
    std::cout << std::left << std::setw(10) << "Policy" << std::right << std::setw(14) << "Accesses"
              << std::setw(14) << "Page Faults" << std::setw(12) << "Fault %" << std::setw(16)
              << "Write-Backs" << "\n";
    std::cout << std::string(66, '-') << "\n";

    for (const auto& stats : results) {
        std::cout << std::left << std::setw(10) << stats.policyName << std::right << std::setw(14)
                  << stats.totalAccesses << std::setw(14) << stats.pageFaults << std::setw(11)
                  << std::fixed << std::setprecision(2) << (stats.pageFaultRate() * 100.0) << "%"
                  << std::setw(16) << stats.diskWriteBacks << "\n";
    }
}

} // namespace

int main(int argc, char** argv) {
    std::string tracePath = argc > 1 ? argv[1] : "trace.txt";

    std::vector<MemoryAccess> trace;
    try {
        trace = loadTrace(tracePath);
    } catch (const std::exception& ex) {
        std::cerr << "Error loading trace: " << ex.what() << "\n";
        return 1;
    }

    printHeader();
    std::cout << "Loaded " << trace.size() << " memory accesses from \"" << tracePath << "\"\n\n";

    std::vector<SimulationStats> results;

    Simulator fifoSim(std::make_unique<FIFOPolicy>());
    results.push_back(fifoSim.run(trace));

    Simulator lruSim(std::make_unique<LRUPolicy>());
    results.push_back(lruSim.run(trace));

    Simulator clockSim(std::make_unique<ClockPolicy>());
    results.push_back(clockSim.run(trace));

    printResults(results);

    return 0;
}
