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
  Elf_Ehdr elf_header ;
  int fd = fs_open(filename, 1, 1);
  size_t success = fs_read(fd, &elf_header, sizeof(Elf_Ehdr)); 
  assert(*(uint32_t *)elf_header.e_ident == 0x464c457f);
  if (success) {
    for (size_t i = 0; i < elf_header.e_phnum; i++) {
      // printf("%d\n",i);
      Elf_Phdr phdr;
      fs_lseek(fd, elf_header.e_phoff + i * sizeof(Elf_Phdr), SEEK_SET);
      success = fs_read(fd, &phdr, sizeof(Elf_Phdr)); //phoff!
      if (success) {
        if (phdr.p_type == PT_LOAD) {
          fs_lseek(fd, phdr.p_offset, SEEK_SET);
          success = fs_read(fd, (void *)(phdr.p_vaddr), phdr.p_memsz);
          memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
        }
      }
    }
  }
  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  // TODO();
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
