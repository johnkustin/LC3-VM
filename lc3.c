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



/**
 * swap16
 * swaps endianness
 */
uint16_t swap16(uint16_t val){
    return (val << 8) | (val >> 8);
}

void read_image_file(FILE* file){
    // get origin to know where in memory to put the image
    uint16_t origin;
    fread(&origin, sizeof(origin), 1, file);
    origin = swap16(origin);

    uint16_t max_read = UINT16_MAX - origin;
    uint16_t* mem_loc = ram + origin;
    size_t read = fread(mem_loc, sizeof(uint16_t), max_read, file);

    while (read-- > 0){
        *mem_loc = swap16(*mem_loc);
        mem_loc++;
    }
}

int read_image(const char* image_path){
    FILE* file = fopen(image_path, "rb");
    if (!file) return 0;
    read_image_file(file);
    fclose(file);
    return 1;
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

void run_trap_switch(uint16_t instruxn, int* running){
    switch (instruxn & 0xFF){
        case TRAP_GETC:
            trap_getc();
            break;
        case TRAP_OUT:
            trap_out();
            break;
        case TRAP_PUTS:
            trap_puts();
            break;
        case TRAP_IN:
            trap_in();
            break;
        case TRAP_PUTSP:
            trap_putsp();
            break;
        case TRAP_HALT:
            trap_halt(running);
            break;
    }
}

void run_opcode_switch(uint8_t opcode, uint16_t instruxn, int* active){
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
                run_trap_switch(instruxn, active);
                break;
            case OP_RES:
            case OP_RTI:
        default:
            bad_opcode();
            break;
        }
}

// INPUT BUFFERING
struct termios original_tio;

void disable_input_buffering()
{
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

// INTERRUPT HANDLING
void handle_interrupt(int signal){
    restore_input_buffering();
    printf("\n");
    exit(-2);
}

// MAIN LOOP OF EXECUTION
int main(int argc, char* argv[]){

    // load CL args
    parse_args(argc, argv);

    // setup
    signal(SIGINT, handle_interrupt);
    disable_input_buffering();

    // init PC
    uint16_t PC_START  = 0x3000;
    reg[R_PC] = PC_START;
    
    int active = 1;
    while (active){
        // fetch instruction
        uint16_t instruxn = mem_read(reg[R_PC]++);
        uint8_t opcode = instruxn >> 12;
        run_opcode_switch(opcode, instruxn, &active);
    }

    // shutdown sequence
    restore_input_buffering();
}