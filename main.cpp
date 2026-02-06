#include <iostream>
#include "zElf.h"

int main(int argc, char* argv[]) {

    const char* elf_file = "D:\\work\\2026\\0202_my_vmp\\ElfEditor\\demo1";
    std::cout << "Parsing ELF file: " << elf_file << std::endl;

    // 创建 zElf 对象并解析文件
    zElf elf(elf_file);

    // Step-1: 打印文件布局
    std::cout << "\n=== Step-1: Print ELF Layout ===" << std::endl;
    elf.print_layout();

    // Step-2: PHT 迁移与段扩增
    std::cout << "\n\n=== Step-2: PHT Relocation & Expansion ===" << std::endl;
    const char* output_file = "D:\\work\\2026\\0202_my_vmp\\ElfEditor\\demo1_pht_relocated";
    int extra_entries = 4;  // 预留 4 个新 PHT 条目

    std::cout << "Relocating PHT to file end and adding " << extra_entries << " extra entries..." << std::endl;

    if (elf.relocate_and_expand_pht(extra_entries, output_file)) {
        std::cout << "\n✓ Successfully relocated and expanded PHT" << std::endl;
        std::cout << "✓ Output file: " << output_file << std::endl;

        // 解析并验证输出文件
        std::cout << "\n=== Verification: Parse Relocated File ===" << std::endl;
        zElf relocated_elf(output_file);
        relocated_elf.print_layout();
    } else {
        std::cerr << "\n✗ Failed to relocate and expand PHT" << std::endl;
        return 1;
    }

    return 0;
}
