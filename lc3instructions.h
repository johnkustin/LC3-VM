#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

// RAM memory
uint16_t ram[UINT16_MAX];

// CPU REGISTERS
enum regs{
    R_0 = 0,
    R_1,
    R_2,
    R_3,
    R_4,
    R_5,
    R_6,
    R_7,
    R_PC,
    R_COND,
    R_COUNT
};

uint16_t reg[R_COUNT];

// OPCODES
enum opcodes{
    OP_BR = 0, /* branch */
    OP_ADD,    /* add  */
    OP_LD,     /* load */
    OP_ST,     /* store */
    OP_JSR,    /* jump register */
    OP_AND,    /* bitwise and */
    OP_LDR,    /* load register */
    OP_STR,    /* store register */
    OP_RTI,    /* unused */
    OP_NOT,    /* bitwise not */
    OP_LDI,    /* load indirect */
    OP_STI,    /* store indirect */
    OP_JMP,    /* jump */
    OP_RES,    /* reserved (unused) */
    OP_LEA,    /* load effective address */
    OP_TRAP    /* execute trap */
};

// CONDITION FLAGS
enum cond_flgs{
    FLG_POS = 1 << 0,
    FLG_ZRO = 1 << 1,
    FLG_NEG = 1 << 2
};

uint16_t mem_read(const uint16_t addr);
uint16_t sign_extend(uint16_t* x, int bit_count);
void update_flags(uint16_t regNum);
void instr_add(uint16_t instr);
void instr_ldi(uint16_t instr);
void instr_and(uint16_t instr);
