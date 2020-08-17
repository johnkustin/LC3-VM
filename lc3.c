/**  
 * LC-3 Virtual Machine
 * Based on 
 * Write your Own Virtual Machine
 * By: Justin Meiners and Ryan Pendleton 
 * https://justinmeiners.github.io/lc3-vm/index.html#1:12
 */

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


#include "lc3instructions.h"


uint16_t read_image(char* argv[]){
    return 0;
}

/**
 * parse_args
 * parses CL arguments to start the VM
 */
void parse_args(int argc, char* argv[]){
    if (argc < 2){
        printf("lc3 [path-to-image-file1] ...\n");
        exit(2);
    }

    for(int i = 1; i < argc; i++){
        if(!read_image(argv[i])){
            printf("\nFailed to load image: %s\n", argv[i]);
            exit(1);    
        }
    }
}

void run_trap_switch(uint16_t instruxn){
    reg[R_7] = reg[R_PC]; // save program counter for after trap code exec
    switch (instruxn & 0xFF){
        case TRAP_GETC:
            {TRAP GETC, 9}
            break;
        case TRAP_OUT:
            {TRAP OUT, 9}
            break;
        case TRAP_PUTS:
            {TRAP PUTS, 8}
            break;
        case TRAP_IN:
            {TRAP IN, 9}
            break;
        case TRAP_PUTSP:
            {TRAP PUTSP, 9}
            break;
        case TRAP_HALT:
            {TRAP HALT, 9}
            break;
    }
}

void run_opcode_switch(uint8_t opcode, uint16_t instruxn){
    switch (opcode)
        {
        case OP_ADD:
                instr_add(instruxn);
                break;
            case OP_AND:
                instr_and(instruxn);
                break;
            case OP_NOT:
                instr_not(instruxn);
                break;
            case OP_BR:
                instr_br(instruxn);
                break;
            case OP_JMP:
                instr_jmp(instruxn);
                break;
            case OP_JSR:
                instr_jsr(instruxn);
                break;
            case OP_LD:
                instr_ld(instruxn);
                break;
            case OP_LDI:
                instr_ldi(instruxn);
                break;
            case OP_LDR:
                instr_ldr(instruxn);
                break;
            case OP_LEA:
                instr_lea(instruxn);
                break;
            case OP_ST:
                instr_st(instruxn);
                break;
            case OP_STI:
                instr_sti(instruxn);
                break;
            case OP_STR:
                instr_str(instruxn);
                break;
            case OP_TRAP:
                run_trap_switch(instruxn);
                break;
            case OP_RES:
            case OP_RTI:
        default:
            bad_opcode();
            break;
        }
}



// MAIN LOOP OF EXECUTION
int main(int argc, char* argv[]){

    // load CL args

    // setup

    // init PC
    uint16_t PC_START  = 0x3000;
    reg[R_PC] = PC_START;
    
    int active = 1;
    while (active){
        // fetch instruction
        uint16_t instruxn = mem_read(reg[R_PC]++);
        uint8_t opcode = instruxn >> 12;
        run_opcode_switch(opcode, instruxn);
    }

    // shutdown sequence
}