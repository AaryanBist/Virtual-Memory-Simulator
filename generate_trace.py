#!/usr/bin/env python3
"""
Generates a synthetic memory trace exhibiting temporal locality: a small
"hot" set of pages receives the large majority of accesses, so that
recency-aware policies (LRU, Clock) naturally outperform FIFO.

Output format, one access per line:
    <R|W> 0x<4 hex digits>

Matches the simulator's hardware model: 16-bit virtual addresses,
512-byte pages (7-bit VPN, 9-bit offset).
"""

import argparse
import random

PAGE_SIZE = 512
NUM_PAGES = (1 << 16) // PAGE_SIZE  # 128 virtual pages

DEFAULT_NUM_OPS = 50_000
HOT_PAGE_FRACTION = 0.20   # 20% of pages...
HOT_ACCESS_PROBABILITY = 0.80  # ...receive 80% of accesses
WRITE_PROBABILITY = 0.30  # fraction of accesses that are writes


def generate_trace(num_ops: int, seed: int) -> list[str]:
    rng = random.Random(seed)

    all_pages = list(range(NUM_PAGES))
    num_hot = max(1, round(NUM_PAGES * HOT_PAGE_FRACTION))
    hot_pages = rng.sample(all_pages, num_hot)
    cold_pages = [p for p in all_pages if p not in hot_pages]

    lines = []
    for _ in range(num_ops):
        if cold_pages and rng.random() > HOT_ACCESS_PROBABILITY:
            page = rng.choice(cold_pages)
        else:
            page = rng.choice(hot_pages)

        offset = rng.randrange(PAGE_SIZE)
        address = (page << 9) | offset

        op = "W" if rng.random() < WRITE_PROBABILITY else "R"
        lines.append(f"{op} 0x{address:04X}")

    return lines


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-n", "--num-ops", type=int, default=DEFAULT_NUM_OPS,
                         help=f"number of memory accesses to generate (default: {DEFAULT_NUM_OPS})")
    parser.add_argument("-o", "--output", default="trace.txt",
                         help="output trace file path (default: trace.txt)")
    parser.add_argument("-s", "--seed", type=int, default=42,
                         help="random seed, for reproducible traces (default: 42)")
    args = parser.parse_args()

    lines = generate_trace(args.num_ops, args.seed)
    with open(args.output, "w") as f:
        f.write("\n".join(lines) + "\n")

    print(f"Wrote {len(lines)} accesses to {args.output}")
    print(f"  Hot pages: {round(NUM_PAGES * HOT_PAGE_FRACTION):.0f} / {NUM_PAGES} "
          f"receive ~{HOT_ACCESS_PROBABILITY * 100:.0f}% of accesses")


if __name__ == "__main__":
    main()
