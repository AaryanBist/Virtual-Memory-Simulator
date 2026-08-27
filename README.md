# Virtual Memory Simulator

A modular C++17 simulator of address translation, page faults, and page
replacement, built for an introductory Operating Systems course.

## Architecture modeled

| Parameter              | Value                                    |
|-------------------------|-------------------------------------------|
| Virtual address space   | 16-bit (64 KB), addresses `0x0000-0xFFFF` |
| Page size               | 512 bytes (9 offset bits)                 |
| Virtual page number     | 7 bits -> 128 virtual pages                |
| Physical memory         | 16 KB -> 32 physical frames                |

A virtual address is split as `VPN = addr >> 9`, `offset = addr & 0x1FF`.

## Project layout

```
include/
  Config.h              Hardware constants + VPN/offset extraction
  PageTable.h            PageTableEntry (frame, valid, dirty, referenced)
  Trace.h                Trace file record + loader declaration
  Simulator.h            Simulator + SimulationStats
  ReplacementPolicy.h    Abstract base class for eviction policies
  FIFOPolicy.h           FIFO eviction (std::queue)
  LRUPolicy.h            LRU eviction (std::list + std::unordered_map)
  ClockPolicy.h          Clock / Second-Chance eviction (circular array)
src/
  Trace.cpp              Parses "R/W 0xADDR" trace lines
  Simulator.cpp           Address translation, fault handling, eviction
  main.cpp                Runs all three policies and prints comparison
generate_trace.py         Generates a locality-biased trace file
Makefile
```

## Building

Requires a C++17 compiler (g++ or clang++).

```bash
make          # builds ./vmsim (mingw32-make on Windows/MinGW)
make clean    # removes the built binary
```

Or compile directly:

```bash
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude src/main.cpp src/Simulator.cpp src/Trace.cpp -o vmsim -static-libgcc -static-libstdc++
```

> The `-static-lib{gcc,stdc++}` flags avoid a crash on Windows machines that
> have more than one MinGW toolchain on `PATH` (e.g. Git for Windows'
> bundled MinGW alongside an MSYS2 install), which can otherwise load a
> mismatched `libstdc++-6.dll` at runtime.

## Generating a trace

```bash
python generate_trace.py               # 50,000 ops -> trace.txt
python generate_trace.py -n 10000 -o small_trace.txt -s 7
```

Each line is `<R|W> <hex address>`, e.g. `W 0x4FA2`. The generator biases
80% of accesses toward a fixed "hot" 20% of the 128 virtual pages, which
gives LRU and Clock a genuine advantage over FIFO to demonstrate.

## Running

```bash
make run                 # generates nothing itself, just runs ./vmsim trace.txt
./vmsim trace.txt
```

Sample output:

```
Policy          Accesses   Page Faults     Fault %     Write-Backs
------------------------------------------------------------------
FIFO               50000         20210      40.42%           10526
LRU                50000         16122      32.24%            7916
Clock              50000         17803      35.61%            8999
```

## How it works

For every access, `Simulator::run` extracts the VPN and checks the page
table:

- **Hit** (valid bit set): set the referenced bit, set the dirty bit on
  writes, and notify the replacement policy of the access.
- **Fault**: increment the fault counter. If a free frame exists, use it;
  otherwise ask the policy for a victim frame via `selectVictim()`. If the
  victim's dirty bit was set, increment the disk write-back counter, then
  invalidate its page table entry and load the new page into that frame.

Each `ReplacementPolicy` subclass only has to track *which frame to evict
next* — it never touches the page table directly:

- **FIFO** pushes a frame onto a `std::queue` when loaded; the victim is
  whatever is at the front, regardless of later accesses.
- **LRU** keeps frames in a `std::list` ordered by recency (front = most
  recently used) with an `std::unordered_map` for O(1) lookup; every
  access moves a frame to the front, and the victim is the back.
- **Clock** keeps a reference bit per frame in a circular array with a
  sweeping "hand": on eviction it clears bits as it scans until it finds
  one already clear, giving recently-referenced frames a second chance.
