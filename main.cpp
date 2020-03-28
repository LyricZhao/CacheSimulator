
# include <cstdio>

# include "cache.hpp"
# include "trace.hpp"
# include "utility.hpp"

int main() {
    TraceReader reader;
    reader.open("trace/bzip2.trace");

    Cache64 cache(LayoutType::WAY_4, ReplaceType::TREELRU, WriteAllocateType::WRITE_ALLOCATE, WritePolicyType::WRITE_BACK, 131072, 64, "bzip2");
    bool type; u64 addr;
    while (reader.read(type, addr)) {
        type ? cache.read(addr) : cache.write(addr);
    }

    cache.statistics();
    reader.close();
    return 0;
}