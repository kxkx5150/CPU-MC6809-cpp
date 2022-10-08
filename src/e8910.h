#ifndef __E8910_H
#define __E8910_H
#include "PC.h"
#include <cstdint>
#include <sys/types.h>


class AY8910 {
  public:
    PC *pc = nullptr;

    int       index;
    int       ready;
    uint64_t *Regs;
    int32_t   lastEnable;
    int32_t   PeriodA, PeriodB, PeriodC, PeriodN, PeriodE;
    int32_t   CountA, CountB, CountC, CountN, CountE;
    uint32_t  VolA, VolB, VolC, VolE;
    uint8_t   EnvelopeA, EnvelopeB, EnvelopeC;
    uint8_t   OutputA, OutputB, OutputC, OutputN;
    int8_t    CountEnv;
    uint8_t   Hold, Alternate, Attack, Holding;
    int32_t   RNG;
    uint64_t  VolTable[32];

  public:
    AY8910(PC *_pc);

    void e8910_write(int r, int v);
    void e8910_callback(void *userdata, uint8_t *stream, int length);
    void e8910_build_mixer_table();
    void e8910_init_sound();
    void e8910_done_sound();
};

#endif
