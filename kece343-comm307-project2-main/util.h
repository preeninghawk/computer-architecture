/***************************************************************/
/*                                                             */
/*   RISC-V (RV32I) Instruction Level Simulator                */
/*                                                             */
/*   KECE343/COMM307 Korea University                          */
/*   util.h                                                    */
/*                                                             */
/***************************************************************/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*          DO NOT MODIFY THIS FILE!                            */
/*          You should only change proc.c, loader.c file!       */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define FALSE 0
#define TRUE  1

/* Functions */
char**		str_split(char *a_str, const char a_delim);
int		    str_to_int(const char *s);
int32_t		str_to_signed_int(const char *s);
char*       dec_to_bin(int n);

#endif
