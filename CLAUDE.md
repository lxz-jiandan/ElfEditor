# CLAUDE.md

本文件为 Claude Code (claude.ai/code) 在此代码库中工作时提供指导。

## 项目概述

解析 ELF 文件，并在最后并将 ELF 的文件布局以地址的降序打印出来，保证整个文件的每一块区域都清楚打印是做什么的

0x00000000-0x0000003f    0x00000040    elf_header
0x00000040-0x0000026f    0x00000230    program_header_table
    0x00000040-0x00000078    0x00000038    program_table_element[0]
    0x00000079-0x000000af    0x00000038    program_table_element[1]
    ......
0x00002288-0x00002a88    0x00000230    section_header_table
    0x00000288-0x0000031f    0x00000098    section_table_element[0]
    0x00000270-0x00000284    0x00000015    section_table_element[1]
    0x00000288-0x0000031f    0x00000098    section_table_element[2]

## 构建命令

项目使用 CMake 配合 Ninja 生成器：

```bash
# 配置并构建（从项目根目录）
cmake -B build -G Ninja
ninja -C build

# 或者如果已经配置过
cd build; ninja

# 输出可执行文件位于：build/ELFReconstruction.exe
```

