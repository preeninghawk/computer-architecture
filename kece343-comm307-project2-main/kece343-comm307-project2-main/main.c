/***************************************************************/
/*                                                             */
/*   RISC-V (RV32I) Instruction Level Simulator                */
/*                                                             */
/*   KECE343/COMM307 Korea University                          */
/*   main.c                                                    */
/*                                                             */
/***************************************************************/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*          DO NOT MODIFY THIS FILE!                            */
/*          You should only change proc.c, loader.c file!       */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>

#include "proc.h"
#include "mem.h"
#include "loader.h"
#include "util.h"


/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename) {
    init_memory();
    load_program(program_filename);
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
    int debug_set = 0;
    
    int i = 100;		//for loop

    int num_inst_set = 0;
    int num_inst = 0;

    int mem_dump_set = 0;
    int start_addr = 0;
    int end_addr = 0;

    int opt;
    extern int optind;

    mem_dump_set = TRUE;
    start_addr = 0x10000000;
    end_addr = 0x1000001f;
    
    while ((opt=getopt(argc, argv, "m:dn:")) != -1) {
        switch (opt) {
            case 'd':
                debug_set = 1;
                break;

            case 'n':
                num_inst_set = 1;
                num_inst = atoi(optarg);
                break;
        }
    }


    if (optind+1 > argc) {
        fprintf(stderr,  "Usage: %s [-d] [-n num_instr] RISC_V_binary\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    initialize(argv[argc-1]);

    if (num_inst_set)
        i = num_inst;

    if (debug_set) {
        printf("[DEBUG] Simulating for %d cycles...\n\n", i);

        for(; i > 0; i--){
            cycle();
            rdump();

            if (mem_dump_set)
                mdump(start_addr, end_addr);
        }

    } else {
        run(i);
        rdump();

        if (mem_dump_set)
            mdump(start_addr, end_addr);
    }

    exit(EXIT_SUCCESS);
}
