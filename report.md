## Cache Analysis

> 计75班 赵成钢 2017011362

### 源代码说明

源代码放置在了 `code/` 目录中，在目录中包含一个 Makefile，输入 `make` 可以编译，`make run` 可以编译同时运行。需要注意的是在运行前，需要保证 `logs` 和`trace`的文件夹的存在，而且`trace`目录中包括几个重要的 trace 文件。

代码的运行会根据实验要求，会像下面展示的一样将实验中全部要求运算并输出日志，把结果写入到`output.csv`文件中。

```c++
const char* name[4] = {"astar", "bzip2", "mcf", "perlbench"};
const char* path[4] = {"trace/astar.trace", "trace/bzip2.trace", "trace/mcf.trace", "trace/perlbench.trace"};

// For answering the problems
void experiment_1(const char *csv) {
    printf("Running experiment 1 ...\n");
    for (int i = 0; i < 4; ++ i)
        for (auto layout: {DIRECT, FULLY, WAY_4, WAY_8})
            for (auto block_size: {8, 32, 64})
                testcase(name[i], path[i], csv, layout,
                    ReplaceType::LRU, WriteAllocateType::WRITE_ALLOCATE,
                    WritePolicyType::WRITE_BACK, 131072, block_size);
}

...
```

#### 输出结果

输出日志参见 `code/log/` 中，实验中的命中率等数据见`code/output.csv`。

### 实现细节

#### Bitmap

为了进一步减少源数据的开销，我仿照`std::bitset`实现了一个动态申请内存的`Bitmap`，可以指定一个宽度为 $count$ 的数组，数组每个元素有 $unit\_bits$ 个位，这个封装会把正好申请 $count*unit\_bits/8$ 个字节，并通过位运算来实现访问和修改，具体实现可以参考`utility.hpp`文件。

为什么要这样做呢？

- 可以避免后面大量编程上的重复复杂的位运算；
- 多个元素直接可以合并为一个字节，位基本控制，严格的空间大小；
- 在硬件上也有类似的包装，在计算机组成原理课程中，我们可以通过声明 `xpm_memory_sprom` 等类似的 Xilinx 封装好的 Block RAM 实现，同样可以设置单个元素的位大小和整体大小，这样做也有一定的一致性。

#### Cache64

实现上全部用参数化设计，不同的 Cache 布局将同样视为多路的实现。替换算法将作为一个抽象类子对象 `replace` 被调用，写策略作为 Cache 的属性，在查找中作为逻辑分支判断使用。具体参考 `cache.hpp`。

#### CacheReplace

为了更好的体现替换行为在逻辑上的一致性，我用了一个 `CacheReplace` 类来抽象替换的行为，包括查找 victim 和命中的行为。随机替换、LRU 和 TreeLRU 算法将全部继承这个抽象类。具体实现参考 `replace.hpp`。

### 实验结果部分

#### 固定替换策略（LRU）；固定写策略（写分配+写会）；尝试不同布局

