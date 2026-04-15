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
    char func7[8], rs2[6], rs1[6], func3[4], rd[6], opcode[8], imm[64] = {0};
    char* buffer = dec_to_bin(word);
    struct inst_t inst;

    // Store original instruction value
    inst.value = word;

    // Extract opcode (bits 0-6)
    strncpy(opcode, buffer + 32 - 7, 7);
    opcode[7] = '\0';
    inst.opcode = str_to_int(opcode);

    // Extract destination register rd (bits 7-11)
    strncpy(rd, buffer + 32 - 12, 5);
    rd[5] = '\0';
    inst.rd = str_to_int(rd);

    // Extract function code func3 (bits 12-14)
    strncpy(func3, buffer + 32 - 15, 3);
    func3[3] = '\0';
    inst.func3 = str_to_int(func3);

    // Extract source register rs1 (bits 15-19)
    strncpy(rs1, buffer + 32 - 20, 5);
    rs1[5] = '\0';
    inst.rs1 = str_to_int(rs1);

    // Extract source register rs2 (bits 20-24)
    strncpy(rs2, buffer + 32 - 25, 5);
    rs2[5] = '\0';
    inst.rs2 = str_to_int(rs2);

    // Extract function code func7 (bits 25-31)
    strncpy(func7, buffer + 32 - 32, 7);
    func7[7] = '\0';
    inst.func7 = str_to_int(func7);

    // Extract and calculate immediate values based on instruction type
    switch (inst.opcode) {
        // R-type instructions: add, sub, sll, slt, sltu, xor, srl, sra, or, and
        case 0b0110011:  // R-type
            inst.imm = 0;  // No immediate for R-type
            break;
            
        // I-type instructions: lb, lh, lw, lbu, lhu, addi, slti, sltiu, xori, ori, andi, slli, srli, srai
        case 0b0000011:  // Load instructions
        case 0b0010011:  // Immediate arithmetic
        case 0b1100111:  // jalr
            // Immediate is in bits 20-31
            memset(imm, 0, sizeof(imm));
            strncpy(imm, buffer, 12);
            imm[12] = '\0';
            inst.imm = str_to_signed_int(imm);
            break;
            
        // S-type instructions: sb, sh, sw
        case 0b0100011:  // Store instructions
            // Immediate is in bits 7-11 and 25-31
            memset(imm, 0, sizeof(imm));
            strncpy(imm, buffer, 7);     // imm[11:5]
            strncpy(imm + 7, buffer + 20, 5);  // imm[4:0]
            imm[12] = '\0';
            inst.imm = str_to_signed_int(imm);
            break;
            
        // B-type instructions: beq, bne, blt, bge, bltu, bgeu
        case 0b1100011:  // Branch instructions
            // Complex immediate encoding for B-type
            memset(imm, 0, sizeof(imm));
            imm[0] = buffer[0];       // imm[12]
            strncpy(imm + 1, buffer + 24, 1);  // imm[11]
            strncpy(imm + 2, buffer + 1, 6);   // imm[10:5]
            strncpy(imm + 8, buffer + 20, 4);  // imm[4:1]
            imm[12] = '0';           // imm[0] = 0
            imm[13] = '\0';
            inst.imm = str_to_signed_int(imm);
            break;
            
        // U-type instructions: lui, auipc
        case 0b0110111:  // lui
        case 0b0010111:  // auipc
            // Immediate is in bits 12-31, appended with 12 zeros
            memset(imm, 0, sizeof(imm));
            strncpy(imm, buffer, 20);
            strncpy(imm + 20, "000000000000", 12); // Add 12 zeros at the end
            imm[32] = '\0';
            inst.imm = str_to_int(imm);
            break;
            
        // J-type instruction: jal
        case 0b1101111:  // jal
            // Complex immediate encoding for J-type
            memset(imm, 0, sizeof(imm));
            imm[0] = buffer[0];       // imm[20]
            strncpy(imm + 1, buffer + 12, 8);  // imm[19:12]
            strncpy(imm + 9, buffer + 11, 1);  // imm[11]
            strncpy(imm + 10, buffer + 1, 10);  // imm[10:1]
            imm[20] = '0';           // imm[0] = 0
            imm[21] = '\0';
            inst.imm = str_to_signed_int(imm);
            break;
    }

    return inst;
}

/***************************************************************/
/* TODO: Execute the decoded instruction                       */
/***************************************************************/
void execute(struct inst_t inst)
{
    // Register x0 is hardwired to 0
    g_processor.regs[0] = 0;
    
    // By default, we'll increment PC by 4
    uint32_t next_pc = g_processor.pc + 4;
    
    switch (inst.opcode) {
        // R-type instructions
        case 0b0110011:  // R-type
            switch (inst.func3) {
                case 0b000:  // add, sub
                    if (inst.func7 == 0)
                        g_processor.regs[inst.rd] = g_processor.regs[inst.rs1] + g_processor.regs[inst.rs2];
                    else if (inst.func7 == 0b0100000)
                        g_processor.regs[inst.rd] = g_processor.regs[inst.rs1] - g_processor.regs[inst.rs2];
                    break;
                case 0b001:  // sll
                    g_processor.regs[inst.rd] = g_processor.regs[inst.rs1] << (g_processor.regs[inst.rs2] & 0x1F);
                    break;
                case 0b010:  // slt
                    g_processor.regs[inst.rd] = ((int32_t)g_processor.regs[inst.rs1] < (int32_t)g_processor.regs[inst.rs2]) ? 1 : 0;
                    break;
                case 0b011:  // sltu
                    g_processor.regs[inst.rd] = (g_processor.regs[inst.rs1] < g_processor.regs[inst.rs2]) ? 1 : 0;
                    break;
                case 0b100:  // xor
                    g_processor.regs[inst.rd] = g_processor.regs[inst.rs1] ^ g_processor.regs[inst.rs2];
                    break;
                case 0b101:  // srl, sra
                    if (inst.func7 == 0)  // srl
                        g_processor.regs[inst.rd] = g_processor.regs[inst.rs1] >> (g_processor.regs[inst.rs2] & 0x1F);
                    else if (inst.func7 == 0b0100000)  // sra (arithmetic shift)
                        g_processor.regs[inst.rd] = (int32_t)g_processor.regs[inst.rs1] >> (g_processor.regs[inst.rs2] & 0x1F);
                    break;
                case 0b110:  // or
                    g_processor.regs[inst.rd] = g_processor.regs[inst.rs1] | g_processor.regs[inst.rs2];
                    break;
                case 0b111:  // and
                    g_processor.regs[inst.rd] = g_processor.regs[inst.rs1] & g_processor.regs[inst.rs2];
                    break;
            }
            break;
            
        // I-type load instructions
        case 0b0000011:
            switch (inst.func3) {
                case 0b000:  // lb
                {
                    uint32_t addr = g_processor.regs[inst.rs1] + inst.imm;
                    uint32_t word = mem_read_32(addr & ~0x3);
                    uint32_t byte_offset = addr & 0x3;
                    uint32_t byte = (word >> (byte_offset * 8)) & 0xFF;
                    g_processor.regs[inst.rd] = (byte & 0x80) ? (byte | 0xFFFFFF00) : byte;  // Sign extend
                    break;
                }
                case 0b001:  // lh
                {
                    uint32_t addr = g_processor.regs[inst.rs1] + inst.imm;
                    uint32_t word = mem_read_32(addr & ~0x3);
                    uint32_t halfword;
                    if (addr & 0x2)
                        halfword = (word >> 16) & 0xFFFF;
                    else
                        halfword = word & 0xFFFF;
                    g_processor.regs[inst.rd] = (halfword & 0x8000) ? (halfword | 0xFFFF0000) : halfword;  // Sign extend
                    break;
                }
                case 0b010:  // lw
                {
                    uint32_t addr = g_processor.regs[inst.rs1] + inst.imm;
                    g_processor.regs[inst.rd] = mem_read_32(addr);
                    break;
                }
                case 0b100:  // lbu
                {
                    uint32_t addr = g_processor.regs[inst.rs1] + inst.imm;
                    uint32_t word = mem_read_32(addr & ~0x3);
                    g_processor.regs[inst.rd] = (word >> ((addr & 0x3) * 8)) & 0xFF;  // Zero extend
                    break;
                }
                case 0b101:  // lhu
                {
                    uint32_t addr = g_processor.regs[inst.rs1] + inst.imm;
                    uint32_t word = mem_read_32(addr & ~0x3);
                    if (addr & 0x2)
                        g_processor.regs[inst.rd] = (word >> 16) & 0xFFFF;  // Zero extend
                    else
                        g_processor.regs[inst.rd] = word & 0xFFFF;  // Zero extend
                    break;
                }
            }
            break;
            
        // I-type immediate arithmetic
        case 0b0010011:
            switch (inst.func3) {
                case 0b000:  // addi
                    g_processor.regs[inst.rd] = g_processor.regs[inst.rs1] + inst.imm;
                    break;
                case 0b010:  // slti
                    g_processor.regs[inst.rd] = ((int32_t)g_processor.regs[inst.rs1] < (int32_t)inst.imm) ? 1 : 0;
                    break;
                case 0b011:  // sltiu
                    g_processor.regs[inst.rd] = (g_processor.regs[inst.rs1] < (uint32_t)inst.imm) ? 1 : 0;
                    break;
                case 0b100:  // xori
                    g_processor.regs[inst.rd] = g_processor.regs[inst.rs1] ^ inst.imm;
                    break;
                case 0b110:  // ori
                    g_processor.regs[inst.rd] = g_processor.regs[inst.rs1] | inst.imm;
                    break;
                case 0b111:  // andi
                    g_processor.regs[inst.rd] = g_processor.regs[inst.rs1] & inst.imm;
                    break;
                case 0b001:  // slli
                    g_processor.regs[inst.rd] = g_processor.regs[inst.rs1] << (inst.imm & 0x1F);
                    break;
                case 0b101:  // srli, srai
                    if ((inst.imm >> 5) == 0)  // srli
                        g_processor.regs[inst.rd] = g_processor.regs[inst.rs1] >> (inst.imm & 0x1F);
                    else  // srai
                        g_processor.regs[inst.rd] = (int32_t)g_processor.regs[inst.rs1] >> (inst.imm & 0x1F);
                    break;
            }
            break;
            
        // S-type instructions
        case 0b0100011:
            switch (inst.func3) {
                case 0b000:  // sb
                {
                    uint32_t addr = g_processor.regs[inst.rs1] + inst.imm;
                    uint32_t word = mem_read_32(addr & ~0x3);
                    uint32_t byte_offset = addr & 0x3;
                    uint32_t shift = byte_offset * 8;
                    uint32_t mask = ~(0xFF << shift);
                    word = (word & mask) | ((g_processor.regs[inst.rs2] & 0xFF) << shift);
                    mem_write_32(addr & ~0x3, word);
                    break;
                }
                case 0b001:  // sh
                {
                    uint32_t addr = g_processor.regs[inst.rs1] + inst.imm;
                    uint32_t word = mem_read_32(addr & ~0x3);
                    if (addr & 0x2) {  // Upper halfword
                        word = (word & 0xFFFF) | ((g_processor.regs[inst.rs2] & 0xFFFF) << 16);
                    } else {  // Lower halfword
                        word = (word & 0xFFFF0000) | (g_processor.regs[inst.rs2] & 0xFFFF);
                    }
                    mem_write_32(addr & ~0x3, word);
                    break;
                }
                case 0b010:  // sw
                {
                    uint32_t addr = g_processor.regs[inst.rs1] + inst.imm;
                    mem_write_32(addr, g_processor.regs[inst.rs2]);
                    break;
                }
            }
            break;
            
        // B-type instructions
        case 0b1100011:
            switch (inst.func3) {
                case 0b000:  // beq
                    if (g_processor.regs[inst.rs1] == g_processor.regs[inst.rs2]) {
                        next_pc = g_processor.pc + inst.imm;
                    }
                    break;
                case 0b001:  // bne
                    if (g_processor.regs[inst.rs1] != g_processor.regs[inst.rs2]) {
                        next_pc = g_processor.pc + inst.imm;
                    }
                    break;
                case 0b100:  // blt
                    if ((int32_t)g_processor.regs[inst.rs1] < (int32_t)g_processor.regs[inst.rs2]) {
                        next_pc = g_processor.pc + inst.imm;
                    }
                    break;
                case 0b101:  // bge
                    if ((int32_t)g_processor.regs[inst.rs1] >= (int32_t)g_processor.regs[inst.rs2]) {
                        next_pc = g_processor.pc + inst.imm;
                    }
                    break;
                case 0b110:  // bltu
                    if (g_processor.regs[inst.rs1] < g_processor.regs[inst.rs2]) {
                        next_pc = g_processor.pc + inst.imm;
                    }
                    break;
                case 0b111:  // bgeu
                    if (g_processor.regs[inst.rs1] >= g_processor.regs[inst.rs2]) {
                        next_pc = g_processor.pc + inst.imm;
                    }
                    break;
            }
            break;
            
        // U-type instructions
        case 0b0110111:  // lui
            g_processor.regs[inst.rd] = inst.imm;
            break;
        case 0b0010111:  // auipc
            g_processor.regs[inst.rd] = g_processor.pc + inst.imm;
            break;
            
        // J-type instruction
        case 0b1101111:  // jal
            g_processor.regs[inst.rd] = g_processor.pc + 4;  // Store return address
            next_pc = g_processor.pc + inst.imm;
            break;
            
        // I-type variant for jalr
        case 0b1100111:  // jalr
            if (inst.func3 == 0) {
                uint32_t temp = g_processor.pc + 4;  // Store return address
                next_pc = (g_processor.regs[inst.rs1] + inst.imm) & ~1;  // Clear least significant bit
                g_processor.regs[inst.rd] = temp;
            }
            break;
    }
    
    // Update PC for branch and jump instructions
    if (next_pc != g_processor.pc + 4) {
        g_processor.pc = next_pc;
        g_processor.pc_hold = TRUE;
    }

    // Check if PC is outside valid text segment
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
    int inst_reg;
    struct inst_t inst;

    // 1. fetch
    inst_reg = fetch(g_processor.pc);

    // 2. decode
    inst = decode(inst_reg);

    // 3. execute
    execute(inst);

    // 4. update stats and PC
    if (!g_processor.pc_hold) {
        // Only increment PC if no jump/branch was taken
        g_processor.pc += BYTES_PER_WORD;
    } else {
        // Reset the PC hold flag for next instruction
        g_processor.pc_hold = FALSE;
    }
    
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
