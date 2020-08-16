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
        switch (opcode)
        {
        case OP_ADD:
                instr_add(instruxn);
                break;
            case OP_AND:
                {AND, 7}
                break;
            case OP_NOT:
                {NOT, 7}
                break;
            case OP_BR:
                {BR, 7}
                break;
            case OP_JMP:
                {JMP, 7}
                break;
            case OP_JSR:
                {JSR, 7}
                break;
            case OP_LD:
                {LD, 7}
                break;
            case OP_LDI:
                instr_ldi(instruxn);
                break;
            case OP_LDR:
                {LDR, 7}
                break;
            case OP_LEA:
                {LEA, 7}
                break;
            case OP_ST:
                {ST, 7}
                break;
            case OP_STI:
                {STI, 7}
                break;
            case OP_STR:
                {STR, 7}
                break;
            case OP_TRAP:
                {TRAP, 8}
                break;
            case OP_RES:
            case OP_RTI:
        default:
            {BAD OPCODE, 7}
            break;
        }
    }

    // shutdown sequence
}