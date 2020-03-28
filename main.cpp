
# include <cstdio>

# include "cache.hpp"
# include "trace.hpp"
# include "utility.hpp"

int main() {
    TraceReader reader;
    reader.open("trace/astar.trace");

    Cache64 cache(LayoutType::DIRECT, ReplaceType::RANDOM, WriteAllocateType::WRITE_ALLOCATE, WritePolicyType::WRITE_BACK, 131072, 8, "astar");
    bool type; u64 addr;
    while (reader.read(type, addr)) {
        printf("%d addr=%llx\n", type, addr);
        type ? cache.read(addr) : cache.write(addr);
    }

    cache.statistics();
    reader.close();
    return 0;
}