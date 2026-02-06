//
// Created by lxz on 2025/6/13.
//

#ifndef OVERT_ZELF_H
#define OVERT_ZELF_H

#include "elf.h"
#include <cstddef>
#include <cstdint>

class zElf {
public:
    enum LINK_VIEW {
        FILE_VIEW,
        UNKNOWN_VIEW
    };

    LINK_VIEW link_view = UNKNOWN_VIEW;

    // ELF 文件指针
    char* elf_file_ptr = nullptr;
    size_t file_size = 0;

    // 构造函数
    zElf();
    zElf(const char* elf_file_name);
    ~zElf();

    // ELF 解析相关成员
    Elf64_Ehdr* elf_header = nullptr;
    Elf64_Phdr* program_header_table = nullptr;
    Elf64_Half program_header_table_num = 0;
    Elf64_Half elf_header_size = 0;
    void parse_elf_head();

    Elf64_Addr load_segment_virtual_offset = 0;
    Elf64_Addr load_segment_physical_offset = 0;
    Elf64_Addr dynamic_table_offset = 0;
    Elf64_Dyn* dynamic_table = nullptr;
    Elf64_Xword dynamic_element_num = 0;
    void parse_program_header_table();

    char* section_string_table = nullptr;
    Elf64_Shdr* section_header_table = nullptr;
    Elf64_Half section_header_table_num = 0;
    Elf64_Xword section_symbol_num = 0;
    char* string_table = nullptr;
    Elf64_Sym* symbol_table = nullptr;
    void parse_section_table();

    Elf64_Addr dynamic_symbol_table_offset = 0;
    Elf64_Sym* dynamic_symbol_table = nullptr;
    Elf64_Xword dynamic_symbol_table_num = 0;
    Elf64_Xword dynamic_symbol_element_size = 0;
    Elf64_Addr dynamic_string_table_offset = 0;
    char* dynamic_string_table = nullptr;
    unsigned long long dynamic_string_table_size = 0;

    // SO 名称
    Elf64_Addr soname_offset = 0;
    char* so_name = nullptr;
    Elf64_Addr gnu_hash_table_offset = 0;
    char* gnu_hash_table = nullptr;
    void parse_dynamic_table();

    // ELF 文件加载
    bool load_elf_file(const char* elf_path);

    // 打印文件布局
    void print_layout();

    // Step-2: PHT 迁移与段扩增
    bool relocate_and_expand_pht(int extra_entries, const char* output_path);
};

#endif //OVERT_ZELF_H
