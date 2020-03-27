# ifndef __REPLACE_HPP__
# define __REPLACE_HPP__

# include <cstdlib>

# include "layout.hpp"
# include "utility.hpp"

enum ReplaceType {
    LRU,
    RANDOM,
    TREE
};

class CacheReplace {
public:
    virtual u32 find(u32 index) const = 0;
    virtual void hit(u32 index, u32 way) const = 0;
};

class LRUReplace: CacheReplace {

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

    u32 find(u32 index) {
        return rand() % ways;
    }
};

class TreeReplace: CacheReplace {
    u32 ways;

public:
    TreeReplace(u32 _ways) {
        ways = _ways;
    }
};

# endif