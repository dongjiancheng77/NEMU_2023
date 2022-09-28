/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include "sdb.h"

#define NR_WP 64

typedef struct watchpoint
{
  int NO;
  struct watchpoint *next;
  char expr[64];
  uint32_t valve;
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool()
{
  int i;
  for (i = 0; i < NR_WP; i++)
  {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
void print_infowatchpoints()
{

  printf("infowatchpoints:\n");
  WP *cur = head;
  while (cur)
  {
    printf("NO: %d\tEXPR: %s\tVALVE: %u \n", cur->NO, cur->expr, cur->valve);
    cur = cur->next;
  }
}

WP *new_wp()
{
  if (free_ == NULL)
    assert(0);
  else
  {
    WP *tmp = free_;
    free_ = free_->next;
    tmp->next = head;
    head = tmp;
    return tmp;
  }
}

void free_wp(WP *wp)
{
  WP *last = head;
  // tmp is the latest element in the wp list
  // e.g. null < 0 < 1 < 2 tmp=2
  int sign = 0;
  while (last)
  {
    if (last == wp)
      break;
    last = last->next;
    sign++;
  }

  last = head;
  for (int i = 1; i < sign; i++)
  {
    last = last->next;
  }
  last->next = last->next->next;

  wp->next = free_;
  free_ = wp;
}
void de_wp(int no)
{
  WP *last = head;
  if (last->NO == no)
  {
    head = last->next;
    last->next = free_;
    free_ = last;
    return;
  }
  while (last != NULL)
  {
    if (last->NO == no)
      break;
    if (last->next == NULL)
    {
      printf("Can't find NO.%d\n", no);
      return;
      last = last->next;
    }
    free_wp(last);
  }
  return;
}