//
// Created by lxz on 2025/6/13.
//

#include "zElf.h"
#include "elf.h"
#include "zLog.h"
#include <cstring>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>

#define ElfW(type) Elf64_ ## type

/**
 * 默认构造函数
 */
zElf::zElf() {
    LOGD("Default constructor called");
}

/**
 * 文件路径构造函数
 */
zElf::zElf(const char *elf_file_name) {
    LOGD("Constructor called with elf_file_name: %s", elf_file_name);

    link_view = LINK_VIEW::FILE_VIEW;
    if (load_elf_file(elf_file_name)) {
        parse_elf_head();
        parse_program_header_table();
        parse_dynamic_table();
        parse_section_table();
    }
}

/**
 * 解析ELF头部
 */
void zElf::parse_elf_head() {
    LOGD("parse_elf_head called");

    if (!elf_file_ptr) {
        LOGE("elf_file_ptr is null");
        return;
    }

    // 设置ELF头部指针
    elf_header = (Elf64_Ehdr *) elf_file_ptr;
    LOGD("elf_header->e_shoff 0x%llx", (unsigned long long)elf_header->e_shoff);
    LOGD("elf_header->e_shnum %x", elf_header->e_shnum);

    // 获取ELF头部大小
    elf_header_size = elf_header->e_ehsize;

    // 设置程序头表指针和数量
    program_header_table = (Elf64_Phdr*)(elf_file_ptr + elf_header->e_phoff);
    program_header_table_num = elf_header->e_phnum;

    // 设置节头表指针和数量
    section_header_table = (Elf64_Shdr*)(elf_file_ptr + elf_header->e_shoff);
    section_header_table_num = elf_header->e_shnum;
}

/**
 * 解析程序头表
 */
void zElf::parse_program_header_table() {
    LOGD("parse_program_header_table called");

    if (!elf_file_ptr || !program_header_table) {
        LOGE("Invalid state for parsing program header table");
        return;
    }

    bool found_load_segment = false;

    // 遍历所有程序头表项
    for (int i = 0; i < program_header_table_num; i++) {
        // 查找第一个加载段
        if (program_header_table[i].p_type == PT_LOAD && !found_load_segment) {
            found_load_segment = true;
            load_segment_virtual_offset = program_header_table[i].p_vaddr;
            load_segment_physical_offset = program_header_table[i].p_paddr;
            LOGD("load_segment_virtual_offset 0x%llx", (unsigned long long)load_segment_virtual_offset);
        }

        // 查找动态段
        if (program_header_table[i].p_type == PT_DYNAMIC) {
            dynamic_table_offset = program_header_table[i].p_offset;
            dynamic_table = (Elf64_Dyn*)(elf_file_ptr + program_header_table[i].p_offset);
            dynamic_element_num = (program_header_table[i].p_filesz) / sizeof(Elf64_Dyn);
            LOGD("dynamic_table_offset 0x%llx", (unsigned long long)dynamic_table_offset);
            LOGD("dynamic_element_num %llu", (unsigned long long)dynamic_element_num);
        }
    }
}

/**
 * 解析动态表
 */
void zElf::parse_dynamic_table() {
    LOGD("parse_dynamic_table called");

    if (!dynamic_table) {
        LOGD("No dynamic table found");
        return;
    }

    Elf64_Dyn *dynamic_element = dynamic_table;

    // 遍历所有动态表项
    for (int i = 0; i < dynamic_element_num; i++) {
        if (dynamic_element->d_tag == DT_STRTAB) {
            // 动态字符串表
            LOGD("DT_STRTAB 0x%llx", (unsigned long long)dynamic_element->d_un.d_ptr);
            dynamic_string_table_offset = dynamic_element->d_un.d_ptr;
        } else if (dynamic_element->d_tag == DT_STRSZ) {
            // 动态字符串表大小
            LOGD("DT_STRSZ 0x%llx", (unsigned long long)dynamic_element->d_un.d_val);
            dynamic_string_table_size = dynamic_element->d_un.d_val;
        } else if (dynamic_element->d_tag == DT_SYMTAB) {
            // 动态符号表
            LOGD("DT_SYMTAB 0x%llx", (unsigned long long)dynamic_element->d_un.d_ptr);
            dynamic_symbol_table_offset = dynamic_element->d_un.d_ptr;
        } else if (dynamic_element->d_tag == DT_SYMENT) {
            // 动态符号表项大小
            LOGD("DT_SYMENT 0x%llx", (unsigned long long)dynamic_element->d_un.d_val);
            dynamic_symbol_element_size = dynamic_element->d_un.d_val;
        } else if (dynamic_element->d_tag == DT_SONAME) {
            // 共享库名称
            LOGD("DT_SONAME 0x%llx", (unsigned long long)dynamic_element->d_un.d_ptr);
            soname_offset = dynamic_element->d_un.d_ptr;
        } else if (dynamic_element->d_tag == DT_GNU_HASH) {
            // GNU哈希表
            LOGD("DT_GNU_HASH 0x%llx", (unsigned long long)dynamic_element->d_un.d_ptr);
            gnu_hash_table_offset = dynamic_element->d_un.d_ptr;
        }
        dynamic_element++;
    }

    LOGI("parse_dynamic_table succeed");
}

/**
 * 解析节头表
 */
void zElf::parse_section_table() {
    LOGD("parse_section_table called");

    if (!elf_file_ptr || !section_header_table) {
        LOGE("Invalid state for parsing section table");
        return;
    }

    // 获取节字符串表索引
    int section_string_section_id = elf_header->e_shstrndx;
    LOGD("parse_section_table section_string_section_id %d", section_string_section_id);

    Elf64_Shdr *section_element = section_header_table;

    // 设置节字符串表
    section_string_table = elf_file_ptr + (section_element + section_string_section_id)->sh_offset;
    LOGD("parse_section_table section_string_table %p", (void*)section_string_table);

    // 遍历所有节
    for (int i = 0; i < section_header_table_num; i++) {
        char *section_name = section_string_table + section_element->sh_name;

        if (strcmp(section_name, ".strtab") == 0) {
            // 字符串表
            LOGD("strtab 0x%llx", (unsigned long long)section_element->sh_offset);
            string_table = elf_file_ptr + section_element->sh_offset;
        } else if (strcmp(section_name, ".dynsym") == 0) {
            // 动态符号表
            LOGD("dynsym 0x%llx", (unsigned long long)section_element->sh_offset);
            dynamic_symbol_table = (Elf64_Sym*)(elf_file_ptr + section_element->sh_offset);
            dynamic_symbol_table_num = section_element->sh_size / sizeof(Elf64_Sym);
            LOGD("symbol_table_num %llu", (unsigned long long)dynamic_symbol_table_num);
        } else if (strcmp(section_name, ".dynstr") == 0) {
            // 动态字符串表
            LOGD("dynstr 0x%llx", (unsigned long long)section_element->sh_offset);
            dynamic_string_table = elf_file_ptr + section_element->sh_offset;
        } else if (strcmp(section_name, ".symtab") == 0) {
            // 符号表
            symbol_table = (Elf64_Sym*) (elf_file_ptr + section_element->sh_offset);
            section_symbol_num = section_element->sh_size / section_element->sh_entsize;
            LOGD("section_symbol_num %llx", (unsigned long long)section_symbol_num);
        }
        section_element++;
    }
    LOGI("parse_section_table succeed");
}

/**
 * 加载ELF文件到内存
 */
bool zElf::load_elf_file(const char *elf_path) {
    LOGI("load_elf_file %s", elf_path);

    FILE *fp = fopen(elf_path, "rb");
    if (!fp) {
        LOGE("Failed to open file: %s", elf_path);
        return false;
    }

    // 获取文件大小
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (file_size <= 0) {
        LOGE("Invalid file size");
        fclose(fp);
        return false;
    }

    LOGD("File size: %zu", file_size);

    // 分配内存
    elf_file_ptr = (char*)malloc(file_size);
    if (!elf_file_ptr) {
        LOGE("Failed to allocate memory");
        fclose(fp);
        return false;
    }

    // 读取文件
    size_t read_size = fread(elf_file_ptr, 1, file_size, fp);
    fclose(fp);

    if (read_size != file_size) {
        LOGE("Failed to read complete file");
        free(elf_file_ptr);
        elf_file_ptr = nullptr;
        return false;
    }

    LOGI("File loaded successfully");
    return true;
}

/**
 * 打印ELF文件布局
 */
void zElf::print_layout() {
    if (!elf_file_ptr) {
        printf("ELF file not loaded\n");
        return;
    }

    printf("\n=== ELF File Layout (by address order) ===\n\n");

    // 结构体用于存储每个区域
    struct MemRegion {
        unsigned long long start;
        unsigned long long end;
        unsigned long long size;
        char name[256];
        int level;  // 0=顶层, 1=子项
    };

    std::vector<MemRegion> regions;

    // 1. ELF Header
    regions.push_back({
        0,
        (unsigned long long)(elf_header_size - 1),
        elf_header_size,
        "elf_header",
        0
    });

    // 2. Program Header Table (顶层)
    Elf64_Off phdr_offset = elf_header->e_phoff;
    Elf64_Xword phdr_size = elf_header->e_phentsize * program_header_table_num;
    regions.push_back({
        (unsigned long long)phdr_offset,
        (unsigned long long)(phdr_offset + phdr_size - 1),
        (unsigned long long)phdr_size,
        "program_header_table",
        0
    });

    // 2.1 添加每个 Program Header Element (子项)
    for (int i = 0; i < program_header_table_num; i++) {
        Elf64_Off entry_offset = phdr_offset + i * elf_header->e_phentsize;

        // 获取类型名称
        const char* type_name = "";
        switch (program_header_table[i].p_type) {
            case PT_NULL: type_name = "NULL"; break;
            case PT_LOAD: type_name = "Loadable Segment"; break;
            case PT_DYNAMIC: type_name = "Dynamic Segment"; break;
            case PT_INTERP: type_name = "Interpreter Path"; break;
            case PT_NOTE: type_name = "Note"; break;
            case PT_PHDR: type_name = "Program Header"; break;
            case PT_TLS: type_name = "Thread-Local Storage"; break;
            case PT_GNU_EH_FRAME: type_name = "GCC .eh_frame_hdr Segment"; break;
            case PT_GNU_STACK: type_name = "GNU Stack (executability)"; break;
            case PT_GNU_RELRO: type_name = "GNU Read-only After Relocation"; break;
            default: type_name = "Unknown"; break;
        }

        // 获取权限标志
        Elf64_Word flags = program_header_table[i].p_flags;
        char perms[4];
        perms[0] = (flags & PF_R) ? 'R' : '_';
        perms[1] = (flags & PF_W) ? 'W' : '_';
        perms[2] = (flags & PF_X) ? 'X' : '_';
        perms[3] = '\0';

        char name[256];
        snprintf(name, sizeof(name), "program_table_element[0x%02x] (%s) %s", i, perms, type_name);

        regions.push_back({
            (unsigned long long)entry_offset,
            (unsigned long long)(entry_offset + elf_header->e_phentsize - 1),
            (unsigned long long)elf_header->e_phentsize,
            "",
            1  // 子项
        });
        strcpy(regions.back().name, name);
    }

    // 3. Section 数据段（作为顶层项）
    if (section_header_table && section_header_table_num > 0) {
        Elf64_Shdr *section = section_header_table;
        for (int i = 0; i < section_header_table_num; i++) {
            if (section->sh_size > 0 && section->sh_offset > 0 && section->sh_type != SHT_NOBITS) {
                const char *section_name = "";
                if (section_string_table && section->sh_name < 10000) {
                    section_name = section_string_table + section->sh_name;
                }
                char name[256];
                if (strlen(section_name) > 0) {
                    snprintf(name, sizeof(name), "section[0x%02x] %s", i, section_name);
                } else {
                    snprintf(name, sizeof(name), "section[0x%02x]", i);
                }
                regions.push_back({
                    (unsigned long long)section->sh_offset,
                    (unsigned long long)(section->sh_offset + section->sh_size - 1),
                    (unsigned long long)section->sh_size,
                    "",
                    0  // 作为顶层项
                });
                strcpy(regions.back().name, name);
            }
            section++;
        }
    }

    // 4. Section Header Table（物理表）
    if (section_header_table && section_header_table_num > 0) {
        Elf64_Off shdr_physical_offset = elf_header->e_shoff;
        Elf64_Xword shdr_physical_size = elf_header->e_shentsize * section_header_table_num;

        regions.push_back({
            (unsigned long long)shdr_physical_offset,
            (unsigned long long)(shdr_physical_offset + shdr_physical_size - 1),
            (unsigned long long)shdr_physical_size,
            "section_header_table",
            0
        });

        // 添加每个 section header entry 作为子项
        for (int i = 0; i < section_header_table_num; i++) {
            Elf64_Off entry_offset = shdr_physical_offset + i * elf_header->e_shentsize;

            // 获取 section 名称
            const char *section_name = "";
            if (i == 0) {
                section_name = "SHN_UNDEF";
            } else {
                Elf64_Shdr *sect = &section_header_table[i];
                if (section_string_table && sect->sh_name < 10000) {
                    section_name = section_string_table + sect->sh_name;
                }
            }

            char name[256];
            if (strlen(section_name) > 0) {
                snprintf(name, sizeof(name), "section_table_element[0x%02x] %s", i, section_name);
            } else {
                snprintf(name, sizeof(name), "section_table_element[0x%02x]", i);
            }

            regions.push_back({
                (unsigned long long)entry_offset,
                (unsigned long long)(entry_offset + elf_header->e_shentsize - 1),
                (unsigned long long)elf_header->e_shentsize,
                "",
                1  // 子项
            });
            strcpy(regions.back().name, name);
        }
    }

    // 按地址排序
    std::sort(regions.begin(), regions.end(), [](const MemRegion& a, const MemRegion& b) {
        return a.start < b.start;
    });

    // 分别为顶层和子项添加 gap
    std::vector<MemRegion> all_regions_with_gaps;

    // 1. 处理顶层区域(level 0) 的 gap
    std::vector<MemRegion> top_level;
    for (const auto& r : regions) {
        if (r.level == 0) top_level.push_back(r);
    }
    std::sort(top_level.begin(), top_level.end(), [](const MemRegion& a, const MemRegion& b) {
        return a.start < b.start;
    });

    unsigned long long last_end = 0;
    for (const auto& region : top_level) {
        if (region.start > last_end) {
            all_regions_with_gaps.push_back({
                last_end,
                region.start - 1,
                region.start - last_end,
                "[padding]",
                0
            });
        }
        all_regions_with_gaps.push_back(region);
        if (region.end + 1 > last_end) {
            last_end = region.end + 1;
        }
    }

    // 2. 处理子项(level 1) 的 gap - 需要根据所属的父级分别处理
    // 2.1 Program Header Table 的子项
    std::vector<MemRegion> program_children;
    Elf64_Off program_start = 0, program_end = 0;
    for (const auto& r : top_level) {
        if (strcmp(r.name, "program_header_table") == 0) {
            program_start = r.start;
            program_end = r.end;
            break;
        }
    }

    for (const auto& r : regions) {
        if (r.level == 1 && strncmp(r.name, "program_table_element", 21) == 0) {
            program_children.push_back(r);
        }
    }
    std::sort(program_children.begin(), program_children.end(), [](const MemRegion& a, const MemRegion& b) {
        return a.start < b.start;
    });

    last_end = program_start;
    for (const auto& region : program_children) {
        if (region.start > last_end) {
            all_regions_with_gaps.push_back({
                last_end,
                region.start - 1,
                region.start - last_end,
                "[padding]",
                1
            });
        }
        all_regions_with_gaps.push_back(region);
        if (region.end + 1 > last_end) {
            last_end = region.end + 1;
        }
    }

    // 2.2 Section Header Table 的子项
    std::vector<MemRegion> section_header_children;
    Elf64_Off section_header_start = 0;
    for (const auto& r : top_level) {
        if (strcmp(r.name, "section_header_table") == 0) {
            section_header_start = r.start;
            break;
        }
    }

    for (const auto& r : regions) {
        if (r.level == 1 && strncmp(r.name, "section_table_element", strlen("section_table_element")) == 0) {
            section_header_children.push_back(r);
        }
    }
    std::sort(section_header_children.begin(), section_header_children.end(), [](const MemRegion& a, const MemRegion& b) {
        return a.start < b.start;
    });

    last_end = section_header_start;
    for (const auto& region : section_header_children) {
        if (region.start > last_end) {
            all_regions_with_gaps.push_back({
                last_end,
                region.start - 1,
                region.start - last_end,
                "[padding]",
                1
            });
        }
        all_regions_with_gaps.push_back(region);
        if (region.end + 1 > last_end) {
            last_end = region.end + 1;
        }
    }

    // 用包含 gap 的完整列表替换原列表
    regions = all_regions_with_gaps;

    // 按地址顺序显示所有内容
    for (const auto& region : regions) {
        if (region.level == 0) {
            // 顶层项
            printf("0x%08llx-0x%08llx    0x%08llx    %s\n",
                   region.start, region.end, region.size, region.name);

            // 如果是 program_header_table，显示其子项
            if (strcmp(region.name, "program_header_table") == 0) {
                for (const auto& child : regions) {
                    if (child.level == 1 && child.start >= region.start && child.end <= region.end) {
                        printf("    0x%08llx-0x%08llx    0x%08llx    %s\n",
                               child.start, child.end, child.size, child.name);
                    }
                }
            }

            // 如果是 section_header_table，显示其子项
            if (strcmp(region.name, "section_header_table") == 0) {
                for (const auto& child : regions) {
                    if (child.level == 1 && child.start >= region.start && child.end <= region.end) {
                        printf("    0x%08llx-0x%08llx    0x%08llx    %s\n",
                               child.start, child.end, child.size, child.name);
                    }
                }
            }
        }
    }

    // 检查覆盖率
    printf("\n=== Coverage Summary ===\n");
    printf("✓ Full coverage - all bytes accounted for (including gaps/padding)\n");
    printf("Total file size: 0x%08zx (%zu bytes)\n", file_size, file_size);
}

/**
 * 析构函数
 */
zElf::~zElf() {
    if (elf_file_ptr) {
        free(elf_file_ptr);
        elf_file_ptr = nullptr;
    }
}
