#ifndef __VECX_H
#define __VECX_H

#include <cstdint>

enum
{
    VECTREX_MHZ    = 1500000,
    VECTREX_COLORS = 128,
    ALG_MAX_X      = 33000,
    ALG_MAX_Y      = 41000
};

typedef struct vector_type
{
    long    x0, y0;
    long    x1, y1;
    uint8_t color;
} vector_t;


extern uint8_t rom[8192];
extern uint8_t cart[32768];

extern uint64_t snd_regs[16];
extern uint64_t alg_jch0;
extern uint64_t alg_jch1;
extern uint64_t alg_jch2;
extern uint64_t alg_jch3;

extern long      vector_draw_cnt;
extern long      vector_erse_cnt;
extern vector_t *vectors_draw;
extern vector_t *vectors_erse;


void vecx_reset(void);
void vecx_emu(long cycles);

uint8_t _read8(uint64_t address);
void    _write8(uint64_t address, uint8_t data);

#endif
