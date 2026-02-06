#include <iostream>
#include "zElf.h"

int main(int argc, char* argv[]) {

    const char* elf_file = "D:\\work\\2026\\0202_my_vmp\\ElfEditor\\demo1";
    std::cout << "Parsing ELF file: " << elf_file << std::endl;

    // 创建 zElf 对象并解析文件
    zElf elf(elf_file);

    // 打印文件布局
    elf.print_layout();

    return 0;
}
