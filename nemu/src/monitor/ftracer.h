
#ifndef _FTRACER_H_
#define _FTRACER_H_
#include <common.h>
typedef struct functab_node
{
    struct functab_node * next;
    char* name;
    vaddr_t addr;
    vaddr_t addr_end;
} functab_node;
extern functab_node* functab_head;
void node_init(char *name, word_t addr, word_t size);
void load_elf(char* elf_file);
functab_node *functab_find(vaddr_t addr);

#endif
