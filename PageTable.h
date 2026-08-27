#pragma once

#include <array>
#include "Config.h"

// A single page table entry: where the page lives in physical memory (if
// anywhere) plus the three bookkeeping bits every OS course cares about.
struct PageTableEntry {
    int frame = -1;        // Physical frame number, or -1 if not mapped.
    bool valid = false;    // Present/valid bit.
    bool dirty = false;    // Set on writes; cleared when the page is (re)loaded.
    bool referenced = false; // Set on any access; used by the Clock policy.
};

// Flat, fixed-size page table indexed directly by virtual page number.
class PageTable {
public:
    void reset() {
        for (auto& entry : entries_) {
            entry = PageTableEntry{};
        }
    }

    PageTableEntry& operator[](int vpn) { return entries_[vpn]; }
    const PageTableEntry& operator[](int vpn) const { return entries_[vpn]; }

private:
    std::array<PageTableEntry, NUM_VIRTUAL_PAGES> entries_{};
};
