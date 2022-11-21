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

#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc)
{
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  csr.mepc = epc;
  csr.mcause = NO;
  // csr.mstatus.m.MPIE = csr.mstatus.m.MIE;
  // csr.mstatus.m.MIE = 0;
  return csr.mtvec;
  // cpu.mepc = epc;
  // cpu.mcause = NO;
  // GET_MSTATUS(mstatus);
  // mstatus.mpie = mstatus.mie;
  // mstatus.mie = 0;
  // SET_MSTATUS(mstatus);
  // return cpu.mtvec;
  // return 0;
}

word_t isa_query_intr()
{
  // if (csr.mstatus.m.MIE == 1 && cpu.INTR)
  // {
  //   cpu.INTR = false;
  //   return IRQ_TIMER;
  // }
  return INTR_EMPTY;
}
