#include "lc3instructions.h"


// unix check_key
uint16_t check_key(){
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

/**
 * mem_read
 * reads from RAM
 */
uint16_t mem_read(uint16_t addr){
    if (addr == MR_KBSR){
        if (check_key()){
            ram[MR_KBSR] = 1 << 15;
            ram[MR_KBDR] = getchar();
        }
        else{
            ram[MR_KBSR] = 0;
        }  
    }
    return ram[addr];
}

/**
 * mem_write
 * writes a byte, data, to RAM at address offset
 */
void mem_write(uint16_t offset, uint16_t data){
    ram[offset] = data;
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


void bad_opcode(){
    abort();
}

/**
 * instr_add
 * implements the add instruction
 * add encodings:
 * 1. add
 * [15-12] 0b0001
 * [11-9] destination reg
 * [8-6] source reg 1
 * [5-3] 0 (indicates this type of add instruction)
 * [2-0] source reg 2
 * 2. add immediate
 * [15-12] 0b0001
 * [11-9] destination reg
 * [8-6] source reg 1
 * [5] 1 (indicates this type of add instruction)
 * [4-0] immediate
 */
void instr_add(uint16_t instr){
    uint8_t dest_reg = (instr >> 9) & 0b111;
    uint8_t src_reg_1 = (instr >> 6) & 0b111;
    uint8_t immediate = (instr >> 5) & 0b1;

    if (immediate){
        uint16_t imm = sign_extend((instr & 0x1F), 5);
        reg[dest_reg] = reg[src_reg_1] + imm;
    }
    else{
        uint8_t src_reg_2 = instr & 0b111;
        reg[dest_reg] = reg[src_reg_1] + reg[src_reg_2];
    }

    update_flags(dest_reg);
}

/**
 * instr_and
 * impelemts the and instruction
 * and encodings:
 * 1. and 
 * [15-12] 0b0101
 * [11-9] destination reg
 * [8-6] source reg 1
 * [5-3] 0 (indicates this type of and instruction)
 * [2-0] source reg 2
 * 2. and immediate
 * [15-12] 0b0101
 * [11-9] destination reg
 * [8-6] source reg 1
 * [5] 0 (indicates this type of and instruction)
 * [4-0] immediate
 */
void instr_and(uint16_t instr){
    uint8_t dest_reg = (instr >> 9) & 0b111;
    uint8_t src_reg_1 = (instr >> 6) & 0b111;
    uint8_t immediate = (instr >> 5) & 1;
    if (immediate){
        uint16_t imm = sign_extend(instr & 0x1F, 5);
        reg[dest_reg] = reg[src_reg_1] & imm;
    }
    else{
        uint8_t src_reg_2 = instr & 0b111;
        reg[dest_reg] = reg[src_reg_1] & reg[src_reg_2];
    }
    update_flags(dest_reg);
}

/**
 * instr_br
 * implements the conditional branch instruction
 * br encoding:
 * [15-12] 0b0000
 * [11] n, branch if negative
 * [10] z, branch if zero
 * [9] p, branch if positive
 * [8-0] program counter offset
 */
void instr_br(uint16_t instr){
    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
    uint8_t cond_flg = (instr >> 9) & 0b111;
    if (cond_flg & reg[R_COND]){
        reg[R_PC] += pc_offset;
    }
}

/**
 * instr_jmp
 * implements both the jump and return from subroutine instructions
 * jmp & ret encodings:
 * jmp:
 * [15-12] 0b1100
 * [11-9] 0
 * [8-6] base register
 * [5-0] 0
 * ret:
 * same as jmp but base register = 7 (R_7)
 */
void instr_jmp(uint16_t instr){
    uint8_t base_reg = (instr >> 6) & 0b111; // also handles RET
    reg[R_PC] = reg[base_reg];
}

/**
 * instr_jsr
 * implements the jump to subroutine instruction
 * encodings
 * 1. jsr
 * [15-12] 0b0100
 * [11] 1 (distinguishes the jsr from jsrr)
 * [10-0] program counter offset
 * 2. jsrr
 * [15-12] 0b0100
 * [11-9] 0 (distinguishes the jsrr from jsr)
 * [8-6] base_register
 * [5-0] 0
 */
void instr_jsr(uint16_t instr){
    uint8_t isRegister = (instr >> 11) & 1;
    reg[R_7] = reg[R_PC];
    if (isRegister){
        uint16_t pc_offset = instr & 0x7FF;
        reg[R_PC] += sign_extend(pc_offset, 11);
    }
    else{
        uint8_t base_reg = (instr >> 6) & 0x7;
        reg[R_PC] = reg[base_reg];
    }
}

/**
 * instr_ld
 * implements the load instruction
 * encoding
 * [15-12] 0b0010
 * [11-9] destination register
 * [8-0] program counter offset
 */
void instr_ld(uint16_t instr){
    uint8_t dest_reg = (instr >> 9) & 0x7;
    reg[dest_reg] = mem_read(reg[R_PC] + sign_extend(instr & 0x1FF, 9));
    update_flags(dest_reg);
}

/**
 * instr_ldi
 * implements the load indirect instruction
 * encoding
 * [15-12] 0b1010
 * [11-9] destination register
 * [8-0] program counter offset
 */
void instr_ldi(uint16_t instr){
    uint8_t dest_reg = (instr >> 9) & 0x7;
    reg[dest_reg] = mem_read(mem_read(reg[R_PC] + sign_extend(instr & 0x1FF, 9)));
    update_flags(dest_reg);
}


/** 
 * instr_ldr
 * implements the load base + offset instruction
 * encoding
 * [15-12] 0b0110
 * [11-9] destination register
 * [8-6] base register
 * [5-0] offset
 */
void instr_ldr(uint16_t instr){
    uint8_t dest_reg = (instr >> 9) & 0x7;
    uint8_t base_reg = (instr >> 6) & 0x7;
    reg[dest_reg] = mem_read(reg[base_reg] + sign_extend(instr & 0x3F, 6));
    update_flags(dest_reg);
}

/**
 * instr_lea
 * implements the load effective address instruction
 * encoding
 * [15-12] 0b1110
 * [11-9] destination register
 * [8-0] program counter offset
 */
void instr_lea(uint16_t instr){
    uint8_t dest_reg = (instr >> 9) & 0x7;
    reg[dest_reg] = reg[R_PC] + sign_extend(instr & 0x1FF, 9);
    update_flags(dest_reg);
}

/**
 * instr_not
 * implements the bit wise complement instruction
 * encoding
 * [15-12] 0b1001
 * [11-9] destination register
 * [8-6] source register
 * [5] 1
 * [4-0] 1
 */
void instr_not(uint16_t instr){
    uint8_t dest_reg = (instr >> 9) & 0x7;
    uint8_t src_reg = (instr >> 6) & 0x7;
    reg[dest_reg] = ~(reg[src_reg]);
    update_flags(dest_reg);
}

/**
 * instr_st
 * implements the store instruction
 * encoding
 * [15-12] 0b0011
 * [11-9] source register
 * [8-0] program counter offset
 */
void instr_st(uint16_t instr){
    uint8_t src_reg = (instr >> 9) & 0x7;
    mem_write(reg[R_PC] + sign_extend(instr & 0x1FF, 9), reg[src_reg]);
}

/**
 * instr_sti
 * implements the store indirect instruction
 * encoding
 * [15-12] 0b1011
 * [11-9] source register
 * [8-0] program counter offset
 */
void instr_sti(uint16_t instr){
    uint8_t src_reg = (instr >> 9) & 0x7;
    mem_write(mem_read(reg[R_PC] + sign_extend(instr & 0x1FF, 9)), reg[src_reg]);
}

/**
 * instr_str
 * implements the store base + offset instruction
 * encoding
 * [15-12] 0b110
 * [11-9] source register
 * [8-6] base register
 * [5-0] offset
 */
void instr_str(uint16_t instr){
    uint8_t src_reg = (instr >> 9 ) & 0x7;
    uint8_t base_reg = (instr >> 6) & 0x7;
    mem_write(reg[base_reg] + sign_extend(instr & 0x3F, 6), reg[src_reg]);
}

/**
 * trap_puts
 * implements the puts trap routine
 */
void trap_puts(){
    // one char per word of memory
    uint16_t* c = ram + reg[R_0];
    while (*c){
        putc((char)*c, stdout);
        c++; // lol c++
    }
    fflush(stdout);
}

/**
 * trap_getc
 * implements the get character trap routine
 */
void trap_getc(){
    reg[R_0] = (uint16_t)getchar(); // read a single ASCII char
}

/**
 * trap_out
 * write a char in R_0 to the console display
 */
void trap_out(){
    putc((char)reg[R_0], stdout);
    fflush(stdout);
}

/**
 * trap_in
 * prints a prompt on screen and read a single char from the keyboard.
 * the char is echoed to the console monitor and copied into R_0.
 */
void trap_in(){
    printf("Enter a character: ");
    char c = getchar();
    putc(c, stdout);
    reg[R_0] = (uint16_t)c;
}

/**
 * trap_putsp
 * writes a string of ASCII chars to the console
 */
void trap_putsp(){
    uint16_t* c = ram + reg[R_0];
    while (*c){
        char char1 = (*c) & 0xFF;
        putc(char1, stdout);
        char char2 = (*c) >> 8;
        if (char2) putc(char2, stdout);
        c++; // lmao c++ again
    }
    fflush(stdout);
}

/**
 * trap_halt
 * halts execution and prints a message to console
 */
void trap_halt(int* running){
    puts("HALT");
    fflush(stdout);
    *running = 0;
}