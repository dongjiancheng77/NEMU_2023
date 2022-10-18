#include <isa.h>
#include <elf.h>
#include "ftracer.h"
#include <common.h>
functab_node *functab_head;
/// @brief 
/// @param addr 
/// @return 
functab_node *functab_find(vaddr_t addr)
{
  functab_node *ptr = functab_head;
  while (ptr)
  {
    if (ptr->addr <= addr && addr < ptr->addr_end)
    {
      return ptr;
    }
    ptr = ptr->next;
  }
  return NULL;
}
void functab_push(const char *name, word_t addr, word_t size)
{
  functab_node *newnode = (functab_node *)malloc(sizeof(functab_node));
  newnode->addr = addr;
  newnode->addr_end = addr + size;
  newnode->name = (char *)malloc(strlen(name) + 1);
  strcpy(newnode->name, name);
  newnode->next = functab_head;
  functab_head = newnode;
}

void functab_print()
{
  functab_node *ptr = functab_head;
  if (functab_head == NULL)
  {
    printf("No Function in symbol table\n");
  }
  while (ptr)
  {
    printf("Function %s @ " FMT_WORD " - " FMT_WORD "\n", ptr->name, ptr->addr, ptr->addr_end);
    ptr = ptr->next;
  }
}
