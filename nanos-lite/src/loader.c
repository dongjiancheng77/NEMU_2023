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

static uintptr_t loader(PCB *pcb, const char *filename)
{
  TODO();
  Elf_Ehdr ehdr, *ptr_ehdr = &ehdr;
  Elf_Phdr phdr, *ptr_phdr = &phdr;
  uint32_t i, phoff;
  int fd;
  fd = fs_open(filename, 0, 0);
  fs_read(fd, ptr_ehdr, sizeof(Elf_Ehdr));
  for (i = 0; i < ehdr.e_phnum; ++i)
  {
    phoff = i * ehdr.e_phentsize + ehdr.e_phoff;
    fs_lseek(fd, phoff, SEEK_SET);
    fs_read(fd, ptr_phdr, sizeof(Elf_Phdr));
    if (phdr.p_type != PT_LOAD)
      continue;
    uintptr_t vpage_start = phdr.p_vaddr & (~0xfff);
    uintptr_t vpage_end = (phdr.p_vaddr + phdr.p_memsz - 1) & (~0xfff);
    int page_num = ((vpage_end - vpage_start) >> 12) + 1;
    uintptr_t page_ptr = (uintptr_t)new_page(page_num);
    for (int j = 0; j < page_num; ++j)
    {
      map(&pcb->as,
          (void *)(vpage_start + (j << 12)),
          (void *)(page_ptr + (j << 12)),
          MMAP_READ | MMAP_WRITE);
    }
    void *page_off = (void *)(phdr.p_vaddr & 0xfff);
    fs_lseek(fd, phdr.p_offset, SEEK_SET);
    fs_read(fd, page_ptr + page_off, phdr.p_filesz);
    pcb->max_brk = vpage_end + PGSIZE;
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
