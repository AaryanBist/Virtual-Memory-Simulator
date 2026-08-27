#pragma once

#include <cstdint>

// ---- Hardware architecture constants ----

// 16-bit virtual address space => 64 KB total.
constexpr int VIRTUAL_ADDR_BITS = 16;

// Page size = 512 bytes => low 9 bits of the address are the page offset.
constexpr int OFFSET_BITS = 9;
constexpr int PAGE_SIZE = 1 << OFFSET_BITS; // 512 bytes

// Remaining bits of the virtual address identify the virtual page number.
constexpr int VPN_BITS = VIRTUAL_ADDR_BITS - OFFSET_BITS; // 7 bits
constexpr int NUM_VIRTUAL_PAGES = 1 << VPN_BITS;          // 128 pages

// Physical memory = 16 KB => 32 physical frames of 512 bytes each.
constexpr int PHYSICAL_MEM_SIZE = 16 * 1024; // 16 KB
constexpr int NUM_FRAMES = PHYSICAL_MEM_SIZE / PAGE_SIZE; // 32 frames

using VirtualAddress = std::uint16_t;

// Extracts the virtual page number from a 16-bit virtual address.
inline int extractVPN(VirtualAddress addr) {
    return static_cast<int>(addr >> OFFSET_BITS);
}

// Extracts the page offset from a 16-bit virtual address.
inline int extractOffset(VirtualAddress addr) {
    return static_cast<int>(addr & (PAGE_SIZE - 1));
}
