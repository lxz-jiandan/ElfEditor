# CLAUDE.md

本文件为 Claude Code (claude.ai/code) 在此代码库中工作时提供指导。

## Step-1：解析 ELF 文件，并在最后并将 ELF 的文件布局以地址的降序的形式打印结构布局

打印结构布局

```
0x00000000-0x0000003f    0x00000040    elf_header
0x00000040-0x0000026f    0x00000230    program_header_table
    0x00000040-0x00000077    0x00000038    program_table_element[0x00] (R__) Program Header
    0x00000078-0x000000af    0x00000038    program_table_element[0x01] (R__) Interpreter Path
    0x000000b0-0x000000e7    0x00000038    program_table_element[0x02] (R_X) Loadable Segment
    0x000000e8-0x0000011f    0x00000038    program_table_element[0x03] (RW_) Loadable Segment
    0x00000120-0x00000157    0x00000038    program_table_element[0x04] (RW_) Loadable Segment
    0x00000158-0x0000018f    0x00000038    program_table_element[0x05] (RW_) Dynamic Segment
    0x00000190-0x000001c7    0x00000038    program_table_element[0x06] (R__) GNU Read-only After Relocation
    0x000001c8-0x000001ff    0x00000038    program_table_element[0x07] (R__) GCC .eh_frame_hdr Segment
    0x00000200-0x00000237    0x00000038    program_table_element[0x08] (RW_) GNU Stack (executability)
    0x00000238-0x0000026f    0x00000038    program_table_element[0x09] (R__) Note
0x00000270-0x00000284    0x00000015    section[0x01] .interp
0x00000285-0x00000287    0x00000003    [padding]
0x00000288-0x0000031f    0x00000098    section[0x02] .note.android.ident
0x00000320-0x00000343    0x00000024    section[0x03] .note.gnu.build-id
0x00000344-0x00000347    0x00000004    [padding]
0x00000348-0x000003a7    0x00000060    section[0x04] .dynsym
0x000003a8-0x000003af    0x00000008    section[0x05] .gnu.version
0x000003b0-0x000003cf    0x00000020    section[0x06] .gnu.version_r
0x000003d0-0x000003eb    0x0000001c    section[0x07] .gnu.hash
0x000003ec-0x00000413    0x00000028    section[0x08] .hash
0x00000414-0x00000452    0x0000003f    section[0x09] .dynstr
0x00000453-0x00000457    0x00000005    [padding]
0x00000458-0x000004cf    0x00000078    section[0x0a] .rela.dyn
0x000004d0-0x00000517    0x00000048    section[0x0b] .rela.plt
0x00000518-0x0000052d    0x00000016    section[0x0c] .rodata
0x0000052e-0x0000052f    0x00000002    [padding]
0x00000530-0x0000056b    0x0000003c    section[0x0d] .eh_frame_hdr
0x0000056c-0x0000056f    0x00000004    [padding]
0x00000570-0x0000063f    0x000000d0    section[0x0e] .eh_frame
0x00000640-0x00000767    0x00000128    section[0x0f] .text
0x00000768-0x0000076f    0x00000008    [padding]
0x00000770-0x000007bf    0x00000050    section[0x10] .plt
0x000007c0-0x0000093f    0x00000180    section[0x11] .dynamic
0x00000940-0x00000957    0x00000018    section[0x12] .got
0x00000958-0x00000987    0x00000030    section[0x13] .got.plt
0x00000988-0x000009af    0x00000028    section[0x15] .data
0x000009b0-0x00000b19    0x0000016a    section[0x17] .comment
0x00000b1a-0x00000cae    0x00000195    section[0x18] .debug_abbrev
0x00000caf-0x000013cb    0x0000071d    section[0x19] .debug_info
0x000013cc-0x000013fb    0x00000030    section[0x1a] .debug_ranges
0x000013fc-0x00001826    0x0000042b    section[0x1b] .debug_str
0x00001827-0x00001b58    0x00000332    section[0x1c] .debug_line
0x00001b59-0x00001b5f    0x00000007    [padding]
0x00001b60-0x00001faf    0x00000450    section[0x1d] .symtab
0x00001fb0-0x000020ed    0x0000013e    section[0x1e] .shstrtab
0x000020ee-0x00002286    0x00000199    section[0x1f] .strtab
0x00002287-0x00002287    0x00000001    [padding]
0x00002288-0x00002a87    0x00000800    section_header_table
    0x00002288-0x000022c7    0x00000040    section_table_element[0x00] SHN_UNDEF
    0x000022c8-0x00002307    0x00000040    section_table_element[0x01] .interp
    0x00002308-0x00002347    0x00000040    section_table_element[0x02] .note.android.ident
    0x00002348-0x00002387    0x00000040    section_table_element[0x03] .note.gnu.build-id
    0x00002388-0x000023c7    0x00000040    section_table_element[0x04] .dynsym
    0x000023c8-0x00002407    0x00000040    section_table_element[0x05] .gnu.version
    0x00002408-0x00002447    0x00000040    section_table_element[0x06] .gnu.version_r
    0x00002448-0x00002487    0x00000040    section_table_element[0x07] .gnu.hash
    0x00002488-0x000024c7    0x00000040    section_table_element[0x08] .hash
    0x000024c8-0x00002507    0x00000040    section_table_element[0x09] .dynstr
    0x00002508-0x00002547    0x00000040    section_table_element[0x0a] .rela.dyn
    0x00002548-0x00002587    0x00000040    section_table_element[0x0b] .rela.plt
    0x00002588-0x000025c7    0x00000040    section_table_element[0x0c] .rodata
    0x000025c8-0x00002607    0x00000040    section_table_element[0x0d] .eh_frame_hdr
    0x00002608-0x00002647    0x00000040    section_table_element[0x0e] .eh_frame
    0x00002648-0x00002687    0x00000040    section_table_element[0x0f] .text
    0x00002688-0x000026c7    0x00000040    section_table_element[0x10] .plt
    0x000026c8-0x00002707    0x00000040    section_table_element[0x11] .dynamic
    0x00002708-0x00002747    0x00000040    section_table_element[0x12] .got
    0x00002748-0x00002787    0x00000040    section_table_element[0x13] .got.plt
    0x00002788-0x000027c7    0x00000040    section_table_element[0x14] .relro_padding
    0x000027c8-0x00002807    0x00000040    section_table_element[0x15] .data
    0x00002808-0x00002847    0x00000040    section_table_element[0x16] .bss
    0x00002848-0x00002887    0x00000040    section_table_element[0x17] .comment
    0x00002888-0x000028c7    0x00000040    section_table_element[0x18] .debug_abbrev
    0x000028c8-0x00002907    0x00000040    section_table_element[0x19] .debug_info
    0x00002908-0x00002947    0x00000040    section_table_element[0x1a] .debug_ranges
    0x00002948-0x00002987    0x00000040    section_table_element[0x1b] .debug_str
    0x00002988-0x000029c7    0x00000040    section_table_element[0x1c] .debug_line
    0x000029c8-0x00002a07    0x00000040    section_table_element[0x1d] .symtab
    0x00002a08-0x00002a47    0x00000040    section_table_element[0x1e] .shstrtab
    0x00002a48-0x00002a87    0x00000040    section_table_element[0x1f] .strtab
```

# Step-2 开发文档：PHT 迁移与段扩增 (PHT Relocation & Expansion)

## 1. 目标

将 ELF 的 **Program Header Table (PHT)** 从文件头部迁移至文件末尾，并增加条目数量。通过这种方式，我们可以自由定义多个新增的 `PT_LOAD` 段，用于存放 VMP 解释器代码、加密字节码及运行时数据。

## 2. 核心原理

Android Linker 在加载 SO 时会根据 `e_phoff` 寻址 PHT。为了保证迁移后的 PHT 能够被正确读取，必须确保：

1. **文件偏移更新**：`e_phoff` 指向文件末尾的新位置。
2. **内存映射自救**：新 PHT 所在的物理区间必须被包含在某个 `PT_LOAD` 段内，否则 Linker 访问 PHT 时会发生段错误（SIGSEGV）。
3. **虚拟地址修正**：`PT_PHDR` 段的 `p_vaddr` 必须更新为新 PHT 映射后的虚拟地址。

------

## 3. 修改流程安排（手术刀式实现）

### 第一阶段：空间预计算

1. **确定新 PHT 位置**：
   - 使用固定的页对齐地址：`New_PH_Offset = 0x3000`（12KB）
   - 这个位置已经超出原始文件末尾（0x2a88），为 PHT 预留独立空间
2. **确定新增段数量**：
   - 建议至少预留 4 个新 `Elf64_Phdr` 槽位（VMP_CODE, VMP_DATA, VMP_STR, RELOC_STUB）
   - `New_PH_Num = Old_PH_Num + 4`
   - 新 PHT 大小：`0x310` (14 * 0x38)

### 第二阶段：构建新 PHT

1. **内存构造**：
   - 申请一块内存 `buffer = New_PH_Num * sizeof(Elf64_Phdr)`
   - 将旧的 PHT 完整拷贝到 `buffer` 前部
2. **初始化新槽位**：
   - 在 `buffer` 后部的空槽位中初始化为 `PT_NULL`（暂时占位）
   - Index 10-12: `PT_NULL`（供后续 VMP 使用）
   - Index 13: 将用作自救 LOAD 段

### 第三阶段：映射自救（手术刀方案 - 关键）

**核心思路**：不破坏原有 LOAD 段，而是创建一个新的专用 LOAD 段来映射新 PHT。

1. **使用最后一个新增槽位** (Index 13) 创建自救 LOAD 段：
   ```
   p_type:   PT_LOAD
   p_offset: 0x3000
   p_vaddr:  0x3000
   p_paddr:  0x3000
   p_filesz: 0x310      (新 PHT 大小)
   p_memsz:  0x310
   p_flags:  PF_R (0x4) (只读权限)
   p_align:  0x1000     (4KB 页对齐)
   ```

2. **优势**：
   - 原有 LOAD 段（如 Index 4 的数据段）保持不变
   - 新 LOAD 段专门负责映射 PHT 区域
   - 干净、独立，不影响原有内存布局

### 第四阶段：元数据同步

1. **修正 `PT_PHDR`**：

   - 在新 PHT 映射表中找到类型为 `PT_PHDR` 的描述符（通常是 Index 0）

   - 更新其元数据：
     ```
     p_offset: 0x3000
     p_vaddr:  0x3000
     p_paddr:  0x3000
     p_filesz: 0x310
     p_memsz:  0x310
     ```

   - **关键对齐验证**：
     - `p_offset % 0x1000 = 0x0`
     - `p_vaddr % 0x1000 = 0x0`
     - 两者页内偏移一致，确保 linker 正确映射

2. **更新 ELF Header**：
   - `e_phoff = 0x3000`
   - `e_phnum = 14`

------

## 4. 验证标准

完成 Step-2 后，生成的 `output.so` 必须通过以下校验：

1. **静态校验**：`readelf -l output.so` 能够正确列出所有段，且 `Program Headers` 的数量已增加到 14 个
2. **地址校验**：`readelf -h` 显示的 `Start of program headers` 指向 0x3000
3. **自救段验证**：新增的 LOAD 段（Index 13）正确配置：
   - p_offset = p_vaddr = 0x3000
   - p_filesz = p_memsz = 0x310
   - p_flags = PF_R (只读)
4. **原有段完整性**：所有原有 LOAD 段（Index 2, 3, 4）保持不变
5. **加载校验**：在 Android 环境下使用 `dlopen` 加载该 SO。如果返回非空句柄且无 Crash，说明"映射自救"成功

## 5. 实现结果示例

成功执行后的文件布局：

```
原始文件: 0x2a88 (10888 bytes)
新文件:   0x3310 (13072 bytes)

新增内容:
0x00002a88-0x00002fff    0x00000578    [padding]
0x00003000-0x0000330f    0x00000310    program_header_table (relocated)
    0x00003000-0x00003037    0x00000038    program_table_element[0x00] (R__) Program Header
    0x00003038-0x0000306f    0x00000038    program_table_element[0x01] (R__) Interpreter Path
    ...
    0x00003230-0x00003267    0x00000038    program_table_element[0x0a] (___) NULL
    0x00003268-0x0000329f    0x00000038    program_table_element[0x0b] (___) NULL
    0x000032a0-0x000032d7    0x00000038    program_table_element[0x0c] (___) NULL
    0x000032d8-0x0000330f    0x00000038    program_table_element[0x0d] (R__) Loadable Segment (自救段)
```



# 构建命令

项目使用 CMake 配合 Ninja 生成器：

```bash
# 配置并构建（从项目根目录）
cmake -B build -G Ninja
ninja -C build

# 或者如果已经配置过
cd build; ninja

# 输出可执行文件位于：build/ElFEditor.exe
```

