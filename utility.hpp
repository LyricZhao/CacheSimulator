# ifndef __UTILITY_HPP__
# define __UTILITY_HPP__

# include <algorithm>

typedef unsigned long long  u64;
typedef unsigned int        u32;
typedef unsigned char       u8;

inline u32 log2(u32 value) {
    u32 ret = -1;
    while (value) {
        value >>= 1, ++ ret;
    }
    return ret;
}

// TODO: check r = 64 whether works or r < l
inline u64 cut_bits(u64 value, u64 l, u64 r) {
    value >>= l;
    value &= (1ull << r) - 1ull;
}

class Bitmap {
    u8 *data;

    u32 unit_bits, count;
public:
    Bitmap(u32 _unit_bits, u32 _count) {
        unit_bits = _unit_bits, count = _count;
        data = (u8*) std:: malloc(count * _unit_bits / 8);
    }

    ~Bitmap() {
        std:: free((void*) data);
    }

    u64 get(u32 addr) {
        u64 ret = 0;
        u32 bits_l = addr * unit_bits, bits_r = bits_l + unit_bits;
        u32 l = bits_l / 8, r = (bits_r - 1) / 8;
        for (u32 i = l; i <= r; ++ i) {
            u32 cut_l = (i == l) ? bits_l % 8 : 0;
            u32 cut_r = (i == r) ? bits_r % 8 : 8;
            cut_r = (!cut_r) ? 8 : cut_r;
            ret |= cut_bits(data[i], cut_l, cut_r) & 255;
            if (i != r) {
                ret <<= std:: min((u32) 8, bits_r - i * 8);
            }
        }
        return ret;
    }

    void put(u32 addr, u64 entry) {
        u32 bits_l = addr * unit_bits, bits_r = bits_l + unit_bits;
        u32 l = bits_l / 8, r = (bits_r - 1) / 8, processed = 0;
        for (u32 i = l; i <= r; ++ i) {
            u32 cut_size = (i == l) ? ((i + 1) * 8 - bits_l): 8;
            u32 cut = std::min(processed + 8, unit_bits);
            u8 update = cut_bits(entry, processed, cut);
            data[i] |= update;
            processed = cut;
        }
    }
};

# endif