
# include <cstdio>

# include "cache.hpp"
# include "trace.hpp"

int main() {
    TraceReader reader;
    reader.open("trace/astar.trace");

    Cache cache;
    bool type; u64 addr;
    while (reader.read(type, addr)) {
        type ? cache.read(addr) : cache.write(addr);
    }

    cache.statistics();
    reader.close();
    return 0;
}