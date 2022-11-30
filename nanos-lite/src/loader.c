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
  //     memset((void *)phdr[i].p_vaddr + phdr[i].p_filesz, 0, phdr[i].p_memsz - phdr[i].p_filesz);
  //   }
  // }
  // return ehdr.e_entry;

  // return 0;

  Elf32_Ehdr ehdr;
  int fd = fs_open(filename, 0, 0);
  Log("load file: %s, fd: %d\n", filename, fd);
  fs_read(fd, &ehdr, sizeof(Elf32_Ehdr));

  assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);

  Elf_Phdr phdr[ehdr.e_phnum];
  fs_lseek(fd, ehdr.e_ehsize, SEEK_SET);
  fs_read(fd, phdr, sizeof(Elf_Phdr) * ehdr.e_phnum);

  for (size_t i = 0; i < ehdr.e_phnum; i++)
  {
    if (phdr[i].p_type == PT_LOAD)
    {
      fs_lseek(fd, phdr[i].p_offset, SEEK_SET);
      fs_read(fd, (void *)phdr[i].p_vaddr, phdr[i].p_memsz);
      memset((void *)phdr[i].p_vaddr + phdr[i].p_filesz, 0, phdr[i].p_memsz - phdr[i].p_filesz);
    }
  }
  return ehdr.e_entry;
}
void context_uload(PCB* ptr_pcb, const char* filename, char* const argv[], char* const envp[]) {
  protect(&ptr_pcb->as);
  uintptr_t ustack = (uintptr_t)new_page(8);
  uintptr_t ustack_mapped = (uintptr_t)ptr_pcb->as.area.end;
  for (int i = 0; i < 8; ++ i) {
    map(&ptr_pcb->as,
        (void*)(ptr_pcb->as.area.end - (8 - i) * PGSIZE),
        (void*)(ustack + i * PGSIZE),
        MMAP_READ | MMAP_WRITE);
    // Log("map 0x%8lx -> 0x%8lx", ptr_pcb->as.area.end - (8 - i) * PGSIZE, ustack + i * PGSIZE);
  }

//* Set argv, envp
  //* collect arg count
  int argv_count = 0;
  int envp_count = 0;
  if (argv) {
    while (argv[argv_count]) argv_count ++;
  }
  if (envp) {
    while (envp[envp_count]) envp_count ++;
  } 
  // char* _argv[20] = {0};
  // char* _envp[20] = {0};
  char** _argv = (char**)ustack;
  char** _envp = (char**)(ustack + (argv_count+1)*sizeof(char*));
  //* use ustack bottom as temporary buffer for new argv and envp
  
  // Log("argv_count:%d, envp_count:%d", argv_count, envp_count);
  // Log("envp: %p", envp[0]);
  //* copy strings
  ustack += 8 * PGSIZE; // put on the bottom of the stack

  for (int i = 0; i < envp_count; ++ i) {
    size_t len = strlen(envp[i]) + 1;
    ustack -= len;
    ustack_mapped -= len;
    strcpy((char*)ustack, envp[i]);
    _envp[i] = (char*)ustack_mapped;
  }

  for (int i = 0; i < argv_count; ++ i) {
    size_t len = strlen(argv[i]) + 1;
    ustack -= len;
    ustack_mapped -= len;
    strcpy((char*)ustack, argv[i]);
    _argv[i] = (char*)ustack_mapped;
  }
  //* copy argv table
  size_t envp_size = sizeof(char*) * (envp_count + 1); // there should be a null at the end 
  size_t argv_size = sizeof(char*) * (argv_count + 1);
  ustack -= envp_size;
  ustack_mapped -= envp_size;
  memcpy((void*) ustack, _envp, envp_size);
  ustack -= argv_size;
  ustack_mapped -= argv_size;
  memcpy((void*) ustack, _argv, argv_size);
  
  //* set argc
  ustack -= sizeof(uintptr_t);
  ustack_mapped -= sizeof(uintptr_t);
  *(uintptr_t *)ustack = argv_count;
  
  uintptr_t entry = loader(ptr_pcb, filename);
  
  Area kstack;
  kstack.start = ptr_pcb; // this is for PCB on stack, processed by kernel
  kstack.end = &ptr_pcb->stack[sizeof(ptr_pcb->stack)];
  ptr_pcb->cp = ucontext(&ptr_pcb->as, kstack, (void*)entry);
  ptr_pcb->cp->GPRx = ustack_mapped;
  // Log("updir %p sp: %p", ptr_pcb->as.ptr, ustack_mapped);
#ifdef NO_TI
  ptr_pcb->cp->mstatus = 0xa00001800;  
#endif
}
void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  // TODO();
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
