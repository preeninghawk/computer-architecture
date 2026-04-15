/***************************************************************/
/*                                                             */
/*   RISC-V (RV32I) Instruction Level Simulator                */
/*                                                             */
/*   KECE343/COMM307 Korea University                          */
/*   pro.h                                                     */
/*                                                             */
/***************************************************************/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*          DO NOT MODIFY THIS FILE!                            */
/*          You should only change proc.c, loader.c file!       */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#ifndef __PROC_H__
#define __PROC_H__

#include <stdint.h>

/* Basic Information */
#define RISC_V_REGS      32
#define BYTES_PER_WORD 4


struct RISC_V_proc_t {
    uint32_t pc;                /* Program counter */
    short pc_hold;              /* Program counter update flag (Consider jumping) */
    uint32_t regs[RISC_V_REGS];    /* Register file */

    uint32_t num_insts;         /* Number of executed instructions */
    uint32_t ticks;             /* Clock ticks (cycles) */

    int      running;           /* Runnning or halted */
    int      input_insts;       /* Size of input instructiosn */
};

/* You should decode your instructions from the
 * ASCII-binary format to this structured format */
struct inst_t {
    int32_t imm;
    short func7;
    short rs2;
    short rs1;
    short func3;
    short rd;
    short opcode;

    uint32_t value;
};


void		    rdump();
void		    run(int num_cycles);
void		    go();

/* key functions */
void            execute(struct inst_t);
struct inst_t   decode(int inst_word);
int             fetch(uint32_t pc);
void		    cycle();

/* Sign Extension */
#define SIGN_EX(X) (((X) & 0x8000) ? ((X) | 0xffff0000) : (X))

#define COND_UN		0x1
#define COND_EQ		0x2
#define COND_LT		0x4
#define COND_IN		0x8


/* Minimum and maximum values that fit in instruction's imm field */
#define IMM_MIN		0xffff8000
#define IMM_MAX 	0x00007fff

#define UIMM_MIN  	(unsigned)0
#define UIMM_MAX  	((unsigned)((1<<16)-1))

#define BRANCH_INST(TEST, TARGET, NULLIFY)	\
{						\
    if (TEST)					\
    {						\
        uint32_t target = (TARGET);		\
        JUMP_INST(target)			\
    }						\
}


#define JUMP_INST(TARGET)			\
{						\
    g_processor.pc = (TARGET);		\
    g_processor.pc_hold = TRUE; \
}

#define LOAD_INST(DEST_A, LD, MASK)		\
{						\
    LOAD_INST_BASE (DEST_A, (LD & (MASK)))	\
}

#define LOAD_INST_BASE(DEST_A, VALUE)		\
{						\
    *(DEST_A) = (VALUE);			\
}

#endif
