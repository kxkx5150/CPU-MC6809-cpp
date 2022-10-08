#include <stdio.h>
#include "cpu.h"
#include "vecx.h"
#include "e8910.h"


extern CPU *cpu;
extern void osint_render(void);



void VECX::snd_update(void)
{
    switch (via_orb & 0x18) {
        case 0x00:
            break;
        case 0x08:
            break;
        case 0x10:
            if (snd_select != 14) {
                snd_regs[snd_select] = via_ora;
                e8910_write(snd_select, via_ora);
            }
            break;
        case 0x18:
            if ((via_ora & 0xf0) == 0x00) {
                snd_select = via_ora & 0x0f;
            }
            break;
    }
}
void VECX::alg_update(void)
{
    switch (via_orb & 0x06) {
        case 0x00:
            alg_jsh = alg_jch0;
            if ((via_orb & 0x01) == 0x00) {
                alg_ysh = alg_xsh;
            }
            break;
        case 0x02:
            alg_jsh = alg_jch1;
            if ((via_orb & 0x01) == 0x00) {
                alg_rsh = alg_xsh;
            }
            break;
        case 0x04:
            alg_jsh = alg_jch2;
            if ((via_orb & 0x01) == 0x00) {
                if (alg_xsh > 0x80) {
                    alg_zsh = alg_xsh - 0x80;
                } else {
                    alg_zsh = 0;
                }
            }
            break;
        case 0x06:
            alg_jsh = alg_jch3;
            break;
    }

    if (alg_jsh > alg_xsh) {
        alg_compare = 0x20;
    } else {
        alg_compare = 0;
    }

    alg_dx = (long)alg_xsh - (long)alg_rsh;
    alg_dy = (long)alg_rsh - (long)alg_ysh;
}
void VECX::int_update(void)
{
    if ((via_ifr & 0x7f) & (via_ier & 0x7f)) {
        via_ifr |= 0x80;
    } else {
        via_ifr &= 0x7f;
    }
}
uint8_t VECX::_read8(uint64_t address)
{
    uint8_t data;
    if ((address & 0xe000) == 0xe000) {
        data = rom[address & 0x1fff];
    } else if ((address & 0xe000) == 0xc000) {
        if (address & 0x800) {
            data = ram[address & 0x3ff];
        } else if (address & 0x1000) {
            switch (address & 0xf) {
                case 0x0:
                    if (via_acr & 0x80) {
                        data = (uint8_t)((via_orb & 0x5f) | via_t1pb7 | alg_compare);
                    } else {
                        data = (uint8_t)((via_orb & 0xdf) | alg_compare);
                    }
                    break;
                case 0x1:
                    if ((via_pcr & 0x0e) == 0x08) {
                        via_ca2 = 0;
                    }
                case 0xf:
                    if ((via_orb & 0x18) == 0x08) {
                        data = (uint8_t)snd_regs[snd_select];
                    } else {
                        data = (uint8_t)via_ora;
                    }
                    break;
                case 0x2:
                    data = (uint8_t)via_ddrb;
                    break;
                case 0x3:
                    data = (uint8_t)via_ddra;
                    break;
                case 0x4:
                    data = (uint8_t)via_t1c;
                    via_ifr &= 0xbf;
                    via_t1on  = 0;
                    via_t1int = 0;
                    via_t1pb7 = 0x80;
                    int_update();
                    break;
                case 0x5:
                    data = (uint8_t)(via_t1c >> 8);
                    break;
                case 0x6:
                    data = (uint8_t)via_t1ll;
                    break;
                case 0x7:
                    data = (uint8_t)via_t1lh;
                    break;
                case 0x8:
                    data = (uint8_t)via_t2c;
                    via_ifr &= 0xdf;
                    via_t2on  = 0;
                    via_t2int = 0;
                    int_update();
                    break;
                case 0x9:
                    data = (uint8_t)(via_t2c >> 8);
                    break;
                case 0xa:
                    data = (uint8_t)via_sr;
                    via_ifr &= 0xfb;
                    via_srb   = 0;
                    via_srclk = 1;
                    int_update();
                    break;
                case 0xb:
                    data = (uint8_t)via_acr;
                    break;
                case 0xc:
                    data = (uint8_t)via_pcr;
                    break;
                case 0xd:
                    data = (uint8_t)via_ifr;
                    break;
                case 0xe:
                    data = (uint8_t)(via_ier | 0x80);
                    break;
            }
        }
    } else if (address < 0x8000) {
        data = cart[address];
    } else {
        data = 0xff;
    }
    return data;
}
void VECX::_write8(uint64_t address, uint8_t data)
{
    if ((address & 0xe000) == 0xe000) {
    } else if ((address & 0xe000) == 0xc000) {
        if (address & 0x800) {
            ram[address & 0x3ff] = data;
        }
        if (address & 0x1000) {
            switch (address & 0xf) {
                case 0x0:
                    via_orb = data;
                    snd_update();
                    alg_update();
                    if ((via_pcr & 0xe0) == 0x80) {
                        via_cb2h = 0;
                    }
                    break;
                case 0x1:
                    if ((via_pcr & 0x0e) == 0x08) {
                        via_ca2 = 0;
                    }
                case 0xf:
                    via_ora = data;
                    snd_update();
                    alg_xsh = data ^ 0x80;
                    alg_update();
                    break;
                case 0x2:
                    via_ddrb = data;
                    break;
                case 0x3:
                    via_ddra = data;
                    break;
                case 0x4:
                    via_t1ll = data;
                    break;
                case 0x5:
                    via_t1lh = data;
                    via_t1c  = (via_t1lh << 8) | via_t1ll;
                    via_ifr &= 0xbf;
                    via_t1on  = 1;
                    via_t1int = 1;
                    via_t1pb7 = 0;
                    int_update();
                    break;
                case 0x6:
                    via_t1ll = data;
                    break;
                case 0x7:
                    via_t1lh = data;
                    break;
                case 0x8:
                    via_t2ll = data;
                    break;
                case 0x9:
                    via_t2c = (data << 8) | via_t2ll;
                    via_ifr &= 0xdf;
                    via_t2on  = 1;
                    via_t2int = 1;
                    int_update();
                    break;
                case 0xa:
                    via_sr = data;
                    via_ifr &= 0xfb;
                    via_srb   = 0;
                    via_srclk = 1;
                    int_update();
                    break;
                case 0xb:
                    via_acr = data;
                    break;
                case 0xc:
                    via_pcr = data;
                    if ((via_pcr & 0x0e) == 0x0c) {
                        via_ca2 = 0;
                    } else {
                        via_ca2 = 1;
                    }
                    if ((via_pcr & 0xe0) == 0xc0) {
                        via_cb2h = 0;
                    } else {
                        via_cb2h = 1;
                    }
                    break;
                case 0xd:
                    via_ifr &= ~(data & 0x7f);
                    int_update();
                    break;
                case 0xe:
                    if (data & 0x80) {
                        via_ier |= data & 0x7f;
                    } else {
                        via_ier &= ~(data & 0x7f);
                    }
                    int_update();
                    break;
            }
        }
    } else if (address < 0x8000) {
    }
}
void VECX::vecx_reset(void)
{
    uint64_t r;
    for (r = 0; r < 1024; r++) {
        ram[r] = r & 0xff;
    }
    for (r = 0; r < 16; r++) {
        snd_regs[r] = 0;
        e8910_write(r, 0);
    }

    snd_regs[14] = 0xff;
    e8910_write(14, 0xff);

    snd_select      = 0;
    via_ora         = 0;
    via_orb         = 0;
    via_ddra        = 0;
    via_ddrb        = 0;
    via_t1on        = 0;
    via_t1int       = 0;
    via_t1c         = 0;
    via_t1ll        = 0;
    via_t1lh        = 0;
    via_t1pb7       = 0x80;
    via_t2on        = 0;
    via_t2int       = 0;
    via_t2c         = 0;
    via_t2ll        = 0;
    via_sr          = 0;
    via_srb         = 8;
    via_src         = 0;
    via_srclk       = 0;
    via_acr         = 0;
    via_pcr         = 0;
    via_ifr         = 0;
    via_ier         = 0;
    via_ca2         = 1;
    via_cb2h        = 1;
    via_cb2s        = 0;
    alg_rsh         = 128;
    alg_xsh         = 128;
    alg_ysh         = 128;
    alg_zsh         = 0;
    alg_jch0        = 128;
    alg_jch1        = 128;
    alg_jch2        = 128;
    alg_jch3        = 128;
    alg_jsh         = 128;
    alg_compare     = 0;
    alg_dx          = 0;
    alg_dy          = 0;
    alg_curr_x      = ALG_MAX_X / 2;
    alg_curr_y      = ALG_MAX_Y / 2;
    alg_vectoring   = 0;
    vector_draw_cnt = 0;
    vector_erse_cnt = 0;

    vectors_draw = vectors_set;
    vectors_erse = vectors_set + VECTOR_CNT;
    fcycles      = FCYCLES_INIT;

    cpu->e6809_reset();
}
void VECX::via_sstep0(void)
{
    uint64_t t2shift;
    if (via_t1on) {
        via_t1c--;
        if ((via_t1c & 0xffff) == 0xffff) {
            if (via_acr & 0x40) {
                via_ifr |= 0x40;
                int_update();
                via_t1pb7 = 0x80 - via_t1pb7;
                via_t1c   = (via_t1lh << 8) | via_t1ll;
            } else {
                if (via_t1int) {
                    via_ifr |= 0x40;
                    int_update();
                    via_t1pb7 = 0x80;
                    via_t1int = 0;
                }
            }
        }
    }
    if (via_t2on && (via_acr & 0x20) == 0x00) {
        via_t2c--;
        if ((via_t2c & 0xffff) == 0xffff) {
            if (via_t2int) {
                via_ifr |= 0x20;
                int_update();
                via_t2int = 0;
            }
        }
    }
    via_src--;
    if ((via_src & 0xff) == 0xff) {
        via_src = via_t2ll;
        if (via_srclk) {
            t2shift   = 1;
            via_srclk = 0;
        } else {
            t2shift   = 0;
            via_srclk = 1;
        }
    } else {
        t2shift = 0;
    }
    if (via_srb < 8) {
        switch (via_acr & 0x1c) {
            case 0x00:
                break;
            case 0x04:
                if (t2shift) {
                    via_sr <<= 1;
                    via_srb++;
                }
                break;
            case 0x08:
                via_sr <<= 1;
                via_srb++;
                break;
            case 0x0c:
                break;
            case 0x10:
                if (t2shift) {
                    via_cb2s = (via_sr >> 7) & 1;
                    via_sr <<= 1;
                    via_sr |= via_cb2s;
                }
                break;
            case 0x14:
                if (t2shift) {
                    via_cb2s = (via_sr >> 7) & 1;
                    via_sr <<= 1;
                    via_sr |= via_cb2s;
                    via_srb++;
                }
                break;
            case 0x18:
                via_cb2s = (via_sr >> 7) & 1;
                via_sr <<= 1;
                via_sr |= via_cb2s;
                via_srb++;
                break;
            case 0x1c:
                break;
        }
        if (via_srb == 8) {
            via_ifr |= 0x04;
            int_update();
        }
    }
}
void VECX::via_sstep1(void)
{
    if ((via_pcr & 0x0e) == 0x0a) {
        via_ca2 = 1;
    }
    if ((via_pcr & 0xe0) == 0xa0) {
        via_cb2h = 1;
    }
}
void VECX::alg_addline(long x0, long y0, long x1, long y1, uint8_t color)
{
    uint64_t key;
    long     index;
    key = (uint64_t)x0;
    key = key * 31 + (uint64_t)y0;
    key = key * 31 + (uint64_t)x1;
    key = key * 31 + (uint64_t)y1;
    key %= VECTOR_HASH;
    index = vector_hash[key];
    if (index >= 0 && index < vector_draw_cnt && x0 == vectors_draw[index].x0 && y0 == vectors_draw[index].y0 &&
        x1 == vectors_draw[index].x1 && y1 == vectors_draw[index].y1) {
        vectors_draw[index].color = color;
    } else {
        if (index >= 0 && index < vector_erse_cnt && x0 == vectors_erse[index].x0 && y0 == vectors_erse[index].y0 &&
            x1 == vectors_erse[index].x1 && y1 == vectors_erse[index].y1) {
            vectors_erse[index].color = VECTREX_COLORS;
        }
        vectors_draw[vector_draw_cnt].x0    = x0;
        vectors_draw[vector_draw_cnt].y0    = y0;
        vectors_draw[vector_draw_cnt].x1    = x1;
        vectors_draw[vector_draw_cnt].y1    = y1;
        vectors_draw[vector_draw_cnt].color = color;
        vector_hash[key]                    = vector_draw_cnt;
        vector_draw_cnt++;
    }
}
void VECX::alg_sstep(void)
{
    long     sig_dx, sig_dy;
    uint64_t sig_ramp;
    uint64_t sig_blank;
    if ((via_acr & 0x10) == 0x10) {
        sig_blank = via_cb2s;
    } else {
        sig_blank = via_cb2h;
    }
    if (via_ca2 == 0) {
        sig_dx = ALG_MAX_X / 2 - alg_curr_x;
        sig_dy = ALG_MAX_Y / 2 - alg_curr_y;
    } else {
        if (via_acr & 0x80) {
            sig_ramp = via_t1pb7;
        } else {
            sig_ramp = via_orb & 0x80;
        }
        if (sig_ramp == 0) {
            sig_dx = alg_dx;
            sig_dy = alg_dy;
        } else {
            sig_dx = 0;
            sig_dy = 0;
        }
    }
    if (alg_vectoring == 0) {
        if (sig_blank == 1 && alg_curr_x >= 0 && alg_curr_x < ALG_MAX_X && alg_curr_y >= 0 && alg_curr_y < ALG_MAX_Y) {
            alg_vectoring    = 1;
            alg_vector_x0    = alg_curr_x;
            alg_vector_y0    = alg_curr_y;
            alg_vector_x1    = alg_curr_x;
            alg_vector_y1    = alg_curr_y;
            alg_vector_dx    = sig_dx;
            alg_vector_dy    = sig_dy;
            alg_vector_color = (uint8_t)alg_zsh;
        }
    } else {
        if (sig_blank == 0) {
            alg_vectoring = 0;
            alg_addline(alg_vector_x0, alg_vector_y0, alg_vector_x1, alg_vector_y1, alg_vector_color);
        } else if (sig_dx != alg_vector_dx || sig_dy != alg_vector_dy || (uint8_t)alg_zsh != alg_vector_color) {
            alg_addline(alg_vector_x0, alg_vector_y0, alg_vector_x1, alg_vector_y1, alg_vector_color);
            if (alg_curr_x >= 0 && alg_curr_x < ALG_MAX_X && alg_curr_y >= 0 && alg_curr_y < ALG_MAX_Y) {
                alg_vector_x0    = alg_curr_x;
                alg_vector_y0    = alg_curr_y;
                alg_vector_x1    = alg_curr_x;
                alg_vector_y1    = alg_curr_y;
                alg_vector_dx    = sig_dx;
                alg_vector_dy    = sig_dy;
                alg_vector_color = (uint8_t)alg_zsh;
            } else {
                alg_vectoring = 0;
            }
        }
    }
    alg_curr_x += sig_dx;
    alg_curr_y += sig_dy;
    if (alg_vectoring == 1 && alg_curr_x >= 0 && alg_curr_x < ALG_MAX_X && alg_curr_y >= 0 && alg_curr_y < ALG_MAX_Y) {
        alg_vector_x1 = alg_curr_x;
        alg_vector_y1 = alg_curr_y;
    }
}
void VECX::vecx_emu(long cycles)
{
    uint64_t c, icycles;
    while (cycles > 0) {
        icycles = cpu->e6809_sstep(via_ifr & 0x80, 0);
        for (c = 0; c < icycles; c++) {
            via_sstep0();
            alg_sstep();
            via_sstep1();
        }
        cycles -= (long)icycles;
        fcycles -= (long)icycles;
        if (fcycles < 0) {
            vector_t *tmp;
            fcycles += FCYCLES_INIT;
            osint_render();
            vector_erse_cnt = vector_draw_cnt;
            vector_draw_cnt = 0;
            tmp             = vectors_erse;
            vectors_erse    = vectors_draw;
            vectors_draw    = tmp;
        }
    }
}
