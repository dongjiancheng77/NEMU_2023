
#include <isa.h>
#include <memory/paddr.h>
#include <elf.h>
#include "ftracer.h"
#include <common.h>
#include <cpu/cpu.h>
// #include "monitor.h"
#ifdef CONFIG_FTRACE
static void functab_push(const char* name, word_t addr, word_t size) {
  functab_node* newnode = (functab_node*) malloc(sizeof(functab_node));
  newnode->addr = addr;
  newnode->addr_end = addr + size;
  newnode->name = (char*)malloc(strlen(name) + 1);
  strcpy(newnode->name, name);
  newnode->next = functab_head;
  functab_head = newnode;
}

static void functab_print() {
  functab_node* ptr = functab_head;
  if (functab_head == NULL) {
    printf("No Function in symbol table\n");
  }
  while(ptr) {
    printf("Function %s @ "FMT_WORD" - "FMT_WORD"\n", ptr->name, ptr->addr, ptr->addr_end);
    ptr = ptr->next;
  }
}
#endif