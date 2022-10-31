
#include <isa.h>
#include <elf.h>
#include "ftracer.h"
#include <common.h>
#include <memory/paddr.h>
#ifdef CONFIG_FTRACE
functab_node *functab_head;
/// @brief
/// @param addr
/// @return
void node_init(char *name, word_t addr, word_t size)
{
  functab_node *newnode = (functab_node *)malloc(sizeof(functab_node));
  newnode->addr = addr;
  newnode->addr_end = addr + size;
  newnode->name = (char *)malloc(strlen(name) + 1);
  strcpy(newnode->name, name);
  newnode->next = functab_head;
  functab_head = newnode;
}
functab_node *functab_find(vaddr_t addr)
{
  for (functab_node *ptr = functab_head; ptr; ptr = ptr->next)
  {
    if (ptr->addr <= addr && addr < ptr->addr_end)
    {
      return ptr;
    }
  }
  return NULL;
}
void load_elf(char *elf_file)
{
  if (elf_file == NULL)
  {
    Log("No elf is given. Use the default build-in image.");
    // return 4096; // built-in image size
    return;
  }
  printf("====== Reading ELF File ======\n");
  FILE *elf = fopen(elf_file, "rb");
  fseek(elf, 0, SEEK_END);
  long size = ftell(elf);
  Elf32_Shdr *strtab_shdr = NULL;
  Elf32_Shdr *symtab_shdr = NULL;
  void *elf_buf = malloc(size);
  fseek(elf, 0, SEEK_SET);
  int succ = fread(elf_buf, size, 1, elf);
  if (succ != 1)
  {
    panic("read elf failed!");
  }
  Elf32_Ehdr *elf_ehdr = elf_buf;
  Elf32_Shdr *shstrtab_shdr = (elf_ehdr->e_shstrndx * elf_ehdr->e_shentsize + elf_ehdr->e_shoff) + elf_buf;
  char *shstrtab_ptr = elf_buf + shstrtab_shdr->sh_offset;
  for (int i = 0; i < elf_ehdr->e_shnum; ++i)
  {
    Elf32_Shdr *elf_shdr = elf_buf + i * elf_ehdr->e_shentsize + elf_ehdr->e_shoff;
    switch (elf_shdr->sh_type)
    {
    case SHT_STRTAB:
      if (strcmp(shstrtab_ptr + elf_shdr->sh_name, ".strtab") == 0)
        strtab_shdr = elf_shdr;
      break;
    case SHT_SYMTAB:
      symtab_shdr = elf_shdr;
    }
  }
  char *strtab_ptr = elf_buf + strtab_shdr->sh_offset;
  for (int i = 0; i < symtab_shdr->sh_size; i += symtab_shdr->sh_entsize)
  {
    Elf32_Sym *elf_sym = elf_buf + symtab_shdr->sh_offset + i;
    if (ELF32_ST_TYPE(elf_sym->st_info) == STT_FUNC)
    {
      printf("func-symbol: %s \t size:%d \t" FMT_WORD " - " FMT_WORD "\n", strtab_ptr + elf_sym->st_name, elf_sym->st_size, elf_sym->st_value, elf_sym->st_value + elf_sym->st_size);
      node_init(strtab_ptr + elf_sym->st_name, elf_sym->st_value, elf_sym->st_size);
    }
  }
  // one malloc one free
  return;
}
#endif