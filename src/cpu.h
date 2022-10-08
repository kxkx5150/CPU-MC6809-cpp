#ifndef __E6809_H
#define __E6809_H
#include <cstdint>


void     e6809_reset(void);
uint64_t e6809_sstep(uint64_t irq_i, uint64_t irq_f);
#endif