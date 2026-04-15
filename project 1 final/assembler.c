/***************************************************************/
/*                                                             */
/*   RISC-V (RV32I) assembler                                  */
/*                                                             */
/*   KECE343/COMM307 Korea University                          */
/*   assembler.c                                               */
/*                                                             */
/***************************************************************/

#include "assembler.h"
#include "util.h"

/******************************************************
 * Global Variable Declaration
 *******************************************************/

inst_t inst_list[INST_LIST_LEN] = {                   //  idx
    {"add",   "0110011", TYPE_R,  "000", "0000000"},  //   0
    {"sub",   "0110011", TYPE_R,  "000", "0100000"},  //   1
    {"addi",  "0010011", TYPE_I,  "000", ""},         //   2
    {"and",   "0110011", TYPE_R,  "111", "0000000"},  //   3
    {"andi",  "0010011", TYPE_I,  "111", ""},         //   4
    {"beq",   "1100011", TYPE_SB, "000", ""},         //   5
    {"bne",   "1100011", TYPE_SB, "001", ""},         //   6
    {"jal",   "1101111", TYPE_UJ, "",    ""},         //   7
    {"jalr",  "1100111", TYPE_I,  "000", ""},         //   8
    {"auipc", "0010111", TYPE_U,  "",    ""},         //   9
    {"lui",   "0110111", TYPE_U,  "",    ""},         //   10
    {"lw",    "0000011", TYPE_I,  "010", ""},         //   11
    {"xori",  "0010011", TYPE_I,  "100", ""},         //   12
    {"or",    "0110011", TYPE_R,  "110", "0000000"},  //   13
    {"ori",   "0010011", TYPE_I,  "110", ""},         //   14
    {"sltiu", "0010011", TYPE_I,  "011", ""},         //   15
    {"sltu",  "0110011", TYPE_R,  "011", "0000000"},  //   16
    {"slli",  "0010011", TYPE_I,  "001", "0000000"},  //   17
    {"srli",  "0010011", TYPE_I,  "101", "0000000"},  //   18
    {"sw",    "0100011", TYPE_S,  "010", ""},         //   19
};

symbol_t SYMBOL_TABLE[MAX_SYMBOL_TABLE_SIZE]; // Global Symbol Table

uint32_t symbol_table_cur_index = 0; // For indexing of symbol table

/* Temporary file stream pointers */
FILE *data_seg;
FILE *text_seg;

/* Size of each section */
uint32_t data_section_size = 0;
uint32_t text_section_size = 0;

/******************************************************
 * Function Declaration
 *******************************************************/

/* Add symbol to global symbol table */
void symbol_table_add_entry(symbol_t symbol)
{
    SYMBOL_TABLE[symbol_table_cur_index++] = symbol;
#if DEBUG
    printf("%s: 0x%08x\n", symbol.name, symbol.address);
#endif
}

/* Record .text section to output file */
void record_text_section(FILE *output)
{
       uint32_t cur_addr = MEM_TEXT_START;
    char line[1024];

    /* Point to text_seg stream */
    rewind(text_seg);

    /* Print .text section */
    while (fgets(line, 1024, text_seg) != NULL) {
        char inst[0x1000] = {0};
        char *temp, *imm_bits;
        int i, idx = 0;
        inst_type_t type = TYPE_UNKNOWN;
        char op[32] = {0};
        char label[32] = {0};
        int addr;
        int rs2, rs1, rd, imm;

        rs2 = rs1 = rd = imm = 0;

#if DEBUG
        printf("0x%08x: ", cur_addr);
#endif
        /* Find the instruction type that matches the line */

        if (strchr(line, ':') != NULL) {
            continue;
        }
        
        temp = strtok(line, "\t\n ,");
        if (temp == NULL) continue; 
	if (strcmp(temp, "la") == 0) {
    rd = atoi(strtok(NULL, "\t\n ,x"));
    char *la_label = strtok(NULL, "\t\n ,");
    uint32_t label_addr = 0;
    for (i = 0; i < symbol_table_cur_index; i++) {
        if (strcmp(SYMBOL_TABLE[i].name, la_label) == 0) {
            label_addr = SYMBOL_TABLE[i].address;
            break;
        }
    }
    
    int32_t offset = label_addr - cur_addr;

    uint32_t upper_imm = (offset >> 12) & 0xFFFFF;
    uint32_t lower_imm = offset & 0xFFF;

    char *auipc_bits = num_to_bits((upper_imm << 12) | (rd << 7) | 0b0010111, 32);
    fprintf(output, "%s\n", auipc_bits);
    free(auipc_bits);
    
    char *addi_bits = num_to_bits((lower_imm << 20) | (rd << 15) | (rd << 7) | 0b0010011, 32);
    fprintf(output, "%s\n", addi_bits);
    free(addi_bits);
    
    cur_addr += BYTES_PER_WORD * 2; 
    continue;
}
        
        for (i = 0; i < INST_LIST_LEN; i++) {
            if (strcmp(temp, inst_list[i].name) == 0) {
                strcpy(op, inst_list[i].op);
                type = inst_list[i].type;
                idx = i;
                break;
            }
        }
        
        if (type == TYPE_UNKNOWN) continue;

        switch (type) {
            case TYPE_R:
                rd = atoi(strtok(NULL, "\t\n ,x"));
                rs1 = atoi(strtok(NULL, "\t\n ,x"));
                rs2 = atoi(strtok(NULL, "\t\n ,x"));
            
                char *r_bits = num_to_bits(
                    (strcmp(inst_list[idx].funct7, "0100000") == 0 ? 0x20 : 0) << 25 |
                    (rs2 & 0x1F) << 20 |
                    (rs1 & 0x1F) << 15 |
                    (strtol(inst_list[idx].funct3, NULL, 2) & 0x7) << 12 |
                    (rd & 0x1F) << 7 |
                    strtol(inst_list[idx].op, NULL, 2), 32);
                
                fprintf(output, "%s\n", r_bits);
                free(r_bits);
#if DEBUG
                printf("op:%s rs1:x%d rs2:x%d rd:x%d funct3:%s funct7:%s\n",
                        op, rs1, rs2, rd, inst_list[idx].funct3, inst_list[idx].funct7);
#endif
                break;

            case TYPE_I:
    rd = atoi(strtok(NULL, "\t\n ,x"));

    if (strcmp(inst_list[idx].name, "lw") == 0 || strcmp(inst_list[idx].name, "jalr") == 0) {
        temp = strtok(NULL, "\t\n ,(");
        if (temp != NULL) {
            if (strncmp(temp, "0x", 2) == 0) {
                imm = strtol(temp, NULL, 16);
            } else {
                imm = atoi(temp);
            }
        }
        

        rs1 = atoi(strtok(NULL, "\t\n ,x)"));
    } else {
        rs1 = atoi(strtok(NULL, "\t\n ,x"));
        temp = strtok(NULL, "\t\n ,");
        if (temp != NULL) {
            if (strncmp(temp, "0x", 2) == 0) {
                imm = strtol(temp, NULL, 16);
            } else {
                imm = atoi(temp);
            }
        }
    }
    char *i_bits = num_to_bits(
        (imm & 0xFFF) << 20 |
        (rs1 & 0x1F) << 15 |
        (strtol(inst_list[idx].funct3, NULL, 2) & 0x7) << 12 |
        (rd & 0x1F) << 7 |
        strtol(inst_list[idx].op, NULL, 2), 32);
    
    fprintf(output, "%s\n", i_bits);
    free(i_bits);                
#if DEBUG
                printf("op:%s rs1:x%d rd:x%d imm:0x%x\n",
                        op, rs1, rd, imm);
#endif
                break;

            case TYPE_S:
                /* blank */
                rs2 = atoi(strtok(NULL, "\t\n ,x"));
 
                temp = strtok(NULL, "\t\n ,(");
                if (temp != NULL) {
                    if (strncmp(temp, "0x", 2) == 0) {
                        imm = strtol(temp, NULL, 16);
                    } else {
                        imm = atoi(temp);
                    }
                }
                
                rs1 = atoi(strtok(NULL, "\t\n ,x)"));
                
                char *s_bits = num_to_bits(
                    ((imm >> 5) & 0x7F) << 25 |
                    (rs2 & 0x1F) << 20 |
                    (rs1 & 0x1F) << 15 |
                    (strtol(inst_list[idx].funct3, NULL, 2) & 0x7) << 12 |
                    (imm & 0x1F) << 7 |
                    strtol(inst_list[idx].op, NULL, 2), 32);
                
                fprintf(output, "%s\n", s_bits);
                free(s_bits);
#if DEBUG
                printf("op:%s rs1:x%d rs2:x%d imm:0x%x\n",
                        op, rs1, rs2, imm);
#endif
                break;

            case TYPE_SB:
                /* blank */
                rs1 = atoi(strtok(NULL, "\t\n ,x"));
                rs2 = atoi(strtok(NULL, "\t\n ,x"));
                
                
                temp = strtok(NULL, "\t\n ,");
                if (temp != NULL) {
                    strcpy(label, temp);
                }
                
                uint32_t label_addr = 0;
                for (i = 0; i < symbol_table_cur_index; i++) {
                    if (strcmp(SYMBOL_TABLE[i].name, label) == 0) {
                        label_addr = SYMBOL_TABLE[i].address;
                        break;
                    }
                }
                
                imm = label_addr - cur_addr;
                
                
                int32_t imm_sb = imm;
                uint32_t imm_sb_bits = 
                    (((imm_sb >> 12) & 0x1) << 31) |  
                    (((imm_sb >> 5) & 0x3F) << 25) |  
                    (((imm_sb >> 1) & 0xF) << 8) |    
                    (((imm_sb >> 11) & 0x1) << 7); 
                
                char *sb_bits = num_to_bits(
                    imm_sb_bits |
                    (rs2 & 0x1F) << 20 |
                    (rs1 & 0x1F) << 15 |
                    (strtol(inst_list[idx].funct3, NULL, 2) & 0x7) << 12 |
                    strtol(inst_list[idx].op, NULL, 2), 32);
                
                fprintf(output, "%s\n", sb_bits);
                free(sb_bits);
#if DEBUG
                printf("op:%s rs1:x%d rs2:x%d imm:0x%x\n",
                        op, rs1, rs2, imm);
#endif
                break;
                
            case TYPE_U:
                /* blank */
                rd = atoi(strtok(NULL, "\t\n ,x"));
                
                temp = strtok(NULL, "\t\n ,");
                if (temp != NULL) {
                    if (strncmp(temp, "0x", 2) == 0) {
                        imm = strtol(temp, NULL, 16);
                    } else {
                        imm = atoi(temp);
                    }
                }
                
                char *u_bits = num_to_bits(
                    (imm & 0xFFFFF) << 12 |
                    (rd & 0x1F) << 7 |
                    strtol(inst_list[idx].op, NULL, 2), 32);
                
                fprintf(output, "%s\n", u_bits);
                free(u_bits);
#if DEBUG
                printf("op:%s rd:x%d imm:0x%x\n",
                        op, rd, imm);
#endif
                break;

            case TYPE_UJ:
                /* blank */
                rd = atoi(strtok(NULL, "\t\n ,x"));
                
                temp = strtok(NULL, "\t\n ,");
                if (temp != NULL) {
                    strcpy(label, temp);
                }
                label_addr = 0;
                for (i = 0; i < symbol_table_cur_index; i++) {
                    if (strcmp(SYMBOL_TABLE[i].name, label) == 0) {
                        label_addr = SYMBOL_TABLE[i].address;
                        break;
                    }
                }
                
                imm = label_addr - cur_addr;
                
                int32_t imm_uj = imm;
                uint32_t imm_uj_bits = 
                    (((imm_uj >> 20) & 0x1) << 31) |  
                    (((imm_uj >> 1) & 0x3FF) << 21) | 
                    (((imm_uj >> 11) & 0x1) << 20) |  
                    (((imm_uj >> 12) & 0xFF) << 12);  
                
                char *uj_bits = num_to_bits(
                    imm_uj_bits |
                    (rd & 0x1F) << 7 |
                    strtol(inst_list[idx].op, NULL, 2), 32);
                
                fprintf(output, "%s\n", uj_bits);
                free(uj_bits);
#if DEBUG
                printf("op:%s rd:x%d imm:0x%x\n",
                        op, rd, imm);
#endif
                break;

            default:
                break;
        }

        cur_addr += BYTES_PER_WORD;
    }
}

/* Record .data section to output file */
void record_data_section(FILE *output)
{
   uint32_t cur_addr = MEM_DATA_START;
    char line[1024];

    /* Point to data segment stream */
    rewind(data_seg);

    /* Print .data section */
    while (fgets(line, 1024, data_seg) != NULL) {
        /* blank */
        char *temp, *value_str;
        int value = 0;
      
        if (strchr(line, ':') != NULL) {
            temp = strtok(line, ":");
            temp = strtok(NULL, " \t\n");
            
            if (temp == NULL || strcmp(temp, ".word") != 0) {
                continue;
            }
            
    
            value_str = strtok(NULL, " \t\n");
        } else {
            temp = strtok(line, " \t\n");
            if (temp == NULL || strcmp(temp, ".word") != 0) {
                continue;
            }
            
            value_str = strtok(NULL, " \t\n");
        }
        
        if (value_str != NULL) {
            if (strncmp(value_str, "0x", 2) == 0) {
                value = strtol(value_str, NULL, 16);
            } else {
                value = atoi(value_str);
            }
        }
        
        char *data_bits = num_to_bits(value, 32);
        fprintf(output, "%s\n", data_bits);
        free(data_bits);
        
#if DEBUG
        printf("0x%08x: ", cur_addr);
        printf("%s", line);
#endif
        cur_addr += BYTES_PER_WORD;
    }
}

void make_binary_file(FILE *output) 
{
#if DEBUG
    char line[1024] = {0};
    rewind(text_seg);
    /* Print line of text segment */
    while (fgets(line, 1024, text_seg) != NULL) {
        printf("%s",line);
    }
    printf("\ntext size: %d, data size: %d\n", text_section_size, data_section_size);
#endif

    /* Print text section size and data section size */
    /* blank */
    fprintf(output, "%032s\n", num_to_bits(text_section_size, 32));
    fprintf(output, "%032s\n", num_to_bits(data_section_size, 32));

    /* Print .text section */
    record_text_section(output);

    /* Print .data section */
    record_data_section(output);
}

/* Fill the blanks */
void make_symbol_table(FILE *input)
{
    char line[1024] = {0};
    uint32_t *section_size = NULL;
    uint32_t address = 0;

    enum section cur_section = MAX_SIZE;

    /* Read each section and put the stream */
    while (fgets(line, 1024, input) != NULL) {
        char *temp;
        char _line[1024] = {0};
        strcpy(_line, line);
        temp = strtok(_line, "\t\n");

        /* Check section type */
        if (!strcmp(temp, ".data")) {
            /* blank */
            cur_section = DATA;
            section_size = &data_section_size;
            address = MEM_DATA_START;
            data_seg = tmpfile();
            continue;
        }
        else if (!strcmp(temp, ".text")) {
            /* blank */
            cur_section = TEXT;
            section_size = &text_section_size;
            address = MEM_TEXT_START;
            text_seg = tmpfile();
            continue;
        }

        /* Put the line into each segment stream */
        if (cur_section == DATA) {
            /* blank */
            fputs(line, data_seg);
           
            char *label_end = strchr(temp, ':');
            if (label_end != NULL) {
                *label_end = '\0';
            
                symbol_t symbol;
                strcpy(symbol.name, temp);
                symbol.address = address;
                symbol_table_add_entry(symbol);
            }
            
            address += BYTES_PER_WORD;
            *section_size += BYTES_PER_WORD;
        }
    
	else if (cur_section == TEXT) {
    fputs(line, text_seg);
    
    char *label_end = strchr(temp, ':');
    if (label_end != NULL) {
        *label_end = '\0';
        
        symbol_t symbol;
        strcpy(symbol.name, temp);
        symbol.address = address;
        symbol_table_add_entry(symbol);
    } else {
        if (temp && strcmp(temp, "la") == 0) {
            address += BYTES_PER_WORD;
            *section_size += BYTES_PER_WORD;
            
            char *reg_str = strtok(NULL, "\t\n ,x");
            char *label_name = strtok(NULL, "\t\n ,");
            
            if (label_name) {
                address += BYTES_PER_WORD;
                *section_size += BYTES_PER_WORD;
            }
        } else {
            address += BYTES_PER_WORD;
            *section_size += BYTES_PER_WORD;
        }
    }
}
    }
}
