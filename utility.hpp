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

inline u64 cut_bits(u64 value, u64 l, u64 r) {
    value >>= l;
    value &= (1ull << (r - l)) - 1ull;
    return value;
}

inline u64 get_bit(u64 value, u64 which) {
    return (value >> which) & 1;
}

inline void modify_bit(u64 &value, u64 which, u64 bit) {
    value |= 1 << which;
    if (!bit) {
        value ^= 1 << which;
    }
}

inline void update_bits(u8 &value, u8 update, u32 l, u32 r) {
    u8 ones = ((1 << (r - l)) - 1) << l;
    value |= ones; value ^= ones;
    value |= update << l;
}

// which = 0 for left, 1 for right
inline u32 child(u32 index, u32 which) {
    return (index << 1) + 1 + which;
}

class Bitmap {
    u8 *data;

    u32 unit_bits, count, bytes;
public:
    Bitmap(u32 _unit_bits, u32 _count) {
        unit_bits = _unit_bits, count = _count;
        bytes = count * _unit_bits / 8;
        data = (u8*) std:: malloc(bytes);
        memset((void*) data, 0, bytes);
    }

    ~Bitmap() {
        std:: free((void*) data);
    }

    u32 size() {
        return bytes;
    }

    u64 get(u32 addr) {
        u64 ret = 0;
        u32 bits_l = addr * unit_bits, bits_r = bits_l + unit_bits;
        int l = bits_l / 8, r = (bits_r - 1) / 8;
        for (u32 i = l; i <= r; ++ i) {
            u32 cut_l = (i == l) ? bits_l % 8 : 0;
            u32 cut_r = (i == r) ? bits_r % 8 : 8;
            cut_r = (!cut_r) ? 8 : cut_r;
            ret <<= cut_r - cut_l;
            ret |= cut_bits(data[i], cut_l, cut_r) & 255;
            // printf("get %d %llx l=%d r=%d ret=%llx\n", i, cut_bits(data[i], cut_l, cut_r) & 255, cut_l, cut_r, ret);
        }
        return ret;
    }

    void put(u32 addr, u64 entry) {
        u32 bits_l = addr * unit_bits, bits_r = bits_l + unit_bits;
        int l = bits_l / 8, r = (bits_r - 1) / 8;
        for (int i = r; i >= l; -- i) {
            u32 cut_l = (i == l) ? bits_l % 8 : 0;
            u32 cut_r = (i == r) ? bits_r % 8 : 8;
            cut_r = (!cut_r) ? 8 : cut_r;
            update_bits(data[i], entry & ((1ull << (cut_r - cut_l)) - 1ull), cut_l, cut_r);
            // printf("update %d %llx l=%d r=%d\n", i, entry & ((1ull << (cut_r - cut_l)) - 1ull), cut_l, cut_r);
            entry >>= (cut_r - cut_l);
        }
    }
};

# endif