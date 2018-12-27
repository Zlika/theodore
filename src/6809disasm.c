/*
 * This file is part of theodore, a Thomson emulator based on
 * Daniel Coulom's DCTO8D emulator (http://dcto8.free.fr/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* Motorola 6809 disassembler */

#include <stdio.h>
#include <string.h>

#include "6809cpu.h"

#define GETBYTE i=Mgetc(pc)&0x00ff;pc+=1;sprintf(w,"%02X",i);strcat(hexa,w)
#define GETWORD i=Mgetw(pc)&0xffff;pc+=2;sprintf(w,"%04X",i);strcat(hexa,w)

static int n;          // number of cpu cycles
static int pc;         // disassembling address
static char w[10];     // work area
static char hexa[20];  // address and hexa dump
static char param[20]; // parameters

// instruction types
#define INV   0x0000 //invalide
#define INH   0x0100 //inherente
#define DIR   0x0200 //directe
#define IMM1  0x0300 //immediate 1 octet
#define IMM2  0x0400 //immediate 2 octets
#define REL1  0x0500 //relative 1 octet
#define REL2  0x0600 //relative 2 octets
#define IND   0x0700 //indexee
#define EXT   0x0800 //etendue
#define REG0  0x0a00 //EXG/TFR
#define REG1  0x0b00 //PSHS
#define REG2  0x0c00 //PULS
#define REG3  0x0d00 //PSHU
#define REG4  0x0e00 //PULU

//indexation des mnemoniques
/* 8-Bit Accumulator and Memory Instructions */
#define ADCA  0x010000
#define ADCB  0x020000
#define ADDA  0x030000
#define ADDB  0x040000
#define ANDA  0x050000
#define ANDB  0x060000
#define ASL   0x070000
#define ASLA  0x080000
#define ASLB  0x090000
#define ASR   0x0a0000
#define ASRA  0x0b0000
#define ASRB  0x0c0000
#define BITA  0x0d0000
#define BITB  0x0e0000
#define CLR   0x0f0000
#define CLRA  0x100000
#define CLRB  0x110000
#define CMPA  0x120000
#define CMPB  0x130000
#define COM   0x140000
#define COMA  0x150000
#define COMB  0x160000
#define DAA   0x170000
#define DEC   0x180000
#define DECA  0x190000
#define DECB  0x1a0000
#define EORA  0x1b0000
#define EORB  0x1c0000
#define INC   0x1d0000
#define INCA  0x1e0000
#define INCB  0x1f0000
#define LDA   0x200000
#define LDB   0x210000
#define LSL   0x220000
#define LSLA  0x230000
#define LSLB  0x240000
#define LSR   0x250000
#define LSRA  0x260000
#define LSRB  0x270000
#define MUL   0x280000
#define NEG   0x290000
#define NEGA  0x2a0000
#define NEGB  0x2b0000
#define ORA   0x2c0000
#define ORB   0x2d0000
#define ROL   0x2e0000
#define ROLA  0x2f0000
#define ROLB  0x300000
#define ROR   0x310000
#define RORA  0x320000
#define RORB  0x330000
#define SBCA  0x340000
#define SBCB  0x350000
#define STA   0x360000
#define STB   0x370000
#define SUBA  0x380000
#define SUBB  0x390000
#define TST   0x3a0000
#define TSTA  0x3b0000
#define TSTB  0x3c0000
/* 16-Bit Accumulator and Memory Instructions */
#define ADDD  0x400000
#define CMPD  0x410000
#define LDD   0x420000
#define SEX   0x430000
#define STD   0x440000
#define SUBD  0x450000
/* Index/Stack Pointer Instructions */
#define CMPS  0x480000
#define CMPU  0x490000
#define CMPX  0x4a0000
#define CMPY  0x4b0000
#define EXG   0x4c0000
#define LEAS  0x4d0000
#define LEAU  0x4e0000
#define LEAX  0x4f0000
#define LEAY  0x500000
#define LDS   0x510000
#define LDU   0x520000
#define LDX   0x530000
#define LDY   0x540000
#define PSHS  0x550000
#define PSHU  0x560000
#define PULS  0x570000
#define PULU  0x580000
#define STS   0x590000
#define STU   0x5a0000
#define STX   0x5b0000
#define STY   0x5c0000
#define TFR   0x5d0000
#define ABX   0x5e0000
/* Branch Instructions */
#define BEQ   0x600000
#define LBEQ  0x610000
#define BNE   0x620000
#define LBNE  0x630000
#define BMI   0x640000
#define LBMI  0x650000
#define BPL   0x660000
#define LBPL  0x670000
#define BCS   0x680000
#define LBCS  0x690000
#define BCC   0x6a0000
#define LBCC  0x6b0000
#define BVS   0x6c0000
#define LBVS  0x6d0000
#define BVC   0x6e0000
#define LBVC  0x6f0000
#define BGT   0x700000
#define LBGT  0x710000
#define BGE   0x720000
#define LBGE  0x730000
#define BLE   0x740000
#define LBLE  0x750000
#define BLT   0x760000
#define LBLT  0x770000
#define BHI   0x780000
#define LBHI  0x790000
#define BHS   0x7a0000
#define LBHS  0x7b0000
#define BLS   0x7c0000
#define LBLS  0x7d0000
#define BLO   0x7e0000
#define LBLO  0x7f0000
#define BSR   0x800000
#define LBSR  0x810000
#define BRA   0x820000
#define LBRA  0x830000
#define BRN   0x840000
#define LBRN  0x850000
/* Miscellaneous Instructions */
#define ANDCC 0x880000
#define CWAI  0x890000
#define NOP   0x8a0000
#define ORCC  0x8b0000
#define JMP   0x8c0000
#define JSR   0x8d0000
#define RTI   0x8e0000
#define RTS   0x8f0000
#define SWI   0x900000
#define SWI2  0x910000
#define SWI3  0x920000
#define SYNC  0x930000

static char mnemonique[152][6] =
{
    /* *********** 8-Bit Accumulator and Memory Instructions ********** */
    /*00*/ "???",  "ADCA", "ADCB", "ADDA", "ADDB", "ANDA", "ANDB", "ASL",
    /*08*/ "ASLA", "ASLB", "ASR",  "ASRA", "ASRB", "BITA", "BITB", "CLR",
    /*10*/ "CLRA", "CLRB", "CMPA", "CMPB", "COM",  "COMA", "COMB", "DAA",
    /*18*/ "DEC",  "DECA", "DECB", "EORA", "EORB", "INC",  "INCA", "INCB",
    /*20*/ "LDA",  "LDB",  "LSL",  "LSLA", "LSLB", "LSR",  "LSRA", "LSRB",
    /*28*/ "MUL",  "NEG",  "NEGA", "NEGB", "ORA",  "ORB",  "ROL",  "ROLA",
    /*30*/ "ROLB", "ROR",  "RORA", "RORB", "SBCA", "SBCB", "STA",  "STB",
    /*38*/ "SUBA", "SUBB", "TST",  "TSTA", "TSTB", "",     "",     "",
    /* *********** 16-Bit Accumulator and Memory Instructions ********** */
    /*40*/ "ADDD", "CMPD", "LDD",  "SEX",  "STD",  "SUBD", "",     "",
    /* *********** Index/Stack Pointer Instructions ******************** */
    /*48*/ "CMPS", "CMPU", "CMPX", "CMPY", "EXG",  "LEAS", "LEAU", "LEAX",
    /*50*/ "LEAY", "LDS",  "LDU",  "LDX",  "LDY",  "PSHS", "PSHU", "PULS",
    /*58*/ "PULU", "STS",  "STU",  "STX",  "STY",  "TFR",  "ABX",  "",
    /* *********** Branch Instructions ********************************* */
    /*60*/ "BEQ",  "LBEQ", "BNE",  "LBNE", "BMI",  "LBMI", "BPL",  "LBPL",
    /*68*/ "BCS",  "LBCS", "BCC",  "LBCC", "BVS",  "LBVS", "BVC",  "LBVC",
    /*70*/ "BGT",  "LBGT", "BGE",  "LBGE", "BLE",  "LBLE", "BLT",  "LBLT",
    /*78*/ "BHI",  "LBHI", "BHS",  "LBHS", "BLS",  "LBLS", "BLO",  "LBLO",
    /*80*/ "BSR",  "LBSR", "BRA",  "LBRA", "BRN",  "LBRN", "",     "",
    /* *********** Miscellaneous Instructions ************************** */
    /*88*/ "ANDCC", "CWAI", "NOP", "ORCC", "JMP",  "JSR",  "RTI",  "RTS",
    /*90*/ "SWI",  "SWI2", "SWI3", "SYNC", "",     "",     "",     "",
};

//definition des instructions sour forme mmttnn hexadecimal avec :
//- mmOOOO = index mnemonique
//-   tt00 = type d'instruction
//-     nn = nombre de cycles
static int instruction[0x300] =
{
    /*0000*/ NEG   | DIR  |  6,
    /*0001*/ BRN   | REL1 |  3,
    /*0002*/ INV,
    /*0003*/ COM   | DIR  |  6,
    /*0004*/ LSR   | DIR  |  6,
    /*0005*/ INV,
    /*0006*/ ROR   | DIR  |  6,
    /*0007*/ ASR   | DIR  |  6,
    /*0008*/ ASL   | DIR  |  6,
    /*0009*/ ROL   | DIR  |  6,
    /*000a*/ DEC   | DIR  |  6,
    /*000b*/ INV,
    /*000c*/ INC   | DIR  |  6,
    /*000d*/ TST   | DIR  |  6,
    /*000e*/ JMP   | DIR  |  3,
    /*000f*/ CLR   | DIR  |  6,

    /*0010*/ INV,
    /*0011*/ INV,
    /*0012*/ NOP   | INH  |  2,
    /*0013*/ SYNC  | INH  |  4,
    /*0014*/ INV,
    /*0015*/ INV,
    /*0016*/ LBRA  | REL2 |  5,
    /*0017*/ LBSR  | REL2 |  9,
    /*0018*/ INV,
    /*0019*/ DAA   | INH  |  2,
    /*001a*/ ORCC  | IMM1 |  3,
    /*001b*/ INV,
    /*001c*/ ANDCC | IMM1 |  3,
    /*001d*/ SEX   | INH  |  2,
    /*001e*/ EXG   | REG0 |  8,
    /*001f*/ TFR   | REG0 |  6,

    /*0020*/ BRA   | REL1 |  3,
    /*0021*/ BRN   | REL1 |  3,
    /*0022*/ BHI   | REL1 |  3,
    /*0023*/ BLS   | REL1 |  3,
    /*0024*/ BCC   | REL1 |  3,
    /*0025*/ BCS   | REL1 |  3,
    /*0026*/ BNE   | REL1 |  3,
    /*0027*/ BEQ   | REL1 |  3,
    /*0028*/ BVC   | REL1 |  3,
    /*0029*/ BVS   | REL1 |  3,
    /*002a*/ BPL   | REL1 |  3,
    /*002b*/ BMI   | REL1 |  3,
    /*002c*/ BGE   | REL1 |  3,
    /*002d*/ BLT   | REL1 |  3,
    /*002e*/ BGT   | REL1 |  3,
    /*002f*/ BLE   | REL1 |  3,

    /*0030*/ LEAX  | IND  |  4,
    /*0031*/ LEAY  | IND  |  4,
    /*0032*/ LEAS  | IND  |  4,
    /*0033*/ LEAU  | IND  |  4,
    /*0034*/ PSHS  | REG1 |  5,
    /*0035*/ PULS  | REG2 |  5,
    /*0036*/ PSHU  | REG3 |  5,
    /*0037*/ PULU  | REG4 |  5,
    /*0038*/ INV,
    /*0039*/ RTS   | INH  |  5,
    /*003a*/ ABX   | INH  |  3,
    /*003b*/ RTI   | INH  |164,
    /*003c*/ CWAI  | INH  | 20,
    /*003d*/ MUL   | INH  | 11,
    /*003e*/ INV,
    /*003f*/ SWI   | IMM1 | 19,

    /*0040*/ NEGA  | INH  |  2,
    /*0041*/ INV,
    /*0042*/ INV,
    /*0043*/ COMA  | INH  |  2,
    /*0044*/ LSRA  | INH  |  2,
    /*0045*/ INV,
    /*0046*/ RORA  | INH  |  2,
    /*0047*/ ASRA  | INH  |  2,
    /*0048*/ ASLA  | INH  |  2,
    /*0049*/ ROLA  | INH  |  2,
    /*004a*/ DECA  | INH  |  2,
    /*004b*/ INV,
    /*004c*/ INCA  | INH  |  2,
    /*004d*/ TSTA  | INH  |  2,
    /*004e*/ INV,
    /*004f*/ CLRA  | INH  |  2,

    /*0050*/ NEGB  | INH  |  2,
    /*0051*/ INV,
    /*0052*/ INV,
    /*0053*/ COMB  | INH  |  2,
    /*0054*/ LSRB  | INH  |  2,
    /*0055*/ INV,
    /*0056*/ RORB  | INH  |  2,
    /*0057*/ ASRB  | INH  |  2,
    /*0058*/ ASLB  | INH  |  2,
    /*0059*/ ROLB  | INH  |  2,
    /*005a*/ DECB  | INH  |  2,
    /*005b*/ INV,
    /*005c*/ INCB  | INH  |  2,
    /*005d*/ TSTB  | INH  |  2,
    /*005e*/ INV,
    /*005f*/ CLRB  | INH  |  2,

    /*0060*/ NEG   | IND  |  6,
    /*0061*/ INV,
    /*0062*/ INV,
    /*0063*/ COM   | IND  |  6,
    /*0064*/ LSR   | IND  |  6,
    /*0065*/ INV,
    /*0066*/ ROR   | IND  |  6,
    /*0067*/ ASR   | IND  |  6,
    /*0068*/ ASL   | IND  |  6,
    /*0069*/ ROL   | IND  |  6,
    /*006a*/ DEC   | IND  |  6,
    /*006b*/ INV,
    /*006c*/ INC   | IND  |  6,
    /*006d*/ TST   | IND  |  6,
    /*006e*/ JMP   | IND  |  3,
    /*006f*/ CLR   | IND  |  6,

    /*0070*/ NEG   | EXT  |  7,
    /*0071*/ INV,
    /*0072*/ INV,
    /*0073*/ COM   | EXT  |  7,
    /*0074*/ LSR   | EXT  |  7,
    /*0075*/ INV,
    /*0076*/ ROR   | EXT  |  7,
    /*0077*/ ASR   | EXT  |  7,
    /*0078*/ ASL   | EXT  |  7,
    /*0079*/ ROL   | EXT  |  7,
    /*007a*/ DEC   | EXT  |  7,
    /*007b*/ INV,
    /*007c*/ INC   | EXT  |  7,
    /*007d*/ TST   | EXT  |  7,
    /*007e*/ JMP   | EXT  |  4,
    /*007f*/ CLR   | EXT  |  7,

    /*0080*/ SUBA  | IMM1 |  2,
    /*0081*/ CMPA  | IMM1 |  2,
    /*0082*/ SBCA  | IMM1 |  2,
    /*0083*/ SUBD  | IMM2 |  4,
    /*0084*/ ANDA  | IMM1 |  2,
    /*0085*/ BITA  | IMM1 |  2,
    /*0086*/ LDA   | IMM1 |  2,
    /*0087*/ INV,
    /*0088*/ EORA  | IMM1 |  2,
    /*0089*/ ADCA  | IMM1 |  2,
    /*008a*/ ORA   | IMM1 |  2,
    /*008b*/ ADDA  | IMM1 |  2,
    /*008c*/ CMPX  | IMM2 |  4,
    /*008d*/ BSR   | REL1 |  7,
    /*008e*/ LDX   | IMM2 |  3,
    /*008f*/ INV,

    /*0090*/ SUBA  | DIR  |  4,
    /*0091*/ CMPA  | DIR  |  4,
    /*0092*/ SBCA  | DIR  |  4,
    /*0093*/ SUBD  | DIR  |  6,
    /*0094*/ ANDA  | DIR  |  4,
    /*0095*/ BITA  | DIR  |  4,
    /*0096*/ LDA   | DIR  |  4,
    /*0097*/ STA   | DIR  |  4,
    /*0098*/ EORA  | DIR  |  4,
    /*0099*/ ADCA  | DIR  |  4,
    /*009a*/ ORA   | DIR  |  4,
    /*009b*/ ADDA  | DIR  |  4,
    /*009c*/ CMPX  | DIR  |  6,
    /*009d*/ JSR   | DIR  |  7,
    /*009e*/ LDX   | DIR  |  5,
    /*009f*/ STX   | DIR  |  5,

    /*00a0*/ SUBA  | IND  |  4,
    /*00a1*/ CMPA  | IND  |  4,
    /*00a2*/ SBCA  | IND  |  4,
    /*00a3*/ SUBD  | IND  |  6,
    /*00a4*/ ANDA  | IND  |  4,
    /*00a5*/ BITA  | IND  |  4,
    /*00a6*/ LDA   | IND  |  4,
    /*00a7*/ STA   | IND  |  4,
    /*00a8*/ EORA  | IND  |  4,
    /*00a9*/ ADCA  | IND  |  4,
    /*00aa*/ ORA   | IND  |  4,
    /*00ab*/ ADDA  | IND  |  4,
    /*00ac*/ CMPX  | IND  |  4,
    /*00ad*/ JSR   | IND  |  5,
    /*00ae*/ LDX   | IND  |  5,
    /*00af*/ STX   | IND  |  5,

    /*00b0*/ SUBA  | EXT  |  5,
    /*00b1*/ CMPA  | EXT  |  5,
    /*00b2*/ SBCA  | EXT  |  5,
    /*00b3*/ SUBD  | EXT  |  7,
    /*00b4*/ ANDA  | EXT  |  5,
    /*00b5*/ BITA  | EXT  |  5,
    /*00b6*/ LDA   | EXT  |  5,
    /*00b7*/ STA   | EXT  |  5,
    /*00b8*/ EORA  | EXT  |  5,
    /*00b9*/ ADCA  | EXT  |  5,
    /*00ba*/ ORA   | EXT  |  5,
    /*00bb*/ ADDA  | EXT  |  5,
    /*00bc*/ CMPX  | EXT  |  7,
    /*00bd*/ JSR   | EXT  |  8,
    /*00be*/ LDX   | EXT  |  6,
    /*00bf*/ STX   | EXT  |  6,

    /*00c0*/ SUBB  | IMM1 |  2,
    /*00c1*/ CMPB  | IMM1 |  2,
    /*00c2*/ SBCB  | IMM1 |  2,
    /*00c3*/ ADDD  | IMM2 |  4,
    /*00c4*/ ANDB  | IMM1 |  2,
    /*00c5*/ BITB  | IMM1 |  2,
    /*00c6*/ LDB   | IMM1 |  2,
    /*00c7*/ INV,
    /*00c8*/ EORB  | IMM1 |  2,
    /*00c9*/ ADCB  | IMM1 |  2,
    /*00ca*/ ORB   | IMM1 |  2,
    /*00cb*/ ADDB  | IMM1 |  2,
    /*00cc*/ LDD   | IMM2 |  3,
    /*00cd*/ INV,
    /*00ce*/ LDU   | IMM2 |  3,
    /*00cf*/ INV,

    /*00d0*/ SUBB  | DIR  |  4,
    /*00d1*/ CMPB  | DIR  |  4,
    /*00d2*/ SBCB  | DIR  |  4,
    /*00d3*/ ADDD  | DIR  |  6,
    /*00d4*/ ANDB  | DIR  |  4,
    /*00d5*/ BITB  | DIR  |  4,
    /*00d6*/ LDB   | DIR  |  4,
    /*00d7*/ STB   | DIR  |  4,
    /*00d8*/ EORB  | DIR  |  4,
    /*00d9*/ ADCB  | DIR  |  4,
    /*00da*/ ORB   | DIR  |  4,
    /*00db*/ ADDB  | DIR  |  4,
    /*00dc*/ LDD   | DIR  |  5,
    /*00dd*/ STD   | DIR  |  5,
    /*00de*/ LDU   | DIR  |  5,
    /*00df*/ STU   | DIR  |  5,

    /*00e0*/ SUBB  | IND  |  4,
    /*00e1*/ CMPB  | IND  |  4,
    /*00e2*/ SBCB  | IND  |  4,
    /*00e3*/ ADDD  | IND  |  6,
    /*00e4*/ ANDB  | IND  |  4,
    /*00e5*/ BITB  | IND  |  4,
    /*00e6*/ LDB   | IND  |  4,
    /*00e7*/ STB   | IND  |  4,
    /*00e8*/ EORB  | IND  |  4,
    /*00e9*/ ADCB  | IND  |  4,
    /*00ea*/ ORB   | IND  |  4,
    /*00eb*/ ADDB  | IND  |  4,
    /*00ec*/ LDD   | IND  |  5,
    /*00ed*/ STD   | IND  |  5,
    /*00ee*/ LDU   | IND  |  5,
    /*00ef*/ STU   | IND  |  5,

    /*00f0*/ SUBB  | EXT  |  5,
    /*00f1*/ CMPB  | EXT  |  5,
    /*00f2*/ SBCB  | EXT  |  5,
    /*00f3*/ ADDD  | EXT  |  7,
    /*00f4*/ ANDB  | EXT  |  5,
    /*00f5*/ BITB  | EXT  |  5,
    /*00f6*/ LDB   | EXT  |  5,
    /*00f7*/ STB   | EXT  |  5,
    /*00f8*/ EORB  | EXT  |  5,
    /*00f9*/ ADCB  | EXT  |  5,
    /*00fa*/ ORB   | EXT  |  5,
    /*00fb*/ ADDB  | EXT  |  5,
    /*00fc*/ LDD   | EXT  |  6,
    /*00fd*/ STD   | EXT  |  6,
    /*00fe*/ LDU   | EXT  |  6,
    /*00ff*/ STU   | EXT  |  6,

    /*1000*/ INV,
    /*1001*/ INV,
    /*1002*/ INV,
    /*1003*/ INV,
    /*1004*/ INV,
    /*1005*/ INV,
    /*1006*/ INV,
    /*1007*/ INV,
    /*1008*/ INV,
    /*1009*/ INV,
    /*100a*/ INV,
    /*100b*/ INV,
    /*100c*/ INV,
    /*100d*/ INV,
    /*100e*/ INV,
    /*100f*/ INV,

    /*1010*/ INV,
    /*1011*/ INV,
    /*1012*/ INV,
    /*1013*/ INV,
    /*1014*/ INV,
    /*1015*/ INV,
    /*1016*/ INV,
    /*1017*/ INV,
    /*1018*/ INV,
    /*1019*/ INV,
    /*101a*/ INV,
    /*101b*/ INV,
    /*101c*/ INV,
    /*101d*/ INV,
    /*101e*/ INV,
    /*101f*/ INV,

    /*1020*/ INV,
    /*1021*/ LBRN  | REL2 | 65,
    /*1022*/ LBHI  | REL2 | 65,
    /*1023*/ LBLS  | REL2 | 65,
    /*1024*/ LBCC  | REL2 | 65,
    /*1025*/ LBCS  | REL2 | 65,
    /*1026*/ LBNE  | REL2 | 65,
    /*1027*/ LBEQ  | REL2 | 65,
    /*1028*/ LBVC  | REL2 | 65,
    /*1029*/ LBVS  | REL2 | 65,
    /*102a*/ LBPL  | REL2 | 65,
    /*102b*/ LBMI  | REL2 | 65,
    /*102c*/ LBGE  | REL2 | 65,
    /*102d*/ LBLT  | REL2 | 65,
    /*102e*/ LBGT  | REL2 | 65,
    /*102f*/ LBLE  | REL2 | 65,

    /*1030*/ INV,
    /*1031*/ INV,
    /*1032*/ INV,
    /*1033*/ INV,
    /*1034*/ INV,
    /*1035*/ INV,
    /*1036*/ INV,
    /*1037*/ INV,
    /*1038*/ INV,
    /*1039*/ INV,
    /*103a*/ INV,
    /*103b*/ INV,
    /*103c*/ INV,
    /*103d*/ INV,
    /*103e*/ INV,
    /*103f*/ SWI2  | INH  | 20,

    /*1040*/ INV,
    /*1041*/ INV,
    /*1042*/ INV,
    /*1043*/ INV,
    /*1044*/ INV,
    /*1045*/ INV,
    /*1046*/ INV,
    /*1047*/ INV,
    /*1048*/ INV,
    /*1049*/ INV,
    /*104a*/ INV,
    /*104b*/ INV,
    /*104c*/ INV,
    /*104d*/ INV,
    /*104e*/ INV,
    /*104f*/ INV,

    /*1050*/ INV,
    /*1051*/ INV,
    /*1052*/ INV,
    /*1053*/ INV,
    /*1054*/ INV,
    /*1055*/ INV,
    /*1056*/ INV,
    /*1057*/ INV,
    /*1058*/ INV,
    /*1059*/ INV,
    /*105a*/ INV,
    /*105b*/ INV,
    /*105c*/ INV,
    /*105d*/ INV,
    /*105e*/ INV,
    /*105f*/ INV,

    /*1060*/ INV,
    /*1061*/ INV,
    /*1062*/ INV,
    /*1063*/ INV,
    /*1064*/ INV,
    /*1065*/ INV,
    /*1066*/ INV,
    /*1067*/ INV,
    /*1068*/ INV,
    /*1069*/ INV,
    /*106a*/ INV,
    /*106b*/ INV,
    /*106c*/ INV,
    /*106d*/ INV,
    /*106e*/ INV,
    /*106f*/ INV,

    /*1070*/ INV,
    /*1071*/ INV,
    /*1072*/ INV,
    /*1073*/ INV,
    /*1074*/ INV,
    /*1075*/ INV,
    /*1076*/ INV,
    /*1077*/ INV,
    /*1078*/ INV,
    /*1079*/ INV,
    /*107a*/ INV,
    /*107b*/ INV,
    /*107c*/ INV,
    /*107d*/ INV,
    /*107e*/ INV,
    /*107f*/ INV,

    /*1080*/ INV,
    /*1081*/ INV,
    /*1082*/ INV,
    /*1083*/ CMPD  | IMM2 |  5,
    /*1084*/ INV,
    /*1085*/ INV,
    /*1086*/ INV,
    /*1087*/ INV,
    /*1088*/ INV,
    /*1089*/ INV,
    /*108a*/ INV,
    /*108b*/ INV,
    /*108c*/ CMPY  | IMM2 |  5,
    /*108d*/ INV,
    /*108e*/ LDY   | IMM2 |  4,
    /*108f*/ INV,

    /*1090*/ INV,
    /*1091*/ INV,
    /*1092*/ INV,
    /*1093*/ CMPD  | DIR  |  7,
    /*1094*/ INV,
    /*1095*/ INV,
    /*1096*/ INV,
    /*1097*/ INV,
    /*1098*/ INV,
    /*1099*/ INV,
    /*109a*/ INV,
    /*109b*/ INV,
    /*109c*/ CMPY  | DIR  |  7,
    /*109d*/ INV,
    /*109e*/ LDY   | DIR  |  6,
    /*109f*/ STY   | DIR  |  6,

    /*10a0*/ INV,
    /*10a1*/ INV,
    /*10a2*/ INV,
    /*10a3*/ CMPD  | IND  |  7,
    /*10a4*/ INV,
    /*10a5*/ INV,
    /*10a6*/ INV,
    /*10a7*/ INV,
    /*10a8*/ INV,
    /*10a9*/ INV,
    /*10aa*/ INV,
    /*10ab*/ INV,
    /*10ac*/ CMPY  | IND  |  7,
    /*10ad*/ INV,
    /*10ae*/ LDY   | IND  |  6,
    /*10af*/ STY   | IND  |  6,

    /*10b0*/ INV,
    /*10b1*/ INV,
    /*10b2*/ INV,
    /*10b3*/ CMPD  | EXT  |  8,
    /*10b4*/ INV,
    /*10b5*/ INV,
    /*10b6*/ INV,
    /*10b7*/ INV,
    /*10b8*/ INV,
    /*10b9*/ INV,
    /*10ba*/ INV,
    /*10bb*/ INV,
    /*10bc*/ CMPY  | EXT  |  8,
    /*10bd*/ INV,
    /*10be*/ LDY   | EXT  |  7,
    /*10bf*/ STY   | EXT  |  7,

    /*10c0*/ INV,
    /*10c1*/ INV,
    /*10c2*/ INV,
    /*10c3*/ INV,
    /*10c4*/ INV,
    /*10c5*/ INV,
    /*10c6*/ INV,
    /*10c7*/ INV,
    /*10c8*/ INV,
    /*10c9*/ INV,
    /*10ca*/ INV,
    /*10cb*/ INV,
    /*10cc*/ INV,
    /*10cd*/ INV,
    /*10ce*/ LDS   | IMM2 |  4,
    /*10cf*/ INV,

    /*10d0*/ INV,
    /*10d1*/ INV,
    /*10d2*/ INV,
    /*10d3*/ INV,
    /*10d4*/ INV,
    /*10d5*/ INV,
    /*10d6*/ INV,
    /*10d7*/ INV,
    /*10d8*/ INV,
    /*10d9*/ INV,
    /*10da*/ INV,
    /*10db*/ INV,
    /*10dc*/ INV,
    /*10dd*/ INV,
    /*10de*/ LDS   | DIR  |  6,
    /*10df*/ STS   | DIR  |  6,

    /*10e0*/ INV,
    /*10e1*/ INV,
    /*10e2*/ INV,
    /*10e3*/ INV,
    /*10e4*/ INV,
    /*10e5*/ INV,
    /*10e6*/ INV,
    /*10e7*/ INV,
    /*10e8*/ INV,
    /*10e9*/ INV,
    /*10ea*/ INV,
    /*10eb*/ INV,
    /*10ec*/ INV,
    /*10ed*/ INV,
    /*10ee*/ LDS   | IND  |  6,
    /*10ef*/ STS   | IND  |  6,

    /*10f0*/ INV,
    /*10f1*/ INV,
    /*10f2*/ INV,
    /*10f3*/ INV,
    /*10f4*/ INV,
    /*10f5*/ INV,
    /*10f6*/ INV,
    /*10f7*/ INV,
    /*10f8*/ INV,
    /*10f9*/ INV,
    /*10fa*/ INV,
    /*10fb*/ INV,
    /*10fc*/ INV,
    /*10fd*/ INV,
    /*10fe*/ LDS   | EXT  |  7,
    /*10ff*/ STS   | EXT  |  7,

    /*1100*/ INV,
    /*1101*/ INV,
    /*1102*/ INV,
    /*1103*/ INV,
    /*1104*/ INV,
    /*1105*/ INV,
    /*1106*/ INV,
    /*1107*/ INV,
    /*1108*/ INV,
    /*1109*/ INV,
    /*110a*/ INV,
    /*110b*/ INV,
    /*110c*/ INV,
    /*110d*/ INV,
    /*110e*/ INV,
    /*110f*/ INV,

    /*1110*/ INV,
    /*1111*/ INV,
    /*1112*/ INV,
    /*1113*/ INV,
    /*1114*/ INV,
    /*1115*/ INV,
    /*1116*/ INV,
    /*1117*/ INV,
    /*1118*/ INV,
    /*1119*/ INV,
    /*111a*/ INV,
    /*111b*/ INV,
    /*111c*/ INV,
    /*111d*/ INV,
    /*111e*/ INV,
    /*111f*/ INV,

    /*1120*/ INV,
    /*1121*/ INV,
    /*1122*/ INV,
    /*1123*/ INV,
    /*1124*/ INV,
    /*1125*/ INV,
    /*1126*/ INV,
    /*1127*/ INV,
    /*1128*/ INV,
    /*1129*/ INV,
    /*112a*/ INV,
    /*112b*/ INV,
    /*112c*/ INV,
    /*112d*/ INV,
    /*112e*/ INV,
    /*112f*/ INV,

    /*1130*/ INV,
    /*1131*/ INV,
    /*1132*/ INV,
    /*1133*/ INV,
    /*1134*/ INV,
    /*1135*/ INV,
    /*1136*/ INV,
    /*1137*/ INV,
    /*1138*/ INV,
    /*1139*/ INV,
    /*113a*/ INV,
    /*113b*/ INV,
    /*113c*/ INV,
    /*113d*/ INV,
    /*113e*/ INV,
    /*113f*/ SWI3  | INH  | 20,

    /*1140*/ INV,
    /*1141*/ INV,
    /*1142*/ INV,
    /*1143*/ INV,
    /*1144*/ INV,
    /*1145*/ INV,
    /*1146*/ INV,
    /*1147*/ INV,
    /*1148*/ INV,
    /*1149*/ INV,
    /*114a*/ INV,
    /*114b*/ INV,
    /*114c*/ INV,
    /*114d*/ INV,
    /*114e*/ INV,
    /*114f*/ INV,

    /*1150*/ INV,
    /*1151*/ INV,
    /*1152*/ INV,
    /*1153*/ INV,
    /*1154*/ INV,
    /*1155*/ INV,
    /*1156*/ INV,
    /*1157*/ INV,
    /*1158*/ INV,
    /*1159*/ INV,
    /*115a*/ INV,
    /*115b*/ INV,
    /*115c*/ INV,
    /*115d*/ INV,
    /*115e*/ INV,
    /*115f*/ INV,

    /*1160*/ INV,
    /*1161*/ INV,
    /*1162*/ INV,
    /*1163*/ INV,
    /*1164*/ INV,
    /*1165*/ INV,
    /*1166*/ INV,
    /*1167*/ INV,
    /*1168*/ INV,
    /*1169*/ INV,
    /*116a*/ INV,
    /*116b*/ INV,
    /*116c*/ INV,
    /*116d*/ INV,
    /*116e*/ INV,
    /*116f*/ INV,

    /*1170*/ INV,
    /*1171*/ INV,
    /*1172*/ INV,
    /*1173*/ INV,
    /*1174*/ INV,
    /*1175*/ INV,
    /*1176*/ INV,
    /*1177*/ INV,
    /*1178*/ INV,
    /*1179*/ INV,
    /*117a*/ INV,
    /*117b*/ INV,
    /*117c*/ INV,
    /*117d*/ INV,
    /*117e*/ INV,
    /*117f*/ INV,

    /*1180*/ INV,
    /*1181*/ INV,
    /*1182*/ INV,
    /*1183*/ CMPU  | IMM2 |  5,
    /*1184*/ INV,
    /*1185*/ INV,
    /*1186*/ INV,
    /*1187*/ INV,
    /*1188*/ INV,
    /*1189*/ INV,
    /*118a*/ INV,
    /*118b*/ INV,
    /*118c*/ CMPS  | IMM2 |  5,
    /*118d*/ INV,
    /*118e*/ INV,
    /*118f*/ INV,

    /*1190*/ INV,
    /*1191*/ INV,
    /*1192*/ INV,
    /*1193*/ CMPU  | DIR  |  7,
    /*1194*/ INV,
    /*1195*/ INV,
    /*1196*/ INV,
    /*1197*/ INV,
    /*1198*/ INV,
    /*1199*/ INV,
    /*119a*/ INV,
    /*119b*/ INV,
    /*119c*/ CMPS  | DIR  |  7,
    /*119d*/ INV,
    /*119e*/ INV,
    /*119f*/ INV,

    /*11a0*/ INV,
    /*11a1*/ INV,
    /*11a2*/ INV,
    /*11a3*/ CMPU  | IND  |  7,
    /*11a4*/ INV,
    /*11a5*/ INV,
    /*11a6*/ INV,
    /*11a7*/ INV,
    /*11a8*/ INV,
    /*11a9*/ INV,
    /*11aa*/ INV,
    /*11ab*/ INV,
    /*11ac*/ CMPS  | IND  |  7,
    /*11ad*/ INV,
    /*11ae*/ INV,
    /*11af*/ INV,

    /*11b0*/ INV,
    /*11b1*/ INV,
    /*11b2*/ INV,
    /*11b3*/ CMPU  | EXT  |  8,
    /*11b4*/ INV,
    /*11b5*/ INV,
    /*11b6*/ INV,
    /*11b7*/ INV,
    /*11b8*/ INV,
    /*11b9*/ INV,
    /*11ba*/ INV,
    /*11bb*/ INV,
    /*11bc*/ CMPS  | EXT  |  8,
    /*11bd*/ INV,
    /*11be*/ INV,
    /*11bf*/ INV,

    /*11c0*/ INV,
    /*11c1*/ INV,
    /*11c2*/ INV,
    /*11c3*/ INV,
    /*11c4*/ INV,
    /*11c5*/ INV,
    /*11c6*/ INV,
    /*11c7*/ INV,
    /*11c8*/ INV,
    /*11c9*/ INV,
    /*11ca*/ INV,
    /*11cb*/ INV,
    /*11cc*/ INV,
    /*11cd*/ INV,
    /*11ce*/ INV,
    /*11cf*/ INV,

    /*11d0*/ INV,
    /*11d1*/ INV,
    /*11d2*/ INV,
    /*11d3*/ INV,
    /*11d4*/ INV,
    /*11d5*/ INV,
    /*11d6*/ INV,
    /*11d7*/ INV,
    /*11d8*/ INV,
    /*11d9*/ INV,
    /*11da*/ INV,
    /*11db*/ INV,
    /*11dc*/ INV,
    /*11dd*/ INV,
    /*11de*/ INV,
    /*11df*/ INV,

    /*11e0*/ INV,
    /*11e1*/ INV,
    /*11e2*/ INV,
    /*11e3*/ INV,
    /*11e4*/ INV,
    /*11e5*/ INV,
    /*11e6*/ INV,
    /*11e7*/ INV,
    /*11e8*/ INV,
    /*11e9*/ INV,
    /*11ea*/ INV,
    /*11eb*/ INV,
    /*11ec*/ INV,
    /*11ed*/ INV,
    /*11ee*/ INV,
    /*11ef*/ INV,

    /*11f0*/ INV,
    /*11f1*/ INV,
    /*11f2*/ INV,
    /*11f3*/ INV,
    /*11f4*/ INV,
    /*11f5*/ INV,
    /*11f6*/ INV,
    /*11f7*/ INV,
    /*11f8*/ INV,
    /*11f9*/ INV,
    /*11fa*/ INV,
    /*11fb*/ INV,
    /*11fc*/ INV,
    /*11fd*/ INV,
    /*11fe*/ INV,
    /*11ff*/ INV   | INV  |  0};

//Creation d'une liste de registres //////////////////////////////////////////
static void Reglist(int type)
{
  // string: pointeur vers la chaine de retour de la liste de registres
  // type: 0=EXG-TFR 1=PSHS 2=PULS 3=PSHU 4=PULU
  int i, c;
  const char s[8][4] =  {"PC,","U,","Y,","X,","DP,","B,","A,","CC,"};
  const char u[8][4] =  {"PC,","S,","Y,","X,","DP,","B,","A,","CC,"};
  const char r[16][4] = {"D,","X,","Y,","U,","S,","PC,","?,","?,",
                         "A,","B,","CC,","DP,","?,","?,","?,","?,"};
  param[0] = 0;
  c = Mgetc(pc++) & 0xff;
  sprintf(w, "%02X", c);
  strcat(hexa, w);
  switch(type)
  {
    case 0: sprintf(param, "%s%s", r[(c & 0xf0) >> 4], r[c & 0x0f]); break;
    case 1: for(i = 0; i < 8; i++)  if(c & (128>>i)) strcat(param, s[i]); break;
    case 2: for(i = 7; i >= 0; i--) if(c & (128>>i)) strcat(param, s[i]); break;
    case 3: for(i = 0; i < 8; i++)  if(c & (128>>i)) strcat(param, u[i]); break;
    case 4: for(i = 7; i >= 0; i--) if(c & (128>>i)) strcat(param, u[i]); break;
  }
  if(strlen(param) > 0) param[strlen(param) - 1] = 0;
  if(type == 0) return;
  if(c & 0x80) n += 2;
  if(c & 0x40) n += 2;
  if(c & 0x20) n += 2;
  if(c & 0x10) n += 2;
  if(c & 0x08) n += 1;
  if(c & 0x04) n += 1;
  if(c & 0x02) n += 1;
  if(c & 0x01) n += 1;
}

//Decodage de l'adressage indexe /////////////////////////////////////////////
static void Indexed()
{
  //string est la ligne desassemblee
  //operande est le champ des operandes
  //a est l'adresse en memoire du premier parametre de l'instruction indexee
  //retour: adresse de l'instruction suivante
  int i, x;
  char reg;
  //selection du registre concerne
  GETBYTE; x = i;
  switch(x & 0x60)
  {
    case 0x00: reg = 'X'; break;
    case 0x20: reg = 'Y'; break;
    case 0x40: reg = 'U'; break;
    case 0x60: reg = 'S'; break;
    default :  reg = ' ';
  }
  switch(x & 0x9f)
  {
    case 0x80: n = 2; sprintf(param, ",%c+", reg); break;
    case 0x81: n = 3; sprintf(param, ",%c++", reg); break;
    case 0x82: n = 2; sprintf(param, ",-%c", reg); break;
    case 0x83: n = 3; sprintf(param, ",--%c", reg); break;
    case 0x84: n = 0; sprintf(param, ",%c", reg); break;
    case 0x85: n = 1; sprintf(param, "B,%c", reg); break;
    case 0x86: n = 1; sprintf(param, "A,%c", reg); break;
    case 0x87: n = 0; sprintf(param, "invalid"); break;
    case 0x88: n = 1; GETBYTE; sprintf(param, "$%02X,%c", i, reg); break;
    case 0x89: n = 4; GETWORD; sprintf(param, "$%04X,%c", i, reg); break;
    case 0x8a: n = 0; sprintf(param, "invalid"); break;
    case 0x8b: n = 4; sprintf(param, "D,%c", reg); break;
    case 0x8c: n = 1; GETBYTE; i = pc + (char)i; i &= 0xffff;
    sprintf(param, "$%04X,%s", i, "PCR"); break;
    case 0x8d: n = 5; GETWORD; i = pc + (short)i; i &= 0xffff;
    sprintf(param, "$%04X,%s", i, "PCR"); break;
    case 0x8e: n = 0; sprintf(param, "invalid"); break;
    case 0x8f: n = 0; sprintf(param, "invalid"); break;
    case 0x90: n = 3; sprintf(param, "invalid"); break;
    case 0x91: n = 6; sprintf(param, "[,%c++]", reg); break;
    case 0x92: n = 3; sprintf(param, "invalid"); break;
    case 0x93: n = 6; sprintf(param, "[,--%c]", reg); break;
    case 0x94: n = 3; sprintf(param, "[,%c]", reg); break;
    case 0x95: n = 4; sprintf(param, "[B,%c]", reg); break;
    case 0x96: n = 4; sprintf(param, "[A,%c]", reg); break;
    case 0x97: n = 3; sprintf(param, "invalid"); break;
    case 0x98: n = 4; GETBYTE; sprintf(param, "[$%02X,%c]", i, reg); break;
    case 0x99: n = 7; GETWORD; sprintf(param, "[$%04X,%c]", i, reg); break;
    case 0x9a: n = 3; sprintf(param, "invalid"); break;
    case 0x9b: n = 7; sprintf(param, "[D,%c]", reg); break;
    case 0x9c: n = 4; GETBYTE; i = pc + (char)i; i &= 0xffff;
    sprintf(param, "[$%04X,%s]", i, "PCR"); break;
    case 0x9d: n = 8; GETWORD; i = pc + (short)i; i &= 0xffff;
    sprintf(param, "[$%04X,%s]", i, "PCR"); break;
    case 0x9e: n = 3; sprintf(param, "invalid"); break;
    case 0x9f: n = 5; GETWORD; sprintf(param, "[$%04X]", i); break;
    default:   n = 1; if(!(x & 0x10)) sprintf(param, "$%02X,%c", x & 0x0f, reg);
    else sprintf(param, "-$%02X,%c", 16 - (x & 0x0f), reg);
    //Assumes 0x84 for invalid bytes 0x87 0x8a 0x8e 0x8f
    //Assumes 0x94 for invalid bytes 0x90 0x92 0x97 0x9a 0x9e
  }
}

// Desassembler ///////////////////////////////////////////////////////////////
int dasm6809(int address, char *string)
{
  unsigned int i, precode, code;
  // initialisations
  pc = address; // disassembling address
  n = 0;        // number of cpu cycles
  sprintf(hexa, "%04X ", address);
  // read opcode
  GETBYTE; code = i; precode = 0;
  if(code == 0x10) {precode = 0x100; i = Mgetc(pc) & 0xff;}
  if(code == 0x11) {precode = 0x200; i = Mgetc(pc) & 0xff;}
  if(precode > 0) if((i != 0x10) && (i != 0x11)) {GETBYTE; code = precode | i;}
  // opcode decoding
  switch(instruction[code] & 0xff00)
  {
    case INH:  param[0] = 0; break;
    case DIR:  GETBYTE; sprintf(param, "/$%02X", i); break;
    case IMM1: GETBYTE; sprintf(param, "#$%02X", i); break;
    case IMM2: GETWORD; sprintf(param, "#$%04X", i); break;
    case REL1: GETBYTE; sprintf(param, "$%04X", (pc + (char)i) & 0xffff); break;
    case REL2: GETWORD; sprintf(param, "$%04X", (pc + (short)i) & 0xffff); break;
    case IND:  Indexed(); break;
    case EXT:  GETWORD; sprintf(param, "$%04X", i); break;
    case REG0: Reglist(0); break;
    case REG1: Reglist(1); break;
    case REG2: Reglist(2); break;
    case REG3: Reglist(3); break;
    case REG4: Reglist(4); break;
    default: param[0] = 0; break;
  }
  n += instruction[code] & 0xff;
  if (n == 0) strcpy(w, "?"); else sprintf(w, "%i", n);
  sprintf(string, "%-13s %-5s %-13s %2s", hexa, mnemonique[instruction[code] >> 16], param, w);
  // return address of next instruction
  return pc;
}
