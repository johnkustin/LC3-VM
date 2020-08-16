#include "lc3instructions.h"


/**
 * mem_read
 * reads from RAM
 */
uint16_t mem_read(const uint16_t addr){
    return ram[addr];
}


uint16_t sign_extend(uint16_t x, int bit_count){
    if (x >> (bit_count - 1)){
        // MSb is 1 so x is negative
        x |= (UINT16_MAX << bit_count);
    };
    return x;
}

void update_flags(uint16_t regNum){
    if (reg[regNum] == 0){
        reg[R_COND] = FLG_ZRO;
    }
    else if (reg[regNum] >> 15){
        reg[R_COND] = FLG_NEG;
    }
    else{
        reg[R_COND] = FLG_POS;
    }
}

/**
 * instr_add
 * implements the add instruction
 * add encodings:
 * 1. add
 * [15-12] 0b0001
 * [11-9] destination reg
 * [8-6] source reg 1
 * [5-3] 0
 * [2-0] source reg 2
 * 2. add immediate
 * [15-12] 0b0001
 * [11-9] destination reg
 * [8-6] source reg 1
 * [5] 0
 * [4-0] immediate
 */
void instr_add(uint16_t instr){
    uint8_t dest_reg = (instr >> 9) & 0b111;
    uint8_t src_reg_1 = (instr >> 6) & 0b111;
    uint8_t immediate = (instr >> 5) & 0b1;

    if (immediate){
        uint16_t imm = sign_extend((instr & 0b11111), 5);
        reg[dest_reg] = reg[src_reg_1] + imm;
    }
    else{
        uint8_t src_reg_2 = instr & 0b111;
        reg[dest_reg] = reg[src_reg_1] + reg[src_reg_2];
    }

    update_flags(dest_reg);
}

/**
 * instr_ldi
 * implements the load indirect instruction
 * ldi encoding:
 * [15-12] 0b1010
 * [11-9] destination reg
 * [8-0] program counter offset
 */
void instr_ldi(uint16_t instr){
    uint8_t dest_reg = (instr >> 9) & 0b111;
    uint16_t pc_offset = sign_extend((instr & 0x1FF), 9);
    /* increment current PC by pc_offset, read memory at that location in RAM into dest_reg */
    reg[dest_reg] = mem_read(reg[R_PC] + pc_offset);
    update_flags(dest_reg);
}