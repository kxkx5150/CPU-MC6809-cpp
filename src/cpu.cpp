#include <stdio.h>
#include "cpu.h"
#include "vecx.h"



CPU::CPU(PC *_pc)
{
    pc = _pc;
}
uint64_t CPU::get_cc(uint64_t flag)
{
    return (reg_cc / flag) & 1;
}
void CPU::set_cc(uint64_t flag, uint64_t value)
{
    reg_cc &= ~flag;
    reg_cc |= value * flag;
}
uint64_t CPU::test_c(uint64_t i0, uint64_t i1, uint64_t r, uint64_t sub)
{
    uint64_t flag;
    flag = (i0 | i1) & ~r;
    flag |= (i0 & i1);
    flag = (flag >> 7) & 1;
    flag ^= sub;
    return flag;
}
uint64_t CPU::test_n(uint64_t r)
{
    return (r >> 7) & 1;
}
uint64_t CPU::test_z8(uint64_t r)
{
    uint64_t flag;
    flag = ~r;
    flag = (flag >> 4) & (flag & 0xf);
    flag = (flag >> 2) & (flag & 0x3);
    flag = (flag >> 1) & (flag & 0x1);
    return flag;
}
uint64_t CPU::test_z16(uint64_t r)
{
    uint64_t flag;
    flag = ~r;
    flag = (flag >> 8) & (flag & 0xff);
    flag = (flag >> 4) & (flag & 0xf);
    flag = (flag >> 2) & (flag & 0x3);
    flag = (flag >> 1) & (flag & 0x1);
    return flag;
}
uint64_t CPU::test_v(uint64_t i0, uint64_t i1, uint64_t r)
{
    uint64_t flag;
    flag = ~(i0 ^ i1);
    flag &= (i0 ^ r);
    flag = (flag >> 7) & 1;
    return flag;
}
uint64_t CPU::get_reg_d(void)
{
    return (reg_a << 8) | (reg_b & 0xff);
}
void CPU::set_reg_d(uint64_t value)
{
    reg_a = value >> 8;
    reg_b = value;
}
uint64_t CPU::read8(uint64_t address)
{
    return pc->vecx->_read8(address & 0xffff);
}
void CPU::write8(uint64_t address, uint64_t data)
{
    pc->vecx->_write8(address & 0xffff, (uint8_t)data);
}
uint64_t CPU::read16(uint64_t address)
{
    uint64_t datahi, datalo;
    datahi = read8(address);
    datalo = read8(address + 1);
    return (datahi << 8) | datalo;
}
void CPU::write16(uint64_t address, uint64_t data)
{
    write8(address, data >> 8);
    write8(address + 1, data);
}
void CPU::push8(uint64_t *sp, uint64_t data)
{
    (*sp)--;
    write8(*sp, data);
}
uint64_t CPU::pull8(uint64_t *sp)
{
    uint64_t data;
    data = read8(*sp);
    (*sp)++;
    return data;
}
void CPU::push16(uint64_t *sp, uint64_t data)
{
    push8(sp, data);
    push8(sp, data >> 8);
}
uint64_t CPU::pull16(uint64_t *sp)
{
    uint64_t datahi, datalo;
    datahi = pull8(sp);
    datalo = pull8(sp);
    return (datahi << 8) | datalo;
}
uint64_t CPU::pc_read8(void)
{
    uint64_t data;
    data = read8(reg_pc);
    reg_pc++;
    return data;
}
uint64_t CPU::pc_read16(void)
{
    uint64_t data;
    data = read16(reg_pc);
    reg_pc += 2;
    return data;
}
uint64_t CPU::sign_extend(uint64_t data)
{
    return (~(data & 0x80) + 1) | (data & 0xff);
}
uint64_t CPU::ea_direct(void)
{
    return (reg_dp << 8) | pc_read8();
}
uint64_t CPU::ea_extended(void)
{
    return pc_read16();
}
uint64_t CPU::ea_indexed(uint64_t *cycles)
{
    uint64_t r, op, ea;
    op = pc_read8();
    r  = (op >> 5) & 3;
    switch (op) {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08:
        case 0x09:
        case 0x0a:
        case 0x0b:
        case 0x0c:
        case 0x0d:
        case 0x0e:
        case 0x0f:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4a:
        case 0x4b:
        case 0x4c:
        case 0x4d:
        case 0x4e:
        case 0x4f:
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6a:
        case 0x6b:
        case 0x6c:
        case 0x6d:
        case 0x6e:
        case 0x6f:
            ea = *rptr_xyus[r] + (op & 0xf);
            (*cycles)++;
            break;
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x19:
        case 0x1a:
        case 0x1b:
        case 0x1c:
        case 0x1d:
        case 0x1e:
        case 0x1f:
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x37:
        case 0x38:
        case 0x39:
        case 0x3a:
        case 0x3b:
        case 0x3c:
        case 0x3d:
        case 0x3e:
        case 0x3f:
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
        case 0x58:
        case 0x59:
        case 0x5a:
        case 0x5b:
        case 0x5c:
        case 0x5d:
        case 0x5e:
        case 0x5f:
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
        case 0x78:
        case 0x79:
        case 0x7a:
        case 0x7b:
        case 0x7c:
        case 0x7d:
        case 0x7e:
        case 0x7f:
            ea = *rptr_xyus[r] + (op & 0xf) - 0x10;
            (*cycles)++;
            break;
        case 0x80:
        case 0x81:
        case 0xa0:
        case 0xa1:
        case 0xc0:
        case 0xc1:
        case 0xe0:
        case 0xe1:
            ea = *rptr_xyus[r];
            *rptr_xyus[r] += 1 + (op & 1);
            *cycles += 2 + (op & 1);
            break;
        case 0x90:
        case 0x91:
        case 0xb0:
        case 0xb1:
        case 0xd0:
        case 0xd1:
        case 0xf0:
        case 0xf1:
            ea = read16(*rptr_xyus[r]);
            *rptr_xyus[r] += 1 + (op & 1);
            *cycles += 5 + (op & 1);
            break;
        case 0x82:
        case 0x83:
        case 0xa2:
        case 0xa3:
        case 0xc2:
        case 0xc3:
        case 0xe2:
        case 0xe3:
            *rptr_xyus[r] -= 1 + (op & 1);
            ea = *rptr_xyus[r];
            *cycles += 2 + (op & 1);
            break;
        case 0x92:
        case 0x93:
        case 0xb2:
        case 0xb3:
        case 0xd2:
        case 0xd3:
        case 0xf2:
        case 0xf3:
            *rptr_xyus[r] -= 1 + (op & 1);
            ea = read16(*rptr_xyus[r]);
            *cycles += 5 + (op & 1);
            break;
        case 0x84:
        case 0xa4:
        case 0xc4:
        case 0xe4:
            ea = *rptr_xyus[r];
            break;
        case 0x94:
        case 0xb4:
        case 0xd4:
        case 0xf4:
            ea = read16(*rptr_xyus[r]);
            *cycles += 3;
            break;
        case 0x85:
        case 0xa5:
        case 0xc5:
        case 0xe5:
            ea = *rptr_xyus[r] + sign_extend(reg_b);
            *cycles += 1;
            break;
        case 0x95:
        case 0xb5:
        case 0xd5:
        case 0xf5:
            ea = read16(*rptr_xyus[r] + sign_extend(reg_b));
            *cycles += 4;
            break;
        case 0x86:
        case 0xa6:
        case 0xc6:
        case 0xe6:
            ea = *rptr_xyus[r] + sign_extend(reg_a);
            *cycles += 1;
            break;
        case 0x96:
        case 0xb6:
        case 0xd6:
        case 0xf6:
            ea = read16(*rptr_xyus[r] + sign_extend(reg_a));
            *cycles += 4;
            break;
        case 0x88:
        case 0xa8:
        case 0xc8:
        case 0xe8:
            ea = *rptr_xyus[r] + sign_extend(pc_read8());
            *cycles += 1;
            break;
        case 0x98:
        case 0xb8:
        case 0xd8:
        case 0xf8:
            ea = read16(*rptr_xyus[r] + sign_extend(pc_read8()));
            *cycles += 4;
            break;
        case 0x89:
        case 0xa9:
        case 0xc9:
        case 0xe9:
            ea = *rptr_xyus[r] + pc_read16();
            *cycles += 4;
            break;
        case 0x99:
        case 0xb9:
        case 0xd9:
        case 0xf9:
            ea = read16(*rptr_xyus[r] + pc_read16());
            *cycles += 7;
            break;
        case 0x8b:
        case 0xab:
        case 0xcb:
        case 0xeb:
            ea = *rptr_xyus[r] + get_reg_d();
            *cycles += 4;
            break;
        case 0x9b:
        case 0xbb:
        case 0xdb:
        case 0xfb:
            ea = read16(*rptr_xyus[r] + get_reg_d());
            *cycles += 7;
            break;
        case 0x8c:
        case 0xac:
        case 0xcc:
        case 0xec:
            r  = sign_extend(pc_read8());
            ea = reg_pc + r;
            *cycles += 1;
            break;
        case 0x9c:
        case 0xbc:
        case 0xdc:
        case 0xfc:
            r  = sign_extend(pc_read8());
            ea = read16(reg_pc + r);
            *cycles += 4;
            break;
        case 0x8d:
        case 0xad:
        case 0xcd:
        case 0xed:
            r  = pc_read16();
            ea = reg_pc + r;
            *cycles += 5;
            break;
        case 0x9d:
        case 0xbd:
        case 0xdd:
        case 0xfd:
            r  = pc_read16();
            ea = read16(reg_pc + r);
            *cycles += 8;
            break;
        case 0x9f:
            ea = read16(pc_read16());
            *cycles += 5;
            break;
        default:
            printf("undefined post-byte\n");
            break;
    }
    return ea;
}
uint64_t CPU::inst_neg(uint64_t data)
{
    uint64_t i0, i1, r;
    i0 = 0;
    i1 = ~data;
    r  = i0 + i1 + 1;
    set_cc(FLAG_H, test_c(i0 << 4, i1 << 4, r << 4, 0));
    set_cc(FLAG_N, test_n(r));
    set_cc(FLAG_Z, test_z8(r));
    set_cc(FLAG_V, test_v(i0, i1, r));
    set_cc(FLAG_C, test_c(i0, i1, r, 1));
    return r;
}
uint64_t CPU::inst_com(uint64_t data)
{
    uint64_t r;
    r = ~data;
    set_cc(FLAG_N, test_n(r));
    set_cc(FLAG_Z, test_z8(r));
    set_cc(FLAG_V, 0);
    set_cc(FLAG_C, 1);
    return r;
}
uint64_t CPU::inst_lsr(uint64_t data)
{
    uint64_t r;
    r = (data >> 1) & 0x7f;
    set_cc(FLAG_N, 0);
    set_cc(FLAG_Z, test_z8(r));
    set_cc(FLAG_C, data & 1);
    return r;
}
uint64_t CPU::inst_ror(uint64_t data)
{
    uint64_t r, c;
    c = get_cc(FLAG_C);
    r = ((data >> 1) & 0x7f) | (c << 7);
    set_cc(FLAG_N, test_n(r));
    set_cc(FLAG_Z, test_z8(r));
    set_cc(FLAG_C, data & 1);
    return r;
}
uint64_t CPU::inst_asr(uint64_t data)
{
    uint64_t r;
    r = ((data >> 1) & 0x7f) | (data & 0x80);
    set_cc(FLAG_N, test_n(r));
    set_cc(FLAG_Z, test_z8(r));
    set_cc(FLAG_C, data & 1);
    return r;
}
uint64_t CPU::inst_asl(uint64_t data)
{
    uint64_t i0, i1, r;
    i0 = data;
    i1 = data;
    r  = i0 + i1;
    set_cc(FLAG_H, test_c(i0 << 4, i1 << 4, r << 4, 0));
    set_cc(FLAG_N, test_n(r));
    set_cc(FLAG_Z, test_z8(r));
    set_cc(FLAG_V, test_v(i0, i1, r));
    set_cc(FLAG_C, test_c(i0, i1, r, 0));
    return r;
}
uint64_t CPU::inst_rol(uint64_t data)
{
    uint64_t i0, i1, c, r;
    i0 = data;
    i1 = data;
    c  = get_cc(FLAG_C);
    r  = i0 + i1 + c;
    set_cc(FLAG_N, test_n(r));
    set_cc(FLAG_Z, test_z8(r));
    set_cc(FLAG_V, test_v(i0, i1, r));
    set_cc(FLAG_C, test_c(i0, i1, r, 0));
    return r;
}
uint64_t CPU::inst_dec(uint64_t data)
{
    uint64_t i0, i1, r;
    i0 = data;
    i1 = 0xff;
    r  = i0 + i1;
    set_cc(FLAG_N, test_n(r));
    set_cc(FLAG_Z, test_z8(r));
    set_cc(FLAG_V, test_v(i0, i1, r));
    return r;
}
uint64_t CPU::inst_inc(uint64_t data)
{
    uint64_t i0, i1, r;
    i0 = data;
    i1 = 1;
    r  = i0 + i1;
    set_cc(FLAG_N, test_n(r));
    set_cc(FLAG_Z, test_z8(r));
    set_cc(FLAG_V, test_v(i0, i1, r));
    return r;
}
void CPU::inst_tst8(uint64_t data)
{
    set_cc(FLAG_N, test_n(data));
    set_cc(FLAG_Z, test_z8(data));
    set_cc(FLAG_V, 0);
}
void CPU::inst_tst16(uint64_t data)
{
    set_cc(FLAG_N, test_n(data >> 8));
    set_cc(FLAG_Z, test_z16(data));
    set_cc(FLAG_V, 0);
}
void CPU::inst_clr(void)
{
    set_cc(FLAG_N, 0);
    set_cc(FLAG_Z, 1);
    set_cc(FLAG_V, 0);
    set_cc(FLAG_C, 0);
}
uint64_t CPU::inst_sub8(uint64_t data0, uint64_t data1)
{
    uint64_t i0, i1, r;
    i0 = data0;
    i1 = ~data1;
    r  = i0 + i1 + 1;
    set_cc(FLAG_H, test_c(i0 << 4, i1 << 4, r << 4, 0));
    set_cc(FLAG_N, test_n(r));
    set_cc(FLAG_Z, test_z8(r));
    set_cc(FLAG_V, test_v(i0, i1, r));
    set_cc(FLAG_C, test_c(i0, i1, r, 1));
    return r;
}
uint64_t CPU::inst_sbc(uint64_t data0, uint64_t data1)
{
    uint64_t i0, i1, c, r;
    i0 = data0;
    i1 = ~data1;
    c  = 1 - get_cc(FLAG_C);
    r  = i0 + i1 + c;
    set_cc(FLAG_H, test_c(i0 << 4, i1 << 4, r << 4, 0));
    set_cc(FLAG_N, test_n(r));
    set_cc(FLAG_Z, test_z8(r));
    set_cc(FLAG_V, test_v(i0, i1, r));
    set_cc(FLAG_C, test_c(i0, i1, r, 1));
    return r;
}
uint64_t CPU::inst_and(uint64_t data0, uint64_t data1)
{
    uint64_t r;
    r = data0 & data1;
    inst_tst8(r);
    return r;
}
uint64_t CPU::inst_eor(uint64_t data0, uint64_t data1)
{
    uint64_t r;
    r = data0 ^ data1;
    inst_tst8(r);
    return r;
}
uint64_t CPU::inst_adc(uint64_t data0, uint64_t data1)
{
    uint64_t i0, i1, c, r;
    i0 = data0;
    i1 = data1;
    c  = get_cc(FLAG_C);
    r  = i0 + i1 + c;
    set_cc(FLAG_H, test_c(i0 << 4, i1 << 4, r << 4, 0));
    set_cc(FLAG_N, test_n(r));
    set_cc(FLAG_Z, test_z8(r));
    set_cc(FLAG_V, test_v(i0, i1, r));
    set_cc(FLAG_C, test_c(i0, i1, r, 0));
    return r;
}
uint64_t CPU::inst_or(uint64_t data0, uint64_t data1)
{
    uint64_t r;
    r = data0 | data1;
    inst_tst8(r);
    return r;
}
uint64_t CPU::inst_add8(uint64_t data0, uint64_t data1)
{
    uint64_t i0, i1, r;
    i0 = data0;
    i1 = data1;
    r  = i0 + i1;
    set_cc(FLAG_H, test_c(i0 << 4, i1 << 4, r << 4, 0));
    set_cc(FLAG_N, test_n(r));
    set_cc(FLAG_Z, test_z8(r));
    set_cc(FLAG_V, test_v(i0, i1, r));
    set_cc(FLAG_C, test_c(i0, i1, r, 0));
    return r;
}
uint64_t CPU::inst_add16(uint64_t data0, uint64_t data1)
{
    uint64_t i0, i1, r;
    i0 = data0;
    i1 = data1;
    r  = i0 + i1;
    set_cc(FLAG_N, test_n(r >> 8));
    set_cc(FLAG_Z, test_z16(r));
    set_cc(FLAG_V, test_v(i0 >> 8, i1 >> 8, r >> 8));
    set_cc(FLAG_C, test_c(i0 >> 8, i1 >> 8, r >> 8, 0));
    return r;
}
uint64_t CPU::inst_sub16(uint64_t data0, uint64_t data1)
{
    uint64_t i0, i1, r;
    i0 = data0;
    i1 = ~data1;
    r  = i0 + i1 + 1;
    set_cc(FLAG_N, test_n(r >> 8));
    set_cc(FLAG_Z, test_z16(r));
    set_cc(FLAG_V, test_v(i0 >> 8, i1 >> 8, r >> 8));
    set_cc(FLAG_C, test_c(i0 >> 8, i1 >> 8, r >> 8, 1));
    return r;
}
void CPU::inst_bra8(uint64_t test, uint64_t op, uint64_t *cycles)
{
    uint64_t offset, mask;
    offset = pc_read8();
    mask   = (test ^ (op & 1)) - 1;
    reg_pc += sign_extend(offset) & mask;
    *cycles += 3;
}
void CPU::inst_bra16(uint64_t test, uint64_t op, uint64_t *cycles)
{
    uint64_t offset, mask;
    offset = pc_read16();
    mask   = (test ^ (op & 1)) - 1;
    reg_pc += offset & mask;
    *cycles += 5 - mask;
}
void CPU::inst_psh(uint64_t op, uint64_t *sp, uint64_t data, uint64_t *cycles)
{
    if (op & 0x80) {
        push16(sp, reg_pc);
        *cycles += 2;
    }
    if (op & 0x40) {
        push16(sp, data);
        *cycles += 2;
    }
    if (op & 0x20) {
        push16(sp, reg_y);
        *cycles += 2;
    }
    if (op & 0x10) {
        push16(sp, reg_x);
        *cycles += 2;
    }
    if (op & 0x08) {
        push8(sp, reg_dp);
        *cycles += 1;
    }
    if (op & 0x04) {
        push8(sp, reg_b);
        *cycles += 1;
    }
    if (op & 0x02) {
        push8(sp, reg_a);
        *cycles += 1;
    }
    if (op & 0x01) {
        push8(sp, reg_cc);
        *cycles += 1;
    }
}
void CPU::inst_pul(uint64_t op, uint64_t *sp, uint64_t *osp, uint64_t *cycles)
{
    if (op & 0x01) {
        reg_cc = pull8(sp);
        *cycles += 1;
    }
    if (op & 0x02) {
        reg_a = pull8(sp);
        *cycles += 1;
    }
    if (op & 0x04) {
        reg_b = pull8(sp);
        *cycles += 1;
    }
    if (op & 0x08) {
        reg_dp = pull8(sp);
        *cycles += 1;
    }
    if (op & 0x10) {
        reg_x = pull16(sp);
        *cycles += 2;
    }
    if (op & 0x20) {
        reg_y = pull16(sp);
        *cycles += 2;
    }
    if (op & 0x40) {
        *osp = pull16(sp);
        *cycles += 2;
    }
    if (op & 0x80) {
        reg_pc = pull16(sp);
        *cycles += 2;
    }
}
uint64_t CPU::exgtfr_read(uint64_t reg)
{
    uint64_t data;
    switch (reg) {
        case 0x0:
            data = get_reg_d();
            break;
        case 0x1:
            data = reg_x;
            break;
        case 0x2:
            data = reg_y;
            break;
        case 0x3:
            data = reg_u;
            break;
        case 0x4:
            data = reg_s;
            break;
        case 0x5:
            data = reg_pc;
            break;
        case 0x8:
            data = 0xff00 | reg_a;
            break;
        case 0x9:
            data = 0xff00 | reg_b;
            break;
        case 0xa:
            data = 0xff00 | reg_cc;
            break;
        case 0xb:
            data = 0xff00 | reg_dp;
            break;
        default:
            data = 0xffff;
            printf("illegal exgtfr reg %.1lx\n", reg);
            break;
    }
    return data;
}
void CPU::exgtfr_write(uint64_t reg, uint64_t data)
{
    switch (reg) {
        case 0x0:
            set_reg_d(data);
            break;
        case 0x1:
            reg_x = data;
            break;
        case 0x2:
            reg_y = data;
            break;
        case 0x3:
            reg_u = data;
            break;
        case 0x4:
            reg_s = data;
            break;
        case 0x5:
            reg_pc = data;
            break;
        case 0x8:
            reg_a = data;
            break;
        case 0x9:
            reg_b = data;
            break;
        case 0xa:
            reg_cc = data;
            break;
        case 0xb:
            reg_dp = data;
            break;
        default:
            printf("illegal exgtfr reg %.1lx\n", reg);
            break;
    }
}
void CPU::inst_exg(void)
{
    uint64_t op, tmp;
    op  = pc_read8();
    tmp = exgtfr_read(op & 0xf);
    exgtfr_write(op & 0xf, exgtfr_read(op >> 4));
    exgtfr_write(op >> 4, tmp);
}
void CPU::inst_tfr(void)
{
    uint64_t op;
    op = pc_read8();
    exgtfr_write(op & 0xf, exgtfr_read(op >> 4));
}
void CPU::e6809_reset(void)
{
    reg_x      = 0;
    reg_y      = 0;
    reg_u      = 0;
    reg_s      = 0;
    reg_a      = 0;
    reg_b      = 0;
    reg_dp     = 0;
    reg_cc     = FLAG_I | FLAG_F;
    irq_status = IRQ_NORMAL;
    reg_pc     = read16(0xfffe);
}
uint64_t CPU::e6809_sstep(uint64_t irq_i, uint64_t irq_f)
{
    uint64_t op;
    uint64_t cycles = 0;
    uint64_t ea, i0, i1, r;

    if (irq_f) {
        if (get_cc(FLAG_F) == 0) {
            if (irq_status != IRQ_CWAI) {
                set_cc(FLAG_E, 0);
                inst_psh(0x81, &reg_s, reg_u, &cycles);
            }
            set_cc(FLAG_I, 1);
            set_cc(FLAG_F, 1);
            reg_pc     = read16(0xfff6);
            irq_status = IRQ_NORMAL;
            cycles += 7;
        } else {
            if (irq_status == IRQ_SYNC) {
                irq_status = IRQ_NORMAL;
            }
        }
    }

    if (irq_i) {
        if (get_cc(FLAG_I) == 0) {
            if (irq_status != IRQ_CWAI) {
                set_cc(FLAG_E, 1);
                inst_psh(0xff, &reg_s, reg_u, &cycles);
            }
            set_cc(FLAG_I, 1);
            reg_pc     = read16(0xfff8);
            irq_status = IRQ_NORMAL;
            cycles += 7;
        } else {
            if (irq_status == IRQ_SYNC) {
                irq_status = IRQ_NORMAL;
            }
        }
    }

    if (irq_status != IRQ_NORMAL) {
        return cycles + 1;
    }

    op = pc_read8();
    switch (op) {
        case 0x00:
            ea = ea_direct();
            r  = inst_neg(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x40:
            reg_a = inst_neg(reg_a);
            cycles += 2;
            break;
        case 0x50:
            reg_b = inst_neg(reg_b);
            cycles += 2;
            break;
        case 0x60:
            ea = ea_indexed(&cycles);
            r  = inst_neg(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x70:
            ea = ea_extended();
            r  = inst_neg(read8(ea));
            write8(ea, r);
            cycles += 7;
            break;
        case 0x03:
            ea = ea_direct();
            r  = inst_com(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x43:
            reg_a = inst_com(reg_a);
            cycles += 2;
            break;
        case 0x53:
            reg_b = inst_com(reg_b);
            cycles += 2;
            break;
        case 0x63:
            ea = ea_indexed(&cycles);
            r  = inst_com(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x73:
            ea = ea_extended();
            r  = inst_com(read8(ea));
            write8(ea, r);
            cycles += 7;
            break;
        case 0x04:
            ea = ea_direct();
            r  = inst_lsr(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x44:
            reg_a = inst_lsr(reg_a);
            cycles += 2;
            break;
        case 0x54:
            reg_b = inst_lsr(reg_b);
            cycles += 2;
            break;
        case 0x64:
            ea = ea_indexed(&cycles);
            r  = inst_lsr(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x74:
            ea = ea_extended();
            r  = inst_lsr(read8(ea));
            write8(ea, r);
            cycles += 7;
            break;
        case 0x06:
            ea = ea_direct();
            r  = inst_ror(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x46:
            reg_a = inst_ror(reg_a);
            cycles += 2;
            break;
        case 0x56:
            reg_b = inst_ror(reg_b);
            cycles += 2;
            break;
        case 0x66:
            ea = ea_indexed(&cycles);
            r  = inst_ror(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x76:
            ea = ea_extended();
            r  = inst_ror(read8(ea));
            write8(ea, r);
            cycles += 7;
            break;
        case 0x07:
            ea = ea_direct();
            r  = inst_asr(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x47:
            reg_a = inst_asr(reg_a);
            cycles += 2;
            break;
        case 0x57:
            reg_b = inst_asr(reg_b);
            cycles += 2;
            break;
        case 0x67:
            ea = ea_indexed(&cycles);
            r  = inst_asr(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x77:
            ea = ea_extended();
            r  = inst_asr(read8(ea));
            write8(ea, r);
            cycles += 7;
            break;
        case 0x08:
            ea = ea_direct();
            r  = inst_asl(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x48:
            reg_a = inst_asl(reg_a);
            cycles += 2;
            break;
        case 0x58:
            reg_b = inst_asl(reg_b);
            cycles += 2;
            break;
        case 0x68:
            ea = ea_indexed(&cycles);
            r  = inst_asl(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x78:
            ea = ea_extended();
            r  = inst_asl(read8(ea));
            write8(ea, r);
            cycles += 7;
            break;
        case 0x09:
            ea = ea_direct();
            r  = inst_rol(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x49:
            reg_a = inst_rol(reg_a);
            cycles += 2;
            break;
        case 0x59:
            reg_b = inst_rol(reg_b);
            cycles += 2;
            break;
        case 0x69:
            ea = ea_indexed(&cycles);
            r  = inst_rol(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x79:
            ea = ea_extended();
            r  = inst_rol(read8(ea));
            write8(ea, r);
            cycles += 7;
            break;
        case 0x0a:
            ea = ea_direct();
            r  = inst_dec(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x4a:
            reg_a = inst_dec(reg_a);
            cycles += 2;
            break;
        case 0x5a:
            reg_b = inst_dec(reg_b);
            cycles += 2;
            break;
        case 0x6a:
            ea = ea_indexed(&cycles);
            r  = inst_dec(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x7a:
            ea = ea_extended();
            r  = inst_dec(read8(ea));
            write8(ea, r);
            cycles += 7;
            break;
        case 0x0c:
            ea = ea_direct();
            r  = inst_inc(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x4c:
            reg_a = inst_inc(reg_a);
            cycles += 2;
            break;
        case 0x5c:
            reg_b = inst_inc(reg_b);
            cycles += 2;
            break;
        case 0x6c:
            ea = ea_indexed(&cycles);
            r  = inst_inc(read8(ea));
            write8(ea, r);
            cycles += 6;
            break;
        case 0x7c:
            ea = ea_extended();
            r  = inst_inc(read8(ea));
            write8(ea, r);
            cycles += 7;
            break;
        case 0x0d:
            ea = ea_direct();
            inst_tst8(read8(ea));
            cycles += 6;
            break;
        case 0x4d:
            inst_tst8(reg_a);
            cycles += 2;
            break;
        case 0x5d:
            inst_tst8(reg_b);
            cycles += 2;
            break;
        case 0x6d:
            ea = ea_indexed(&cycles);
            inst_tst8(read8(ea));
            cycles += 6;
            break;
        case 0x7d:
            ea = ea_extended();
            inst_tst8(read8(ea));
            cycles += 7;
            break;
        case 0x0e:
            reg_pc = ea_direct();
            cycles += 3;
            break;
        case 0x6e:
            reg_pc = ea_indexed(&cycles);
            cycles += 3;
            break;
        case 0x7e:
            reg_pc = ea_extended();
            cycles += 4;
            break;
        case 0x0f:
            ea = ea_direct();
            inst_clr();
            write8(ea, 0);
            cycles += 6;
            break;
        case 0x4f:
            inst_clr();
            reg_a = 0;
            cycles += 2;
            break;
        case 0x5f:
            inst_clr();
            reg_b = 0;
            cycles += 2;
            break;
        case 0x6f:
            ea = ea_indexed(&cycles);
            inst_clr();
            write8(ea, 0);
            cycles += 6;
            break;
        case 0x7f:
            ea = ea_extended();
            inst_clr();
            write8(ea, 0);
            cycles += 7;
            break;
        case 0x80:
            reg_a = inst_sub8(reg_a, pc_read8());
            cycles += 2;
            break;
        case 0x90:
            ea    = ea_direct();
            reg_a = inst_sub8(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xa0:
            ea    = ea_indexed(&cycles);
            reg_a = inst_sub8(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xb0:
            ea    = ea_extended();
            reg_a = inst_sub8(reg_a, read8(ea));
            cycles += 5;
            break;
        case 0xc0:
            reg_b = inst_sub8(reg_b, pc_read8());
            cycles += 2;
            break;
        case 0xd0:
            ea    = ea_direct();
            reg_b = inst_sub8(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xe0:
            ea    = ea_indexed(&cycles);
            reg_b = inst_sub8(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xf0:
            ea    = ea_extended();
            reg_b = inst_sub8(reg_b, read8(ea));
            cycles += 5;
            break;
        case 0x81:
            inst_sub8(reg_a, pc_read8());
            cycles += 2;
            break;
        case 0x91:
            ea = ea_direct();
            inst_sub8(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xa1:
            ea = ea_indexed(&cycles);
            inst_sub8(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xb1:
            ea = ea_extended();
            inst_sub8(reg_a, read8(ea));
            cycles += 5;
            break;
        case 0xc1:
            inst_sub8(reg_b, pc_read8());
            cycles += 2;
            break;
        case 0xd1:
            ea = ea_direct();
            inst_sub8(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xe1:
            ea = ea_indexed(&cycles);
            inst_sub8(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xf1:
            ea = ea_extended();
            inst_sub8(reg_b, read8(ea));
            cycles += 5;
            break;
        case 0x82:
            reg_a = inst_sbc(reg_a, pc_read8());
            cycles += 2;
            break;
        case 0x92:
            ea    = ea_direct();
            reg_a = inst_sbc(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xa2:
            ea    = ea_indexed(&cycles);
            reg_a = inst_sbc(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xb2:
            ea    = ea_extended();
            reg_a = inst_sbc(reg_a, read8(ea));
            cycles += 5;
            break;
        case 0xc2:
            reg_b = inst_sbc(reg_b, pc_read8());
            cycles += 2;
            break;
        case 0xd2:
            ea    = ea_direct();
            reg_b = inst_sbc(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xe2:
            ea    = ea_indexed(&cycles);
            reg_b = inst_sbc(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xf2:
            ea    = ea_extended();
            reg_b = inst_sbc(reg_b, read8(ea));
            cycles += 5;
            break;
        case 0x84:
            reg_a = inst_and(reg_a, pc_read8());
            cycles += 2;
            break;
        case 0x94:
            ea    = ea_direct();
            reg_a = inst_and(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xa4:
            ea    = ea_indexed(&cycles);
            reg_a = inst_and(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xb4:
            ea    = ea_extended();
            reg_a = inst_and(reg_a, read8(ea));
            cycles += 5;
            break;
        case 0xc4:
            reg_b = inst_and(reg_b, pc_read8());
            cycles += 2;
            break;
        case 0xd4:
            ea    = ea_direct();
            reg_b = inst_and(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xe4:
            ea    = ea_indexed(&cycles);
            reg_b = inst_and(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xf4:
            ea    = ea_extended();
            reg_b = inst_and(reg_b, read8(ea));
            cycles += 5;
            break;
        case 0x85:
            inst_and(reg_a, pc_read8());
            cycles += 2;
            break;
        case 0x95:
            ea = ea_direct();
            inst_and(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xa5:
            ea = ea_indexed(&cycles);
            inst_and(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xb5:
            ea = ea_extended();
            inst_and(reg_a, read8(ea));
            cycles += 5;
            break;
        case 0xc5:
            inst_and(reg_b, pc_read8());
            cycles += 2;
            break;
        case 0xd5:
            ea = ea_direct();
            inst_and(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xe5:
            ea = ea_indexed(&cycles);
            inst_and(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xf5:
            ea = ea_extended();
            inst_and(reg_b, read8(ea));
            cycles += 5;
            break;
        case 0x86:
            reg_a = pc_read8();
            inst_tst8(reg_a);
            cycles += 2;
            break;
        case 0x96:
            ea    = ea_direct();
            reg_a = read8(ea);
            inst_tst8(reg_a);
            cycles += 4;
            break;
        case 0xa6:
            ea    = ea_indexed(&cycles);
            reg_a = read8(ea);
            inst_tst8(reg_a);
            cycles += 4;
            break;
        case 0xb6:
            ea    = ea_extended();
            reg_a = read8(ea);
            inst_tst8(reg_a);
            cycles += 5;
            break;
        case 0xc6:
            reg_b = pc_read8();
            inst_tst8(reg_b);
            cycles += 2;
            break;
        case 0xd6:
            ea    = ea_direct();
            reg_b = read8(ea);
            inst_tst8(reg_b);
            cycles += 4;
            break;
        case 0xe6:
            ea    = ea_indexed(&cycles);
            reg_b = read8(ea);
            inst_tst8(reg_b);
            cycles += 4;
            break;
        case 0xf6:
            ea    = ea_extended();
            reg_b = read8(ea);
            inst_tst8(reg_b);
            cycles += 5;
            break;
        case 0x97:
            ea = ea_direct();
            write8(ea, reg_a);
            inst_tst8(reg_a);
            cycles += 4;
            break;
        case 0xa7:
            ea = ea_indexed(&cycles);
            write8(ea, reg_a);
            inst_tst8(reg_a);
            cycles += 4;
            break;
        case 0xb7:
            ea = ea_extended();
            write8(ea, reg_a);
            inst_tst8(reg_a);
            cycles += 5;
            break;
        case 0xd7:
            ea = ea_direct();
            write8(ea, reg_b);
            inst_tst8(reg_b);
            cycles += 4;
            break;
        case 0xe7:
            ea = ea_indexed(&cycles);
            write8(ea, reg_b);
            inst_tst8(reg_b);
            cycles += 4;
            break;
        case 0xf7:
            ea = ea_extended();
            write8(ea, reg_b);
            inst_tst8(reg_b);
            cycles += 5;
            break;
        case 0x88:
            reg_a = inst_eor(reg_a, pc_read8());
            cycles += 2;
            break;
        case 0x98:
            ea    = ea_direct();
            reg_a = inst_eor(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xa8:
            ea    = ea_indexed(&cycles);
            reg_a = inst_eor(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xb8:
            ea    = ea_extended();
            reg_a = inst_eor(reg_a, read8(ea));
            cycles += 5;
            break;
        case 0xc8:
            reg_b = inst_eor(reg_b, pc_read8());
            cycles += 2;
            break;
        case 0xd8:
            ea    = ea_direct();
            reg_b = inst_eor(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xe8:
            ea    = ea_indexed(&cycles);
            reg_b = inst_eor(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xf8:
            ea    = ea_extended();
            reg_b = inst_eor(reg_b, read8(ea));
            cycles += 5;
            break;
        case 0x89:
            reg_a = inst_adc(reg_a, pc_read8());
            cycles += 2;
            break;
        case 0x99:
            ea    = ea_direct();
            reg_a = inst_adc(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xa9:
            ea    = ea_indexed(&cycles);
            reg_a = inst_adc(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xb9:
            ea    = ea_extended();
            reg_a = inst_adc(reg_a, read8(ea));
            cycles += 5;
            break;
        case 0xc9:
            reg_b = inst_adc(reg_b, pc_read8());
            cycles += 2;
            break;
        case 0xd9:
            ea    = ea_direct();
            reg_b = inst_adc(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xe9:
            ea    = ea_indexed(&cycles);
            reg_b = inst_adc(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xf9:
            ea    = ea_extended();
            reg_b = inst_adc(reg_b, read8(ea));
            cycles += 5;
            break;
        case 0x8a:
            reg_a = inst_or(reg_a, pc_read8());
            cycles += 2;
            break;
        case 0x9a:
            ea    = ea_direct();
            reg_a = inst_or(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xaa:
            ea    = ea_indexed(&cycles);
            reg_a = inst_or(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xba:
            ea    = ea_extended();
            reg_a = inst_or(reg_a, read8(ea));
            cycles += 5;
            break;
        case 0xca:
            reg_b = inst_or(reg_b, pc_read8());
            cycles += 2;
            break;
        case 0xda:
            ea    = ea_direct();
            reg_b = inst_or(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xea:
            ea    = ea_indexed(&cycles);
            reg_b = inst_or(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xfa:
            ea    = ea_extended();
            reg_b = inst_or(reg_b, read8(ea));
            cycles += 5;
            break;
        case 0x8b:
            reg_a = inst_add8(reg_a, pc_read8());
            cycles += 2;
            break;
        case 0x9b:
            ea    = ea_direct();
            reg_a = inst_add8(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xab:
            ea    = ea_indexed(&cycles);
            reg_a = inst_add8(reg_a, read8(ea));
            cycles += 4;
            break;
        case 0xbb:
            ea    = ea_extended();
            reg_a = inst_add8(reg_a, read8(ea));
            cycles += 5;
            break;
        case 0xcb:
            reg_b = inst_add8(reg_b, pc_read8());
            cycles += 2;
            break;
        case 0xdb:
            ea    = ea_direct();
            reg_b = inst_add8(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xeb:
            ea    = ea_indexed(&cycles);
            reg_b = inst_add8(reg_b, read8(ea));
            cycles += 4;
            break;
        case 0xfb:
            ea    = ea_extended();
            reg_b = inst_add8(reg_b, read8(ea));
            cycles += 5;
            break;
        case 0x83:
            set_reg_d(inst_sub16(get_reg_d(), pc_read16()));
            cycles += 4;
            break;
        case 0x93:
            ea = ea_direct();
            set_reg_d(inst_sub16(get_reg_d(), read16(ea)));
            cycles += 6;
            break;
        case 0xa3:
            ea = ea_indexed(&cycles);
            set_reg_d(inst_sub16(get_reg_d(), read16(ea)));
            cycles += 6;
            break;
        case 0xb3:
            ea = ea_extended();
            set_reg_d(inst_sub16(get_reg_d(), read16(ea)));
            cycles += 7;
            break;
        case 0x8c:
            inst_sub16(reg_x, pc_read16());
            cycles += 4;
            break;
        case 0x9c:
            ea = ea_direct();
            inst_sub16(reg_x, read16(ea));
            cycles += 6;
            break;
        case 0xac:
            ea = ea_indexed(&cycles);
            inst_sub16(reg_x, read16(ea));
            cycles += 6;
            break;
        case 0xbc:
            ea = ea_extended();
            inst_sub16(reg_x, read16(ea));
            cycles += 7;
            break;
        case 0x8e:
            reg_x = pc_read16();
            inst_tst16(reg_x);
            cycles += 3;
            break;
        case 0x9e:
            ea    = ea_direct();
            reg_x = read16(ea);
            inst_tst16(reg_x);
            cycles += 5;
            break;
        case 0xae:
            ea    = ea_indexed(&cycles);
            reg_x = read16(ea);
            inst_tst16(reg_x);
            cycles += 5;
            break;
        case 0xbe:
            ea    = ea_extended();
            reg_x = read16(ea);
            inst_tst16(reg_x);
            cycles += 6;
            break;
        case 0xce:
            reg_u = pc_read16();
            inst_tst16(reg_u);
            cycles += 3;
            break;
        case 0xde:
            ea    = ea_direct();
            reg_u = read16(ea);
            inst_tst16(reg_u);
            cycles += 5;
            break;
        case 0xee:
            ea    = ea_indexed(&cycles);
            reg_u = read16(ea);
            inst_tst16(reg_u);
            cycles += 5;
            break;
        case 0xfe:
            ea    = ea_extended();
            reg_u = read16(ea);
            inst_tst16(reg_u);
            cycles += 6;
            break;
        case 0x9f:
            ea = ea_direct();
            write16(ea, reg_x);
            inst_tst16(reg_x);
            cycles += 5;
            break;
        case 0xaf:
            ea = ea_indexed(&cycles);
            write16(ea, reg_x);
            inst_tst16(reg_x);
            cycles += 5;
            break;
        case 0xbf:
            ea = ea_extended();
            write16(ea, reg_x);
            inst_tst16(reg_x);
            cycles += 6;
            break;
        case 0xdf:
            ea = ea_direct();
            write16(ea, reg_u);
            inst_tst16(reg_u);
            cycles += 5;
            break;
        case 0xef:
            ea = ea_indexed(&cycles);
            write16(ea, reg_u);
            inst_tst16(reg_u);
            cycles += 5;
            break;
        case 0xff:
            ea = ea_extended();
            write16(ea, reg_u);
            inst_tst16(reg_u);
            cycles += 6;
            break;
        case 0xc3:
            set_reg_d(inst_add16(get_reg_d(), pc_read16()));
            cycles += 4;
            break;
        case 0xd3:
            ea = ea_direct();
            set_reg_d(inst_add16(get_reg_d(), read16(ea)));
            cycles += 6;
            break;
        case 0xe3:
            ea = ea_indexed(&cycles);
            set_reg_d(inst_add16(get_reg_d(), read16(ea)));
            cycles += 6;
            break;
        case 0xf3:
            ea = ea_extended();
            set_reg_d(inst_add16(get_reg_d(), read16(ea)));
            cycles += 7;
            break;
        case 0xcc:
            set_reg_d(pc_read16());
            inst_tst16(get_reg_d());
            cycles += 3;
            break;
        case 0xdc:
            ea = ea_direct();
            set_reg_d(read16(ea));
            inst_tst16(get_reg_d());
            cycles += 5;
            break;
        case 0xec:
            ea = ea_indexed(&cycles);
            set_reg_d(read16(ea));
            inst_tst16(get_reg_d());
            cycles += 5;
            break;
        case 0xfc:
            ea = ea_extended();
            set_reg_d(read16(ea));
            inst_tst16(get_reg_d());
            cycles += 6;
            break;
        case 0xdd:
            ea = ea_direct();
            write16(ea, get_reg_d());
            inst_tst16(get_reg_d());
            cycles += 5;
            break;
        case 0xed:
            ea = ea_indexed(&cycles);
            write16(ea, get_reg_d());
            inst_tst16(get_reg_d());
            cycles += 5;
            break;
        case 0xfd:
            ea = ea_extended();
            write16(ea, get_reg_d());
            inst_tst16(get_reg_d());
            cycles += 6;
            break;
        case 0x12:
            cycles += 2;
            break;
        case 0x3d:
            r = (reg_a & 0xff) * (reg_b & 0xff);
            set_reg_d(r);
            set_cc(FLAG_Z, test_z16(r));
            set_cc(FLAG_C, (r >> 7) & 1);
            cycles += 11;
            break;
        case 0x20:
        case 0x21:
            inst_bra8(0, op, &cycles);
            break;
        case 0x22:
        case 0x23:
            inst_bra8(get_cc(FLAG_C) | get_cc(FLAG_Z), op, &cycles);
            break;
        case 0x24:
        case 0x25:
            inst_bra8(get_cc(FLAG_C), op, &cycles);
            break;
        case 0x26:
        case 0x27:
            inst_bra8(get_cc(FLAG_Z), op, &cycles);
            break;
        case 0x28:
        case 0x29:
            inst_bra8(get_cc(FLAG_V), op, &cycles);
            break;
        case 0x2a:
        case 0x2b:
            inst_bra8(get_cc(FLAG_N), op, &cycles);
            break;
        case 0x2c:
        case 0x2d:
            inst_bra8(get_cc(FLAG_N) ^ get_cc(FLAG_V), op, &cycles);
            break;
        case 0x2e:
        case 0x2f:
            inst_bra8(get_cc(FLAG_Z) | (get_cc(FLAG_N) ^ get_cc(FLAG_V)), op, &cycles);
            break;
        case 0x16:
            r = pc_read16();
            reg_pc += r;
            cycles += 5;
            break;
        case 0x17:
            r = pc_read16();
            push16(&reg_s, reg_pc);
            reg_pc += r;
            cycles += 9;
            break;
        case 0x8d:
            r = pc_read8();
            push16(&reg_s, reg_pc);
            reg_pc += sign_extend(r);
            cycles += 7;
            break;
        case 0x9d:
            ea = ea_direct();
            push16(&reg_s, reg_pc);
            reg_pc = ea;
            cycles += 7;
            break;
        case 0xad:
            ea = ea_indexed(&cycles);
            push16(&reg_s, reg_pc);
            reg_pc = ea;
            cycles += 7;
            break;
        case 0xbd:
            ea = ea_extended();
            push16(&reg_s, reg_pc);
            reg_pc = ea;
            cycles += 8;
            break;
        case 0x30:
            reg_x = ea_indexed(&cycles);
            set_cc(FLAG_Z, test_z16(reg_x));
            cycles += 4;
            break;
        case 0x31:
            reg_y = ea_indexed(&cycles);
            set_cc(FLAG_Z, test_z16(reg_y));
            cycles += 4;
            break;
        case 0x32:
            reg_s = ea_indexed(&cycles);
            cycles += 4;
            break;
        case 0x33:
            reg_u = ea_indexed(&cycles);
            cycles += 4;
            break;
        case 0x34:
            inst_psh(pc_read8(), &reg_s, reg_u, &cycles);
            cycles += 5;
            break;
        case 0x35:
            inst_pul(pc_read8(), &reg_s, &reg_u, &cycles);
            cycles += 5;
            break;
        case 0x36:
            inst_psh(pc_read8(), &reg_u, reg_s, &cycles);
            cycles += 5;
            break;
        case 0x37:
            inst_pul(pc_read8(), &reg_u, &reg_s, &cycles);
            cycles += 5;
            break;
        case 0x39:
            reg_pc = pull16(&reg_s);
            cycles += 5;
            break;
        case 0x3a:
            reg_x += reg_b & 0xff;
            cycles += 3;
            break;
        case 0x1a:
            reg_cc |= pc_read8();
            cycles += 3;
            break;
        case 0x1c:
            reg_cc &= pc_read8();
            cycles += 3;
            break;
        case 0x1d:
            set_reg_d(sign_extend(reg_b));
            set_cc(FLAG_N, test_n(reg_a));
            set_cc(FLAG_Z, test_z16(get_reg_d()));
            cycles += 2;
            break;
        case 0x1e:
            inst_exg();
            cycles += 8;
            break;
        case 0x1f:
            inst_tfr();
            cycles += 6;
            break;
        case 0x3b:
            if (get_cc(FLAG_E)) {
                inst_pul(0xff, &reg_s, &reg_u, &cycles);
            } else {
                inst_pul(0x81, &reg_s, &reg_u, &cycles);
            }
            cycles += 3;
            break;
        case 0x3f:
            set_cc(FLAG_E, 1);
            inst_psh(0xff, &reg_s, reg_u, &cycles);
            set_cc(FLAG_I, 1);
            set_cc(FLAG_F, 1);
            reg_pc = read16(0xfffa);
            cycles += 7;
            break;
        case 0x13:
            irq_status = IRQ_SYNC;
            cycles += 2;
            break;
        case 0x19:
            i0 = reg_a;
            i1 = 0;
            if ((reg_a & 0x0f) > 0x09 || get_cc(FLAG_H) == 1) {
                i1 |= 0x06;
            }
            if ((reg_a & 0xf0) > 0x80 && (reg_a & 0x0f) > 0x09) {
                i1 |= 0x60;
            }
            if ((reg_a & 0xf0) > 0x90 || get_cc(FLAG_C) == 1) {
                i1 |= 0x60;
            }
            reg_a = i0 + i1;
            set_cc(FLAG_N, test_n(reg_a));
            set_cc(FLAG_Z, test_z8(reg_a));
            set_cc(FLAG_V, 0);
            set_cc(FLAG_C, test_c(i0, i1, reg_a, 0));
            cycles += 2;
            break;
        case 0x3c:
            reg_cc &= pc_read8();
            set_cc(FLAG_E, 1);
            inst_psh(0xff, &reg_s, reg_u, &cycles);
            irq_status = IRQ_CWAI;
            cycles += 4;
            break;
        case 0x10:
            op = pc_read8();
            switch (op) {
                case 0x20:
                case 0x21:
                    inst_bra16(0, op, &cycles);
                    break;
                case 0x22:
                case 0x23:
                    inst_bra16(get_cc(FLAG_C) | get_cc(FLAG_Z), op, &cycles);
                    break;
                case 0x24:
                case 0x25:
                    inst_bra16(get_cc(FLAG_C), op, &cycles);
                    break;
                case 0x26:
                case 0x27:
                    inst_bra16(get_cc(FLAG_Z), op, &cycles);
                    break;
                case 0x28:
                case 0x29:
                    inst_bra16(get_cc(FLAG_V), op, &cycles);
                    break;
                case 0x2a:
                case 0x2b:
                    inst_bra16(get_cc(FLAG_N), op, &cycles);
                    break;
                case 0x2c:
                case 0x2d:
                    inst_bra16(get_cc(FLAG_N) ^ get_cc(FLAG_V), op, &cycles);
                    break;
                case 0x2e:
                case 0x2f:
                    inst_bra16(get_cc(FLAG_Z) | (get_cc(FLAG_N) ^ get_cc(FLAG_V)), op, &cycles);
                    break;
                case 0x83:
                    inst_sub16(get_reg_d(), pc_read16());
                    cycles += 5;
                    break;
                case 0x93:
                    ea = ea_direct();
                    inst_sub16(get_reg_d(), read16(ea));
                    cycles += 7;
                    break;
                case 0xa3:
                    ea = ea_indexed(&cycles);
                    inst_sub16(get_reg_d(), read16(ea));
                    cycles += 7;
                    break;
                case 0xb3:
                    ea = ea_extended();
                    inst_sub16(get_reg_d(), read16(ea));
                    cycles += 8;
                    break;
                case 0x8c:
                    inst_sub16(reg_y, pc_read16());
                    cycles += 5;
                    break;
                case 0x9c:
                    ea = ea_direct();
                    inst_sub16(reg_y, read16(ea));
                    cycles += 7;
                    break;
                case 0xac:
                    ea = ea_indexed(&cycles);
                    inst_sub16(reg_y, read16(ea));
                    cycles += 7;
                    break;
                case 0xbc:
                    ea = ea_extended();
                    inst_sub16(reg_y, read16(ea));
                    cycles += 8;
                    break;
                case 0x8e:
                    reg_y = pc_read16();
                    inst_tst16(reg_y);
                    cycles += 4;
                    break;
                case 0x9e:
                    ea    = ea_direct();
                    reg_y = read16(ea);
                    inst_tst16(reg_y);
                    cycles += 6;
                    break;
                case 0xae:
                    ea    = ea_indexed(&cycles);
                    reg_y = read16(ea);
                    inst_tst16(reg_y);
                    cycles += 6;
                    break;
                case 0xbe:
                    ea    = ea_extended();
                    reg_y = read16(ea);
                    inst_tst16(reg_y);
                    cycles += 7;
                    break;
                case 0x9f:
                    ea = ea_direct();
                    write16(ea, reg_y);
                    inst_tst16(reg_y);
                    cycles += 6;
                    break;
                case 0xaf:
                    ea = ea_indexed(&cycles);
                    write16(ea, reg_y);
                    inst_tst16(reg_y);
                    cycles += 6;
                    break;
                case 0xbf:
                    ea = ea_extended();
                    write16(ea, reg_y);
                    inst_tst16(reg_y);
                    cycles += 7;
                    break;
                case 0xce:
                    reg_s = pc_read16();
                    inst_tst16(reg_s);
                    cycles += 4;
                    break;
                case 0xde:
                    ea    = ea_direct();
                    reg_s = read16(ea);
                    inst_tst16(reg_s);
                    cycles += 6;
                    break;
                case 0xee:
                    ea    = ea_indexed(&cycles);
                    reg_s = read16(ea);
                    inst_tst16(reg_s);
                    cycles += 6;
                    break;
                case 0xfe:
                    ea    = ea_extended();
                    reg_s = read16(ea);
                    inst_tst16(reg_s);
                    cycles += 7;
                    break;
                case 0xdf:
                    ea = ea_direct();
                    write16(ea, reg_s);
                    inst_tst16(reg_s);
                    cycles += 6;
                    break;
                case 0xef:
                    ea = ea_indexed(&cycles);
                    write16(ea, reg_s);
                    inst_tst16(reg_s);
                    cycles += 6;
                    break;
                case 0xff:
                    ea = ea_extended();
                    write16(ea, reg_s);
                    inst_tst16(reg_s);
                    cycles += 7;
                    break;
                case 0x3f:
                    set_cc(FLAG_E, 1);
                    inst_psh(0xff, &reg_s, reg_u, &cycles);
                    reg_pc = read16(0xfff4);
                    cycles += 8;
                    break;
                default:
                    printf("unknown page-1 op code: %.2lx\n", op);
                    break;
            }
            break;
        case 0x11:
            op = pc_read8();
            switch (op) {
                case 0x83:
                    inst_sub16(reg_u, pc_read16());
                    cycles += 5;
                    break;
                case 0x93:
                    ea = ea_direct();
                    inst_sub16(reg_u, read16(ea));
                    cycles += 7;
                    break;
                case 0xa3:
                    ea = ea_indexed(&cycles);
                    inst_sub16(reg_u, read16(ea));
                    cycles += 7;
                    break;
                case 0xb3:
                    ea = ea_extended();
                    inst_sub16(reg_u, read16(ea));
                    cycles += 8;
                    break;
                case 0x8c:
                    inst_sub16(reg_s, pc_read16());
                    cycles += 5;
                    break;
                case 0x9c:
                    ea = ea_direct();
                    inst_sub16(reg_s, read16(ea));
                    cycles += 7;
                    break;
                case 0xac:
                    ea = ea_indexed(&cycles);
                    inst_sub16(reg_s, read16(ea));
                    cycles += 7;
                    break;
                case 0xbc:
                    ea = ea_extended();
                    inst_sub16(reg_s, read16(ea));
                    cycles += 8;
                    break;
                case 0x3f:
                    set_cc(FLAG_E, 1);
                    inst_psh(0xff, &reg_s, reg_u, &cycles);
                    reg_pc = read16(0xfff2);
                    cycles += 8;
                    break;
                default:
                    printf("unknown page-2 op code: %.2lx\n", op);
                    break;
            }
            break;
        default:
            printf("unknown page-0 op code: %.2lx\n", op);
            break;
    }
    return cycles;
}
