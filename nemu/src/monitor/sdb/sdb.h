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

#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>
typedef struct watchpoint {  int NO;
  struct watchpoint *next;
  char expr[64];
  uint32_t valve;
  /* TODO: Add more members if necessary */
} WP;
bool check_watchpoint(WP **point);

WP* new_wp(char *expr, bool *success);
void free_wp(WP *wp);
void de_wp(int no);
void print_infowatchpoints();
bool hook();
word_t expr(char *e, bool *success);
#endif
