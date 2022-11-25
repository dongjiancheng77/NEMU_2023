#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
static uintptr_t loader(PCB *pcb, const char *filename)
{
  // TODO();
  // Elf_Ehdr ehdr;
  // ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
  // // printf("Read 1\n");
  // Elf_Phdr phdr[ehdr.e_phnum];
  // ramdisk_read(phdr, ehdr.e_ehsize, sizeof(Elf_Phdr)*ehdr.e_phnum);
  // for (int i = 0; i < ehdr.e_phnum; ++i)
  // {
  //   if (phdr[i].p_type == PT_LOAD)
  //   {
  //     ramdisk_read((void *)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);
  //     memset((void *)phdr[i].p_vaddr + phdr[i].p_filesz, 0,
  //            phdr[i].p_memsz - phdr[i].p_filesz);
  //   }
  // }
  // return ehdr.e_entry;

  // return 0;
  Elf32_Ehdr elf;
  // ramdisk_read(&elf, 0, sizeof(Elf32_Ehdr));
  int fd = fs_open(filename, 0, 0);
  printf("load file: %s, fd: %d\n", filename, fd);
  fs_read(fd, &elf, sizeof(Elf32_Ehdr));

  /*elf magic number*/
  assert(*(uint32_t *)elf.e_ident == 0x464c457f);

  /*获取程序头表,程序头表在elf头的后面*/
  Elf_Phdr phdr[elf.e_phnum];
  // ramdisk_read(phdr, elf.e_ehsize, sizeof(Elf_Phdr) * elf.e_phnum);
  fs_lseek(fd, elf.e_ehsize, SEEK_SET);
  fs_read(fd, phdr, sizeof(Elf_Phdr) * elf.e_phnum);

  /*开始加载*/
  for (size_t i = 0; i < elf.e_phnum; i++) {
    if (phdr[i].p_type == PT_LOAD) {
      // ramdisk_read((void *)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);
      fs_lseek(fd, phdr[i].p_offset, SEEK_SET);
      fs_read(fd, (void *)phdr[i].p_vaddr, phdr[i].p_memsz);
      /*多与部分清零*/
      memset((void *)phdr[i].p_vaddr + phdr[i].p_filesz, 0, phdr[i].p_memsz - phdr[i].p_filesz);
    }
  }
  printf("entry: %x\n", elf.e_entry);
  return elf.e_entry;
}

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  // TODO();
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
