# ifndef __LAYOUT_HPP__
# define __LAYOUT_HPP__

# include "replace.hpp"
# include "utility.hpp"

enum LayoutType {
    DIRECT,
    FULLY,
    WAY_4,
    WAY_8
};

// Address layout: [tag index offset]
// total size = (2 ^ index) * ways * block_size

class Cache64 {
    // Parameters
    u32 ways; // We can simply use 'ways' to control which method the layout is
    u32 size, meta_size, cache_size, block_size, way_size;

    // Meta infomation
    u32 bits_tag, bits_index, bits_offset;
    Bitmap *meta;

    // Counter
    u32 hit, miss;

    // Replacer
    CacheReplace *replace;

public:
    Cache64(LayoutType type, u32 _cache_size, u32 _block_size, ReplaceType replace_type) {
        switch (type) {
            DIRECT: ways = 1;           break;
            FULLY:  ways = _cache_size; break;
            WAY_4:  ways = 4;           break;
            WAY_8:  ways = 8;           break;
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

        size = (bits_tag + 2 + block_size * 8) * (cache_size / block_size) / 8;
        meta_size = (bits_tag + 2) * (cache_size / block_size) / 8; // valid + dirty

        meta = new Bitmap(bits_tag, cache_size / block_size);
    }

    void read(u64 addr) {
        u64 offset = cut_bits(addr, 0, bits_offset);
        u64 index = cut_bits(addr, bits_offset, bits_offset + bits_index);
        u64 tag = cut_bits(addr, bits_offset + bits_index, 64);

        u32 l = index * ways, r = l + ways;
        bool has_empty = false; u32 first_empty; 
        for (u32 i = l; i < r; ++ i) {
            u64 entry = meta -> get(i);
            bool valid = entry & 1, dirty = entry & 2;
            u64 entry_tag = entry >> 2;
            if (valid && entry_tag == tag) { // Hit
                replace -> hit(index, i % ways);
                return;
            } else if (!valid) {
                first_empty = i;
                has_empty = true;
            }
        }

        u32 pos;
        if (has_empty) {
            pos = first_empty;
            replace -> hit(index, pos % ways);
        } else {
            pos = l + replace -> find(index);
        }
        meta -> put(pos, (tag << 2) | 3);
    }

    void write(u64 addr) {
        
    }

    void statistics() {
        double rate = 1.0 * hit / miss;
        printf("Hit rate: %.2f (%d hits, %d misses)\n", rate, hit, miss);
    }

    ~Cache64() {
        delete meta;
    }
};

# endif