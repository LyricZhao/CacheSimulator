# ifndef __LAYOUT_HPP__
# define __LAYOUT_HPP__

# include <string>

# include "nameof.hpp"
# include "replace.hpp"
# include "utility.hpp"

enum LayoutType {
    DIRECT,
    FULLY,
    WAY_4,
    WAY_8
};

enum WriteAllocateType {
    WRITE_ALLOCATE,
    WRITE_NOT_ALLOCATE
};

enum WritePolicyType {
    WRITE_BACK,
    WRITE_THROUGH
};

// Address layout: [tag index offset]
// total size = (2 ^ index) * ways * block_size

class Cache64 {
    # define HIT    true
    # define MISS   false

    // Parameters
    u32 ways; // We can simply use 'ways' to control which method the layout is
    u32 size, meta_size, cache_size, block_size, way_size, index_count, valid_dirty_size;

    // Meta infomation
    u32 bits_tag, bits_index, bits_offset;
    Bitmap *meta;

    // Counter
    u32 hit, miss;

    // Strategies
    LayoutType type;
    ReplaceType replace_type;
    CacheReplace *replace;
    WriteAllocateType write_allocate_type;
    WritePolicyType write_policy_type;

    // Logger
    std:: string case_name;
    FILE *file;
    const char *csv;

public:
    Cache64(const char *name, const char *_csv, LayoutType _type, ReplaceType _replace_type, WriteAllocateType _write_allocate_type, WritePolicyType _write_policy_type, u32 _cache_size, u32 _block_size) {
        type = _type;
        replace_type = _replace_type;
        case_name = std:: string(name);
        hit = miss = 0;
        csv = _csv;
        
        // Parameters
        switch (type) {
            case DIRECT:    ways = 1;                           break;
            case FULLY:     ways = _cache_size / _block_size;   break;
            case WAY_4:     ways = 4;                           break;
            case WAY_8:     ways = 8;                           break;
            default: printf("Cache layout type does not support\n"); exit(0);
        }
        cache_size = _cache_size;
        block_size = _block_size;
        way_size = cache_size / ways;

        bits_index = log2(cache_size / ways / block_size);
        bits_offset = log2(block_size); // index by bytes not words
        bits_tag = 64 - bits_index - bits_offset;
        index_count = 1 << bits_index;

        valid_dirty_size = 1 + (_write_policy_type == WritePolicyType::WRITE_BACK);
        size = (bits_tag + valid_dirty_size + block_size * 8) * (cache_size / block_size) / 8;
        meta_size = (bits_tag + valid_dirty_size) * (cache_size / block_size) / 8;

        // Meta
        meta = new Bitmap(bits_tag + valid_dirty_size, cache_size / block_size);

        // Strategies
        write_allocate_type = _write_allocate_type;
        write_policy_type = _write_policy_type;
        switch (replace_type) {
            case LRU:       replace = new LRUReplace(index_count, ways);        break;
            case RANDOM:    replace = new RandomReplace(ways);                  break;
            case TREELRU:   replace = new TreeLRUReplace(index_count, ways);    break;
            default: printf("Replace policy does not support\n"); exit(0);
        }

        // Open log file
        char filename[256];
        sprintf(filename, "log/%s_%s+%s+%s+%s+%dB.log",
            name,
            std:: string(NAMEOF_ENUM(type)).c_str(),
            std:: string(NAMEOF_ENUM(replace_type)).c_str(),
            std:: string(NAMEOF_ENUM(write_allocate_type)).c_str(),
            std:: string(NAMEOF_ENUM(write_policy_type)).c_str(),
            block_size);
        file = fopen(filename, "w");
    }

    void access(u64 addr, bool just_check) {
        u64 offset = cut_bits(addr, 0, bits_offset);
        u64 index = cut_bits(addr, bits_offset, bits_offset + bits_index);
        u64 tag = cut_bits(addr, bits_offset + bits_index, 64);
        
        u32 l = index * ways, r = l + ways;
        bool has_empty = false; u32 first_empty; 
        for (u32 i = l; i < r; ++ i) {
            u64 entry = meta -> get(i);
            bool valid = entry & 1;
            u64 entry_tag = entry >> valid_dirty_size;
            if (valid && entry_tag == tag) { // Hit
                replace -> hit(index, i % ways);
                log(HIT);
                return;
            } else if (!valid && !has_empty) {
                first_empty = i;
                has_empty = true;
            }
        }

        log(MISS);
        if (just_check)
            return;

        u32 pos;
        if (has_empty) {
            pos = first_empty;
            replace -> hit(index, pos % ways);
        } else {
            pos = l + replace -> find(index);
        }
        meta -> put(pos, (tag << valid_dirty_size) | 1);
    }

    void read(u64 addr) {
        access(addr, false);
    }

    void write(u64 addr) {
        if (write_allocate_type == WriteAllocateType::WRITE_ALLOCATE)
            return access(addr, false);
        if (write_allocate_type == WriteAllocateType::WRITE_NOT_ALLOCATE)
            return access(addr, true);
    }

    void log(bool x) {
        if (x) {
            ++ hit;
            fprintf(file, "Hit\n");
        } else {
            ++ miss;
            fprintf(file, "Miss\n");
        }
    }

    u32 writeCausedSize() {
        return (write_policy_type == WritePolicyType::WRITE_BACK) * (cache_size / block_size);
    }

    void statistics() {
        double hit_rate = 100.0 * hit / (hit + miss);
        printf("Case %s:\n", case_name.c_str());
        printf(" > Type: %s\n", std:: string(NAMEOF_ENUM(type)).c_str());
        printf(" > Block size: %d bytes\n", block_size);
        printf(" > Replace policy: %s\n", std:: string(NAMEOF_ENUM(replace_type)).c_str());
        printf(" > Write Allocate policy: %s\n", std:: string(NAMEOF_ENUM(write_allocate_type)).c_str());
        printf(" > Write policy: %s\n", std:: string(NAMEOF_ENUM(write_policy_type)).c_str());
        printf(" > Hit rate: %.5f%% (%d hits, %d misses)\n", hit_rate, hit, miss);

        u32 total_size = size + replace -> metaSize() + writeCausedSize();
        u32 total_meta_size = meta_size + replace -> metaSize() + writeCausedSize();
        double meta_rate = 100. * total_meta_size / total_size;
        printf(" > Total space: %d bytes\n", total_size);
        printf(" > Total meta size: %d bytes (%.2lf%%)\n", total_meta_size, meta_rate);
        printf(" > Layout-caused meta size: %d bytes\n", meta_size);
        printf(" > Replace-caused meta size: %d bytes\n", replace -> metaSize());
        printf(" > Write-caused meta size: %d bytes\n\n", writeCausedSize());

        if (csv != nullptr) {
            FILE *cf = fopen(csv, "a+");
            // case,type,block_size,replace,write_allocate,write_policy,hit,miss,rate,meta,layout_meta,replace_meta,write_meta
            fprintf(cf, "%s,%s,%d,%s,%s,%s,%d,%d,%.5lf,%d,%d,%d,%d\n",
                case_name.c_str(),
                std:: string(NAMEOF_ENUM(type)).c_str(),
                block_size,
                std:: string(NAMEOF_ENUM(replace_type)).c_str(),
                std:: string(NAMEOF_ENUM(write_allocate_type)).c_str(),
                std:: string(NAMEOF_ENUM(write_policy_type)).c_str(),
                hit, miss,
                hit_rate,
                total_meta_size,
                meta_size,
                replace -> metaSize(),
                writeCausedSize());
            fclose(cf);
        }
    }

    ~Cache64() {
        delete meta;
        delete replace;
        fclose(file);
    }
};

# endif