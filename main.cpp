
# include <cstdio>

# include "cache.hpp"
# include "trace.hpp"
# include "utility.hpp"

// # define DEBUG_MODE

void testcase(const char *name,
    const char *filename,
    LayoutType layout_type,
    ReplaceType replace_type,
    WriteAllocateType write_allocate_type,
    WritePolicyType write_policy_type,
    u32 cache_size,
    u32 block_size) {
    
    // Read
    TraceReader reader;
    reader.open(filename);

    // Simulate
    Cache64 cache(name, layout_type, replace_type, write_allocate_type, write_policy_type, cache_size, block_size);
    bool type; u64 addr;
    while (reader.read(type, addr)) {
        type ? cache.read(addr) : cache.write(addr);
    }

    // Statistics
    cache.statistics();
    reader.close();
}

void debug() {
    // For debug
    testcase("astar", "trace/astar.trace", LayoutType::DIRECT, ReplaceType::TREELRU, WriteAllocateType::WRITE_ALLOCATE, WritePolicyType::WRITE_BACK, 131072, 64);
}

// For answering the problems
void experiment_1() {
    testcase("astar", "trace/astar.trace", LayoutType::DIRECT, ReplaceType::TREELRU, WriteAllocateType::WRITE_ALLOCATE, WritePolicyType::WRITE_BACK, 131072, 64);
}

int main() {
# ifdef DEBUG_MODE
    debug();
# else
    experiment_1();
# endif
    return 0;
}