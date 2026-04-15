/***************************************************************/
/*                                                             */
/*   RISC-V (RV32I) Instruction Level Simulator                */
/*                                                             */
/*   KECE343/COMM307 Korea University                          */
/*   loader.h                                                  */
/*                                                             */
/***************************************************************/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*          DO NOT MODIFY THIS FILE!                            */
/*          You should only change proc.c, loader.c file!       */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#ifndef __LOADER_H__
#define __LOADER_H__

#include "proc.h"

extern struct RISC_V_proc_t g_processor;


void        load_inst_to_mem(const char *buffer, const int index);
void        load_data_to_mem(const char *buffer, const int index);
void        load_program(char *program_filename);

#endif
