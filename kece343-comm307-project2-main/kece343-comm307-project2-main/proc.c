/***************************************************************/
/*                                                             */
/*   RISC-V (RV32I) Instruction Level Simulator                */
/*                                                             */
/*   KECE343/COMM307 Korea University                          */
/*   proc.c                                                    */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <malloc.h>

#include "proc.h"
#include "mem.h"
#include "util.h"

/***************************************************************/
/* System (CPU and Memory) info.                                             */
/***************************************************************/
struct RISC_V_proc_t g_processor;


/***************************************************************/
/* Fetch an instruction indicated by PC                        */
/***************************************************************/
int fetch(uint32_t pc)
{
    return mem_read_32(pc);
}

/***************************************************************/
/* TODO: Decode the given encoded 32bit data (word)            */
/***************************************************************/
struct inst_t decode(int word)
{
    char func7[8], rs2[6], rs1[6], func3[4], rd[6], opcode[8], imm[21] = {0};
    char* buffer = dec_to_bin(word);
    struct inst_t inst;

    // TODO

    return inst;
}

/***************************************************************/
/* TODO: Execute the decoded instruction                       */
/***************************************************************/
void execute(struct inst_t inst)
{
    // TODO

    if (g_processor.pc < g_mem_regions[0].start ||
	    g_processor.pc >= (g_mem_regions[0].start + (g_processor.input_insts * 4)))
    {
	    g_processor.running = FALSE;
    }

}

/***************************************************************/
/* Advance a cycle                                             */
/***************************************************************/
void cycle()
{
    // TODO : Check if something is wrong!!

    int inst_reg;
    struct inst_t inst;

    // 1. fetch
    inst_reg = fetch(g_processor.pc);

    // 2. decode
    inst = decode(inst_reg);

    // 3. execute
    execute(inst);

    // 4. update stats
    
    g_processor.pc += BYTES_PER_WORD;
    
    g_processor.pc_hold = FALSE;
    g_processor.num_insts++;
    g_processor.ticks++;
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump() {
    int k;

    printf("\n[INFO] Current register values :\n");
    printf("-------------------------------------\n");
    printf("PC: 0x%08x\n", g_processor.pc);
    printf("Registers:\n");
    for (k = 0; k < RISC_V_REGS; k++)
        printf("x%d: 0x%08x\n", k, g_processor.regs[k]);
}



/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate RISC-V for n cycles                    */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
    int i;

    if (g_processor.running == FALSE) {
        printf("[ERROR] Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("[INFO] Simulating for %d cycles...\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
        if (g_processor.running == FALSE) {
            printf("[INFO] Simulator halted\n");
            break;
        }
        cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate RISC-V until HALTed                    */
/*                                                             */
/***************************************************************/
void go() {
    if (g_processor.running == FALSE) {
        printf("[ERROR] Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("[INFO] Simulating...\n");
    while (g_processor.running)
        cycle();
    printf("[INFO] Simulator halted\n");
}
