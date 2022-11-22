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

word_t csr_read(word_t cpu_no, word_t src1)
{
  word_t t = 0;
  switch (cpu_no)
  {
  case 0x300:
    t = cpu.mstatus;
    cpu.mstatus = src1;
    return t;

  case 0x305:
    t = cpu.mtvec;
    cpu.mtvec = src1;
    return t;

  case 0x340:
    t = cpu.mscratch;
    cpu.mscratch = src1;
    return t;

  case 0x341:
    t = cpu.mepc;
    cpu.mepc = src1;
    return t;

  case 0x342:
    t = cpu.mcause;
    cpu.mcause = src1;
    return t;
  }
  return t;
}

word_t csr_read1(word_t csr_no, word_t src1)
{
  word_t t = 0;
  switch (csr_no)
  {
  case 0x300:
    t = cpu.mstatus;
    cpu.mstatus = t | src1;
    return t;

  case 0x305:
    t = cpu.mtvec;
    cpu.mtvec = t | src1;
    return t;

  case 0x340:
    t = cpu.mscratch;
    cpu.mscratch = t | src1;
    return t;

  case 0x341:
    t = cpu.mepc;
    cpu.mepc = t | src1;
    return t;

  case 0x342:
    t = cpu.mcause;
    cpu.mcause = t | src1;
    return t;

    // case 0x180:
    //   return &(csr.satp);

  default:
    assert(0);
  }
}
word_t isa_raise_intr(word_t NO, vaddr_t epc)
{
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.mepc = epc;
  cpu.mcause = NO;
  // cpu.mstatus.m.MPIE = csr.mstatus.m.MIE;
  // cpu.mstatus.m.MIE = 0;
  return cpu.mtvec;
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
