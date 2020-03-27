# ifndef __LAYOUT_HPP__
# define __LAYOUT_HPP__

# include "utility.hpp"

enum LayoutType {
    DIRECT,
    FULLY,
    WAY_4,
    WAY_8
};

// Address layout: [tag index offset]
// total size = (2 ^ index) * ways * block_size

class CacheLayout64 {
    u32 ways; // We can simply use 'ways' to control which method the layout is
    u32 size, meta_size, cache_size, block_size, way_size;

    u32 bits_tag, bits_index, bits_offset;
    Bitmap *meta;

public:
    CacheLayout64(LayoutType type, u32 _cache_size, u32 _block_size) {
        switch (type) {
            DIRECT:
                ways = 1;
                break;
            FULLY:
                ways = _cache_size;
                break;
            WAY_4:
                ways = 4;
                break;
            WAY_8:
                ways = 8;
                break;
            default:
                printf("Cache layout type does not support");
                break;
        }
        cache_size = _cache_size;
        block_size = _block_size;
        way_size = cache_size / ways;

        bits_index = log2(cache_size / ways / block_size);
        bits_offset = log2(block_size); // index by bytes not words
        bits_tag = 64 - bits_index - bits_offset;

        size = (bits_tag + block_size * 8) * (cache_size / block_size) / 8;
        meta_size = bits_tag * (cache_size / block_size) / 8;

        meta = new Bitmap(bits_tag, cache_size / block_size);
    }

    bool read(u64 addr) {
        u64 offset = cut_bits(addr, 0, bits_offset);
        u64 index = cut_bits(addr, bits_offset, bits_offset + bits_index);
        u64 tag = cut_bits(addr, bits_offset + bits_index, 64);

        u32 l = index * ways, r = l + ways;
        for (u32 i = l; i < r; ++ i) {
            if (meta -> get(i) == tag) { // Hit
                return true;
            }
        }

        return false;
    }
};

# endif