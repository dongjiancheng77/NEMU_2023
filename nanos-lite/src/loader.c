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
  TODO();
  Elf_Ehdr ehdr;
  Elf_Phdr phdr[ehdr.e_phnum];
  ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
  ramdisk_read(phdr, ehdr.e_ehsize, sizeof(Elf_Ehdr));
  for (int i = 0; i < ehdr.e_phnum; ++i)
  {
    if (phdr[i].p_type == PT_LOAD)
    {
      ramdisk_read((void *)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);
      memset((void *)phdr[i].p_vaddr + phdr[i].p_filesz, 0,
             phdr[i].p_memsz-phdr[i].p_filesz);
    }
  }
  return ehdr.e_entry;
  // return 0;
}

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
