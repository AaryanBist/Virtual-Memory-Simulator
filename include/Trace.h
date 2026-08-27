#pragma once

#include <string>
#include <vector>
#include "Config.h"

// One parsed line of the memory trace file: "R 0x4FA2" or "W 0x4FA2".
struct MemoryAccess {
    bool isWrite;
    VirtualAddress address;
};

// Reads a trace file where each line is "<R|W> <hex address>".
// Throws std::runtime_error if the file can't be opened or a line is malformed.
std::vector<MemoryAccess> loadTrace(const std::string& path);
