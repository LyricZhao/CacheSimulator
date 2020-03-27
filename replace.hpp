# ifndef __REPLACE_HPP__
# define __REPLACE_HPP__

# include "layout.hpp"
# include "utility.hpp"

class CacheReplace {
public:
    virtual void replace(u64 addr) const = 0;
};

# endif