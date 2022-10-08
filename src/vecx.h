#ifndef __VECX_H
#define __VECX_H
#include <cstdint>


class VECX {
  public:
    typedef struct vector_type
    {
        long    x0, y0;
        long    x1, y1;
        uint8_t color;
    } vector_t;

    enum
    {
        VECTREX_MHZ    = 1500000,
        VECTREX_COLORS = 128,
        ALG_MAX_X      = 33000,
        ALG_MAX_Y      = 41000
    };

    enum
    {
        VECTREX_PDECAY = 30,
        FCYCLES_INIT   = VECTREX_MHZ / VECTREX_PDECAY,
        VECTOR_CNT     = VECTREX_MHZ / VECTREX_PDECAY,
        VECTOR_HASH    = 65521
    };

  public:
    uint8_t  rom[8192];
    uint8_t  cart[32768];
    uint64_t snd_regs[16];
    uint8_t  ram[1024];

    uint64_t snd_select;

    uint64_t via_ora;
    uint64_t via_orb;
    uint64_t via_ddra;
    uint64_t via_ddrb;
    uint64_t via_t1on;
    uint64_t via_t1int;
    uint64_t via_t1c;
    uint64_t via_t1ll;
    uint64_t via_t1lh;
    uint64_t via_t1pb7;
    uint64_t via_t2on;
    uint64_t via_t2int;
    uint64_t via_t2c;
    uint64_t via_t2ll;
    uint64_t via_sr;
    uint64_t via_srb;
    uint64_t via_src;
    uint64_t via_srclk;
    uint64_t via_acr;
    uint64_t via_pcr;
    uint64_t via_ifr;
    uint64_t via_ier;
    uint64_t via_ca2;
    uint64_t via_cb2h;
    uint64_t via_cb2s;

    uint64_t alg_rsh;
    uint64_t alg_xsh;
    uint64_t alg_ysh;
    uint64_t alg_zsh;

    uint64_t alg_jch0;
    uint64_t alg_jch1;
    uint64_t alg_jch2;
    uint64_t alg_jch3;

    uint64_t alg_jsh;
    uint64_t alg_compare;
    long     alg_dx;
    long     alg_dy;
    long     alg_curr_x;
    long     alg_curr_y;

    uint64_t alg_vectoring;
    long     alg_vector_x0;
    long     alg_vector_y0;
    long     alg_vector_x1;
    long     alg_vector_y1;
    long     alg_vector_dx;
    long     alg_vector_dy;
    uint8_t  alg_vector_color;

    long      vector_draw_cnt;
    long      vector_erse_cnt;
    vector_t  vectors_set[2 * VECTOR_CNT];
    vector_t *vectors_draw;
    vector_t *vectors_erse;
    long      vector_hash[VECTOR_HASH];
    long      fcycles;

  public:
    void    snd_update(void);
    void    alg_update(void);
    void    int_update(void);
    uint8_t _read8(uint64_t address);
    void    _write8(uint64_t address, uint8_t data);
    void    vecx_reset(void);
    void    via_sstep0(void);
    void    via_sstep1(void);
    void    alg_addline(long x0, long y0, long x1, long y1, uint8_t color);
    void    alg_sstep(void);
    void    vecx_emu(long cycles);
};
#endif
