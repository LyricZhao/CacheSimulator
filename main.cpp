
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

const char* name[4] = {"astar", "bzip2", "mcf", "perlbench"};
const char* path[4] = {"trace/astar.trace", "trace/bzip2.trace", "trace/mcf.trace", "trace/perlbench.trace"};

// For answering the problems
void experiment_1() {
    printf("Running experiment 1 ...\n");
    for (int i = 0; i < 4; ++ i)
        for (auto layout: {DIRECT, WAY_4, WAY_8})
            for (auto block_size: {8, 32, 64})
                testcase(name[i], path[i], layout,
                    ReplaceType::LRU, WriteAllocateType::WRITE_ALLOCATE,
                    WritePolicyType::WRITE_BACK, 131072, block_size);
}

void experiment_2() {
    printf("Running experiment 2 ...\n");
    for (int i = 0; i < 4; ++ i)
        for (auto replace: {RANDOM, LRU, TREELRU})
            testcase(name[i], path[i], LayoutType::WAY_8,
                replace, WriteAllocateType::WRITE_ALLOCATE,
                WritePolicyType::WRITE_BACK, 131072, 8);
}

void experiment_3() {
    printf("Running experiment 3 ...\n");
    for (int i = 0; i < 4; ++ i)
        for (auto write_allocate: {WRITE_ALLOCATE, WRITE_NOT_ALLOCATE})
            for (auto write_policy: {WRITE_BACK, WRITE_THROUGH})
                testcase(name[i], path[i], LayoutType::WAY_8,
                    ReplaceType::LRU, write_allocate,
                    write_policy, 131072, 8);
}

int main() {
# ifdef DEBUG_MODE
    debug();
# else
    experiment_1();
# endif
    return 0;
}