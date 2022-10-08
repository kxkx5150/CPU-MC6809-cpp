#ifndef __E6809_H
#define __E6809_H
#include <cstdint>


class CPU {
  public:
    enum
    {
        FLAG_E     = 0x80,
        FLAG_F     = 0x40,
        FLAG_H     = 0x20,
        FLAG_I     = 0x10,
        FLAG_N     = 0x08,
        FLAG_Z     = 0x04,
        FLAG_V     = 0x02,
        FLAG_C     = 0x01,
        IRQ_NORMAL = 0,
        IRQ_SYNC   = 1,
        IRQ_CWAI   = 2
    };

    uint64_t  reg_x;
    uint64_t  reg_y;
    uint64_t  reg_u;
    uint64_t  reg_s;
    uint64_t  reg_pc;
    uint64_t  reg_a;
    uint64_t  reg_b;
    uint64_t  reg_dp;
    uint64_t  reg_cc;
    uint64_t  irq_status;
    uint64_t *rptr_xyus[4] = {&reg_x, &reg_y, &reg_u, &reg_s};

  public:
    uint64_t get_cc(uint64_t flag);
    void     set_cc(uint64_t flag, uint64_t value);

    uint64_t test_c(uint64_t i0, uint64_t i1, uint64_t r, uint64_t sub);
    uint64_t test_n(uint64_t r);
    uint64_t test_z8(uint64_t r);
    uint64_t test_z16(uint64_t r);
    uint64_t test_v(uint64_t i0, uint64_t i1, uint64_t r);

    uint64_t get_reg_d(void);
    void     set_reg_d(uint64_t value);

    uint64_t read8(uint64_t address);
    void     write8(uint64_t address, uint64_t data);
    uint64_t read16(uint64_t address);
    void     write16(uint64_t address, uint64_t data);
    void     push8(uint64_t *sp, uint64_t data);
    uint64_t pull8(uint64_t *sp);
    void     push16(uint64_t *sp, uint64_t data);
    uint64_t pull16(uint64_t *sp);

    uint64_t pc_read8(void);
    uint64_t pc_read16(void);

    uint64_t sign_extend(uint64_t data);
    uint64_t ea_direct(void);
    uint64_t ea_extended(void);
    uint64_t ea_indexed(uint64_t *cycles);

    uint64_t inst_neg(uint64_t data);
    uint64_t inst_com(uint64_t data);
    uint64_t inst_lsr(uint64_t data);
    uint64_t inst_ror(uint64_t data);
    uint64_t inst_asr(uint64_t data);
    uint64_t inst_asl(uint64_t data);
    uint64_t inst_rol(uint64_t data);
    uint64_t inst_dec(uint64_t data);
    uint64_t inst_inc(uint64_t data);
    void     inst_tst8(uint64_t data);
    void     inst_tst16(uint64_t data);
    void     inst_clr(void);
    uint64_t inst_sub8(uint64_t data0, uint64_t data1);
    uint64_t inst_sbc(uint64_t data0, uint64_t data1);
    uint64_t inst_and(uint64_t data0, uint64_t data1);
    uint64_t inst_eor(uint64_t data0, uint64_t data1);
    uint64_t inst_adc(uint64_t data0, uint64_t data1);
    uint64_t inst_or(uint64_t data0, uint64_t data1);
    uint64_t inst_add8(uint64_t data0, uint64_t data1);
    uint64_t inst_add16(uint64_t data0, uint64_t data1);
    uint64_t inst_sub16(uint64_t data0, uint64_t data1);
    void     inst_bra8(uint64_t test, uint64_t op, uint64_t *cycles);
    void     inst_bra16(uint64_t test, uint64_t op, uint64_t *cycles);
    void     inst_psh(uint64_t op, uint64_t *sp, uint64_t data, uint64_t *cycles);
    void     inst_pul(uint64_t op, uint64_t *sp, uint64_t *osp, uint64_t *cycles);

    uint64_t exgtfr_read(uint64_t reg);
    void     exgtfr_write(uint64_t reg, uint64_t data);

    void inst_exg(void);
    void inst_tfr(void);

    void     e6809_reset(void);
    uint64_t e6809_sstep(uint64_t irq_i, uint64_t irq_f);
};
#endif