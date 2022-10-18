
#include <common.h>
typedef struct functab_node
{
    struct functab_node * next;
    char* name;
    vaddr_t addr;
    vaddr_t addr_end;
} functab_node;
void functab_print();
extern functab_node* functab_head;
void functab_push(const char *name, word_t addr, word_t size);