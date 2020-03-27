# ifndef __CACHE_HPP__
# define __CACHE_HPP__

# include <cstdio>

# include "layout.hpp"
# include "replace.hpp"
# include "utility.hpp"

class Cache {
    int hit, miss;
    CacheLayout64 *layout;
    CacheReplace *replace;

public:
    Cache(CacheLayout64 *_layout, CacheReplace *_replace) {
        hit = miss = 0;
        layout = _layout;
        replace = _replace;
    }

    void read(u64 addr) {
        if (layout -> read(addr)) {
            ++ hit;
            return;
        }

        ++ miss;
        replace -> replace(addr);
    }

    void write(u64 addr) {
        // TODO
    }

    void statistics() {
        double rate = 1.0 * hit / miss;
        printf("Hit rate: %.2f (%d hits, %d misses)\n", rate, hit, miss);
    }
};

# endif