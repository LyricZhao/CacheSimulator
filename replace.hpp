# ifndef __REPLACE_HPP__
# define __REPLACE_HPP__

# include <cstdlib>
# include <cstdio>

# include "layout.hpp"
# include "utility.hpp"

enum ReplaceType {
    LRU,
    RANDOM,
    TREE
};

class CacheReplace {
public:
    virtual u32 metaSize() const = 0;
    virtual u32 find(u32 index) const = 0;
    virtual void hit(u32 index, u32 way) const = 0;
};

class LRUReplace: CacheReplace {
    u32 count, ways, width;
    Bitmap *meta;

public:
    LRUReplace(u32 _count, u32 _ways) {
        if (_ways > 16) {
            printf("LRU does not support ways > 16");
            return;
        }
        
        count = _count, ways = _ways, width = log2(ways);
        meta = new Bitmap(width * ways, count);

        u64 entry = 0;
        for (u32 i = 0; i < entry; ++ i) {
            entry = (entry << width) | i;
        }
        for (u32 i = 0; i < count; ++ i) {
            meta -> put(i, entry);
        }
    }

    u32 metaSize() {
        return meta -> size();
    }

    u32 find(u32 index) {
        u64 stack = meta -> get(index);
        u64 bottom = stack & ((1ull << width) - 1);
        stack >>= width;
        stack |= bottom << (width * (ways - 1));
        meta -> put(index, stack);
    }

    void hit(u32 index, u32 way) {
        u64 stack = meta -> get(index);
        static u8 s[16]; u32 pos;
        for (u32 i = 0; i < ways; ++ i) {
            s[i] = stack & ((1ull << width) - 1);
            if (s[i] == way)
                pos = i;
        }
        stack = way;
        for (u32 i = 0; i < ways; ++ i) {
            if (i == pos)
                continue;
            stack = (stack << width) | s[i];
        }
        meta -> put(index, stack);
    }

    ~LRUReplace() {
        delete meta;
    }
};

class RandomReplace: CacheReplace {
    # define SEED 19981011
    u32 ways;

public:
    RandomReplace(u32 _ways) {
        ways = _ways;
        srand(SEED);
    }

    void hit(u32 index, u32 way) {
        // Do nothing
    }

    u32 metaSize() {
        return 0;
    }

    u32 find(u32 index) {
        return rand() % ways;
    }
};

class TreeReplace: CacheReplace {
    u32 count, ways;
    Bitmap *meta;

public:
    TreeReplace(u32 _count, u32 _ways) {
        count = _count, ways = _ways;
        meta = new Bitmap(ways, count);
    }

    u32 metaSize() {
        return meta -> size();
    }

    u32 find(u32 index) {

    }

    void hit(u32 index, u32 way) {
        
    }

    ~TreeReplace() {
        delete meta;
    }
};

# endif