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

// TRAP CODES
enum trap_codes{
    TRAP_GETC = 0x20,  /* get character from keyboard, not echoed onto the terminal */
    TRAP_OUT = 0x21,   /* output a character */
    TRAP_PUTS = 0x22,  /* output a word string */
    TRAP_IN = 0x23,    /* get character from keyboard, echoed onto the terminal */
    TRAP_PUTSP = 0x24, /* output a byte string */
    TRAP_HALT = 0x25   /* halt the program */
};

uint16_t mem_read(const uint16_t addr);
uint16_t sign_extend(uint16_t x, int bit_count);
void update_flags(uint16_t regNum);
void instr_add(uint16_t instr);
void instr_ldi(uint16_t instr);
void instr_and(uint16_t instr);
void instr_br(uint16_t instr);
void instr_jmp(uint16_t instr);
void instr_jsr(uint16_t instr);
void instr_ld(uint16_t instr);
void instr_ldi(uint16_t instr);
void instr_ldr(uint16_t instr);
void instr_lea(uint16_t instr);
void instr_not(uint16_t instr);
void instr_st(uint16_t instr);
void instr_sti(uint16_t instr);
void instr_str(uint16_t instr);
void trap_puts();
void trap_out();
void trap_in();
void trap_putsp();
void trap_halt();
void bad_opcode();
