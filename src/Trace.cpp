#include "Trace.h"

#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

std::vector<MemoryAccess> loadTrace(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open trace file: " + path);
    }

    std::vector<MemoryAccess> trace;
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;
        if (line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue; // skip blank lines
        }

        std::istringstream iss(line);
        std::string opToken, addrToken;
        if (!(iss >> opToken >> addrToken)) {
            throw std::runtime_error("Malformed trace line " + std::to_string(lineNumber) + ": " + line);
        }

        char op = static_cast<char>(std::toupper(static_cast<unsigned char>(opToken[0])));
        if (op != 'R' && op != 'W') {
            throw std::runtime_error("Unknown operation on trace line " + std::to_string(lineNumber) + ": " + line);
        }

        unsigned long addrValue;
        try {
            addrValue = std::stoul(addrToken, nullptr, 16);
        } catch (const std::exception&) {
            throw std::runtime_error("Bad hex address on trace line " + std::to_string(lineNumber) + ": " + line);
        }

        trace.push_back(MemoryAccess{op == 'W', static_cast<VirtualAddress>(addrValue)});
    }

    return trace;
}
