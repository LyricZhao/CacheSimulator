# ifndef __TRACE_HPP__
# define __TRACE_HPP__

# include <cstdio>

# include "utility.hpp"

typedef unsigned long long  u64;
typedef unsigned int        u32;
typedef unsigned char       u8;

class TraceReader {
private:
    FILE *file;

public:
    TraceReader() {
        file = nullptr;
    }

    void open(const char *filename) {
        file = fopen(filename, "r");
    }

    bool read(bool &type, u64 &addr) {
        char _type[2];
        int ret = fscanf(file, "%s %x", _type, &addr);
        type = (_type[0] == 'r');
        return ret != EOF;
    }

    void close() {
        fclose(file);
    }
};

# endif