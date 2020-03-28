# ifndef __REPLACE_HPP__
# define __REPLACE_HPP__

# include <algorithm>
# include <cstdlib>
# include <cstdio>

# include "utility.hpp"

enum ReplaceType {
    LRU,
    RANDOM,
    TREELRU
};

class CacheReplace {
public:
    virtual u32 metaSize() = 0;
    virtual u32 find(u32 index) = 0;
    virtual void hit(u32 index, u32 way) = 0;
    virtual ~CacheReplace() {}
};

class LRUReplace: public CacheReplace {
    u32 count, ways, width;
    Bitmap *meta;

public:
    LRUReplace(u32 _count, u32 _ways) {
        count = _count, ways = _ways, width = std:: max(log2(ways), (u32) 1);
        meta = new Bitmap(width, count * ways);

        for (u32 i = 0, index = 0; i < count; ++ i) {
            for (u32 j = 0; j < ways; ++ j, ++ index) {
                meta -> put(index, j);
            }
        }
    }

    u32 metaSize() override {
        return meta -> size();
    }

    u32 find(u32 index) override {
        u32 stack_bottom = index * ways, stack_top = stack_bottom + ways;
        u32 bottom = meta -> get(stack_bottom);
        for (u32 i = stack_bottom; i < stack_top - 1; ++ i)
            meta -> put(i, meta -> get(i + 1));
        meta -> put(stack_top - 1, bottom);
        return bottom;
    }

    void hit(u32 index, u32 way) override {
        u32 stack_bottom = index * ways, stack_top = stack_bottom + ways;
        for (u32 i = stack_bottom; i < stack_top; ++ i) {
            if (meta -> get(i) == way) {
                for (u32 j = i; j < stack_top - 1; ++ j)
                    meta -> put(j, meta -> get(j + 1));
                meta -> put(stack_top - 1, way);
                return;
            }
        }

        // Unreachable
        assert(false);
    }

    ~LRUReplace() {
        delete meta;
    }
};

class RandomReplace: public CacheReplace {
    # define SEED 19981011
    u32 ways;

public:
    RandomReplace(u32 _ways) {
        ways = _ways;
        srand(SEED);
    }

    void hit(u32 index, u32 way) override {}

    u32 metaSize() override {
        return 0;
    }

    u32 find(u32 index) override {
        return rand() % ways;
    }

    ~RandomReplace() {}
};

// Bit 0/1 means older
class TreeLRUReplace: public CacheReplace {
    u32 count, ways, depth;
    Bitmap *meta;

public:
    TreeLRUReplace(u32 _count, u32 _ways) {
        if (_ways > 64) {
            printf("Tree LRU does not support ways > 64\n");
            exit(0);
        }
        count = _count, ways = _ways;
        meta = new Bitmap(ways, count);
        depth = log2(ways);
    }

    u32 metaSize() override {
        return meta -> size();
    }

    u32 find(u32 index) override {
        u64 tree = meta -> get(index);
        u32 node = 0, way = 0;
        for (int i = depth - 1; ~ i; -- i) {
            u32 bit = get_bit(tree, node);
            modify_bit(tree, node, bit ^ 1);
            node = child(node, bit);
            way = (way << 1) | bit;
        }
        meta -> put(index, tree);
        return way;
    }

    void hit(u32 index, u32 way) override {
        u64 tree = meta -> get(index);
        u32 node = 0;
        for (int i = depth - 1; ~ i; -- i) {
            u32 bit = get_bit(way, i);
            modify_bit(tree, i, bit ^ 1);
            node = child(node, bit);
        }
        meta -> put(index, tree);
    }

    ~TreeLRUReplace() {
        delete meta;
    }
};

# endif