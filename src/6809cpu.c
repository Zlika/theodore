/*
 * This file is part of theodore (https://github.com/Zlika/theodore),
 * a Thomson emulator based on Daniel Coulom's DCTO8D emulator
 * (http://dcto8.free.fr/).
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

/* Motorola 6809 microprocessor emulation */

//pointeurs vers fonctions d'acces memoire
char (*Mgetc)(unsigned short a);
void (*Mputc)(unsigned short a, char c);

//global variables
static int dc6809_cycles; //additional cycles
static int dc6809_sync;   //synchronisation flag
int dc6809_irq;    //irq trigger  (0=inactif)
static int dc6809_firq;   //firq trigger (0=inactif)
static int dc6809_nmi;    //nmi trigger  (0=inactif)
static short dc6809_w;    //dc6809 work register

//6809 registers
char  dc6809_cc;   //condition code
unsigned short dc6809_pc; //program counter
static short dc6809_d;    //D register
short dc6809_x;    //X register
short dc6809_y;    //Y register
short dc6809_u;    //U register
short dc6809_s;    //S register
static short dc6809_da;   //direct address (DP register = high byte of direct address)

//pointers to register bytes
char *dc6809_a;    //pointer to A register
char *dc6809_b;    //pointer to B register
char *dc6809_dp;   //pointer to DP register
static char *dc6809_dd;   //pointer to direct address low byte
static char *dc6809_pch;  //pointer to PC low byte
static char *dc6809_pcl;  //pointer to PC high byte
static char *dc6809_xh;   //pointer to X low byte
static char *dc6809_xl;   //pointer to X high byte
static char *dc6809_yh;   //pointer to Y low byte
static char *dc6809_yl;   //pointer to Y high byte
static char *dc6809_uh;   //pointer to U low byte
static char *dc6809_ul;   //pointer to U high byte
static char *dc6809_sh;   //pointer to S low byte
static char *dc6809_sl;   //pointer to S high byte

//aliases
#define AP   dc6809_a
#define BP   dc6809_b
#define N    dc6809_cycles
#define CC   dc6809_cc
#define PC   dc6809_pc
#define PCH *dc6809_pch
#define PCL *dc6809_pcl
#define DA   dc6809_da
#define DP  *dc6809_dp
#define DD  *dc6809_dd
#define D    dc6809_d
#define A   *dc6809_a
#define B   *dc6809_b
#define X    dc6809_x
#define XH  *dc6809_xh
#define XL  *dc6809_xl
#define Y    dc6809_y
#define YH  *dc6809_yh
#define YL  *dc6809_yl
#define U    dc6809_u
#define UH  *dc6809_uh
#define UL  *dc6809_ul
#define S    dc6809_s
#define SH  *dc6809_sh
#define SL  *dc6809_sl
#define W    dc6809_w

/* memory access C = 1 byte, W = 2 bytes */
#define GETC(x)   Mgetc(x)
#define PUTC(x,y) Mputc(x,y)
#define GETW(x)   (Mgetc(x)<<8|(Mgetc(x+1)&0xff))
#define PUTW(x,y) {Mputc(x,y>>8);Mputc(x+1,y);}

/*condition code masks (CC=EFHINZVC)*/
#define  CC_C 0x01  /* carry */
#define  CC_V 0x02  /* overflow */
#define  CC_Z 0x04  /* zero */
#define  CC_N 0x08  /* negative */
#define  CC_I 0x10  /* irq mask */
#define  CC_H 0x20  /* half-carry */
#define  CC_F 0x40  /* firq mask */
#define  CC_E 0x80  /* extended registers save */

/*conditional jump*/
/*
C BCC BCS      Z BNE BEQ      V BVC BVS     NZV BGT BLE
0 yes no       0 yes no       0 yes no      000 yes no
1 no  yes      1 no  yes      1 no  yes     100 no  yes
                                            010 no  yes
N BL BMI     ZC BHI BLS     NV BGE BLT     001 no  yes
0 yes no      00 yes no      00 yes no      110 no  no
1 no  yes     10 no  yes     10 no  yes     101 yes no
              01 no  yes     01 no  yes     011 no  no
              11 no  no      11 yes no      111 no  yes
 */
#define CC_BCC (dc6809_cc&1)==0  // BCC = BHS
#define CC_BCS (dc6809_cc&1)==1  // BCS = BLO
#define CC_BVC (dc6809_cc&2)==0
#define CC_BVS (dc6809_cc&2)==2
#define CC_BNE (dc6809_cc&4)==0
#define CC_BEQ (dc6809_cc&4)==4
#define CC_BHI (dc6809_cc&5)==0
#define CC_BLS (dc6809_cc&5)==4||(dc6809_cc&5)==1
#define CC_BL  (dc6809_cc&8)==0
#define CC_BMI (dc6809_cc&8)==8
#define CC_BGE (dc6809_cc&10)==0||(dc6809_cc&10)==10
#define CC_BLT (dc6809_cc&10)==8||(dc6809_cc&10)==2
#define CC_BGT (dc6809_cc&14)==0||(dc6809_cc&14)==10
#define CC_BLE (dc6809_cc&14)==8||(dc6809_cc&14)==14||(dc6809_cc&14)==4||(dc6809_cc&14)==2
#define BRANCH {dc6809_pc+=GETC(dc6809_pc);}
#define LBRANCH {dc6809_pc+=GETW(dc6809_pc);dc6809_cycles++;}

//repetitive code
#define INDIRECT Mgeti()
#define DIRECT *dc6809_dd=GETC(dc6809_pc);dc6809_pc++
#define EXTENDED dc6809_w=GETW(dc6809_pc);dc6809_pc+=2
#define SET_Z if(dc6809_w)dc6809_cc&=0xfb;else dc6809_cc|=0x04

// Fonctions d'acces memoire
short Mgetw(unsigned short a) {return (Mgetc(a) << 8 | (Mgetc(a+1) & 0xff));}
void Mputw(unsigned short a, short w) {Mputc(a, w >> 8); Mputc(++a, w);}

// Processor initialisation //////////////////////////////////////////////////
static void Init6809(void)
{
  int i = 1;              //integer used to test endianness
  char *c = (char*)&i;    //left byte of integer i
  //register pointer initialisation
  dc6809_pch = dc6809_pcl = (char*)&dc6809_pc; //PC msb & lsb
  dc6809_a = dc6809_b = (char*)&dc6809_d;      // D msb & lsb
  dc6809_xh = dc6809_xl = (char*)&dc6809_x;    // X msb & lsb
  dc6809_yh = dc6809_yl = (char*)&dc6809_y;    // Y msb & lsb
  dc6809_uh = dc6809_ul = (char*)&dc6809_u;    // U msb & lsb
  dc6809_sh = dc6809_sl = (char*)&dc6809_s;    // S msb & lsb
  dc6809_dp = dc6809_dd = (char*)&dc6809_da;   //DP msb & lsb
  //test endianness: 1=little-endian, 0=big-endian
  switch(c[0])
  {
    case 1:  dc6809_xh++; dc6809_yh++; dc6809_uh++; dc6809_sh++;
    dc6809_pch++; dc6809_dp++; dc6809_a++; break;
    default: dc6809_xl++; dc6809_yl++; dc6809_ul++; dc6809_sl++;
    dc6809_pcl++; dc6809_dd++; dc6809_b++; break;
  }
}

// Processor reset ///////////////////////////////////////////////////////////
void Reset6809(void)
{
  Init6809();
  dc6809_sync = 0;   //synchronisation flag
  dc6809_irq = 0;    //irq trigger
  dc6809_firq = 0;   //firq trigger
  dc6809_nmi = 0;    //nmi trigger
  CC = 0x10;         //condition code
  PC = GETW(0xfffe); //program counter
}

// Get memory (indexed) //////////////////////////////////////////////////////
static void Mgeti(void)
{
  int i;
  short *r;
  i = GETC(PC); PC++;
  switch (i & 0x60)
  {
    case 0x00: r = &X; break;
    case 0x20: r = &Y; break;
    case 0x40: r = &U; break;
    case 0x60: r = &S; break;
    default: r = &X;
  }
  switch(i &= 0x9f)
  {
    case 0x80: N = 2; W = *r; *r += 1; return;                    // ,R+
    case 0x81: N = 3; W = *r; *r += 2; return;                    // ,R++
    case 0x82: N = 2; *r -= 1; W = *r; return;                    // ,-R
    case 0x83: N = 3; *r -= 2; W = *r; return;                    // ,--R
    case 0x84: N = 0; W = *r; return;                             // ,R
    case 0x85: N = 1; W = *r + B; return;                         // B,R
    case 0x86: N = 1; W = *r + A; return;                         // A,R
    case 0x87: N = 0; W = *r; return;                             // invalid
    case 0x88: N = 1; W = *r + GETC(PC); PC++; return;            // char,R
    case 0x89: N = 4; EXTENDED; W += *r; return;                  // word,R
    case 0x8a: N = 0; W = *r; return;                             // invalid
    case 0x8b: N = 4; W = *r + D; return;                         // D,R
    case 0x8c: N = 1; W = GETC(PC); PC++; W += PC; return;        // char,PCR
    case 0x8d: N = 5; EXTENDED; W += PC; return;                  // word,PCR
    case 0x8e: N = 0; W = *r; return;                             // invalid
    case 0x8f: N = 0; W = *r; return;                             // invalid
    case 0x90: N = 3; W = GETW(*r); return;                       // invalid
    case 0x91: N = 6; *r += 2; W = GETW(*r - 2); return;          // [,R++]
    case 0x92: N = 3; W = GETW(*r); return;                       // invalid
    case 0x93: N = 6; *r -= 2; W = GETW(*r); return;              // [,--R]
    case 0x94: N = 3; W = GETW(*r); return;                       // [,R]
    case 0x95: N = 4; W = GETW(*r + B); return;                   // [B,R]
    case 0x96: N = 4; W = GETW(*r + A); return;                   // [A,R]
    case 0x97: N = 3; W = GETW(*r); return;                       // invalid
    case 0x98: N = 4; W = GETW(*r + GETC(PC)); PC++; return;      // [char,R]
    case 0x99: N = 7; EXTENDED; W = GETW(*r + W); return;         // [word,R]
    case 0x9a: N = 3; W = GETW(*r); return;                       // invalid
    case 0x9b: N = 7; W = GETW(*r + D); return;                   // [D,R]
    case 0x9c: N = 4; W = GETW(PC+1+GETC(PC)); PC++; return;      // [char,PCR]
    case 0x9d: N = 8; EXTENDED; W = GETW(PC + W); return;         // [word,PCR]
    case 0x9e: N = 3; W = GETW(*r); return;                       // invalid
    case 0x9f: N = 5; EXTENDED; W = GETW(W); return;              // [word]
    default  : N = 1; if(i & 0x10) i -= 0x20; W = *r + i; return; // 5 bits,R
    //Assumes 0x84 for invalid bytes 0x87 0x8a 0x8e 0x8f
    //Assumes 0x94 for invalid bytes 0x90 0x92 0x97 0x9a 0x9e
  }
}

// PSH, PUL, EXG, TFR /////////////////////////////////////////////////////////
static void Pshs(char c)
{
  if(c & 0x80) {PUTC(--S,PCL); PUTC(--S,PCH); N += 2;}
  if(c & 0x40) {PUTC(--S, UL); PUTC(--S, UH); N += 2;}
  if(c & 0x20) {PUTC(--S, YL); PUTC(--S, YH); N += 2;}
  if(c & 0x10) {PUTC(--S, XL); PUTC(--S, XH); N += 2;}
  if(c & 0x08) {PUTC(--S, DP); N += 1;}
  if(c & 0x04) {PUTC(--S,  B); N += 1;}
  if(c & 0x02) {PUTC(--S,  A); N += 1;}
  if(c & 0x01) {PUTC(--S, CC); N += 1;}
}

static void Pshu(char c)
{
  if(c & 0x80) {PUTC(--U,PCL); PUTC(--U,PCH); N += 2;}
  if(c & 0x40) {PUTC(--U, SL); PUTC(--U, SH); N += 2;}
  if(c & 0x20) {PUTC(--U, YL); PUTC(--U, YH); N += 2;}
  if(c & 0x10) {PUTC(--U, XL); PUTC(--U, XH); N += 2;}
  if(c & 0x08) {PUTC(--U, DP); N += 1;}
  if(c & 0x04) {PUTC(--U,  B); N += 1;}
  if(c & 0x02) {PUTC(--U,  A); N += 1;}
  if(c & 0x01) {PUTC(--U, CC); N += 1;}
}

static void Puls(char c)
{
  if(c & 0x01) {CC = GETC(S); S++; N += 1;}
  if(c & 0x02) { A = GETC(S); S++; N += 1;}
  if(c & 0x04) { B = GETC(S); S++; N += 1;}
  if(c & 0x08) {DP = GETC(S); S++; N += 1;}
  if(c & 0x10) {XH = GETC(S); S++; XL = GETC(S); S++; N += 2;}
  if(c & 0x20) {YH = GETC(S); S++; YL = GETC(S); S++; N += 2;}
  if(c & 0x40) {UH = GETC(S); S++; UL = GETC(S); S++; N += 2;}
  if(c & 0x80) {PCH= GETC(S); S++; PCL= GETC(S); S++; N += 2;}
}

static void Pulu(char c)
{
  if(c & 0x01) {CC = GETC(U); U++; N += 1;}
  if(c & 0x02) { A = GETC(U); U++; N += 1;}
  if(c & 0x04) { B = GETC(U); U++; N += 1;}
  if(c & 0x08) {DP = GETC(U); U++; N += 1;}
  if(c & 0x10) {XH = GETC(U); U++; XL = GETC(U); U++; N += 2;}
  if(c & 0x20) {YH = GETC(U); U++; YL = GETC(U); U++; N += 2;}
  if(c & 0x40) {SH = GETC(U); U++; SL = GETC(U); U++; N += 2;}
  if(c & 0x80) {PCH= GETC(U); U++; PCL= GETC(U); U++; N += 2;}
}

static void Exg(char c)
{
  switch(c & 0xff)
  {
    case 0x01: W = D; D = X; X = W; return;     //D-X
    case 0x02: W = D; D = Y; Y = W; return;     //D-Y
    case 0x03: W = D; D = U; U = W; return;     //D-U
    case 0x04: W = D; D = S; S = W; return;     //D-S
    case 0x05: W = D; D = PC; PC = W; return;   //D-PC
    case 0x10: W = X; X = D; D = W; return;     //X-D
    case 0x12: W = X; X = Y; Y = W; return;     //X-Y
    case 0x13: W = X; X = U; U = W; return;     //X-U
    case 0x14: W = X; X = S; S = W; return;     //X-S
    case 0x15: W = X; X = PC; PC = W; return;   //X-PC
    case 0x20: W = Y; Y = D; D = W; return;     //Y-D
    case 0x21: W = Y; Y = X; X = W; return;     //Y-X
    case 0x23: W = Y; Y = U; U = W; return;     //Y-U
    case 0x24: W = Y; Y = S; S = W; return;     //Y-S
    case 0x25: W = Y; Y = PC; PC = W; return;   //Y-PC
    case 0x30: W = U; U = D; D = W; return;     //U-D
    case 0x31: W = U; U = X; X = W; return;     //U-X
    case 0x32: W = U; U = Y; Y = W; return;     //U-Y
    case 0x34: W = U; U = S; S = W; return;     //U-S
    case 0x35: W = U; U = PC; PC = W; return;   //U-PC
    case 0x40: W = S; S = D; D = W; return;     //S-D
    case 0x41: W = S; S = X; X = W; return;     //S-X
    case 0x42: W = S; S = Y; Y = W; return;     //S-Y
    case 0x43: W = S; S = U; U = W; return;     //S-U
    case 0x45: W = S; S = PC; PC = W; return;   //S-PC
    case 0x50: W = PC; PC = D; D = W; return;   //PC-D
    case 0x51: W = PC; PC = X; X = W; return;   //PC-X
    case 0x52: W = PC; PC = Y; Y = W; return;   //PC-Y
    case 0x53: W = PC; PC = U; U = W; return;   //PC-U
    case 0x54: W = PC; PC = S; S = W; return;   //PC-S
    case 0x89: W = A; A = B; B = W; return;     //A-B
    case 0x8a: W = A; A = CC; CC = W; return;   //A-CC
    case 0x8b: W = A; A = DP; DP = W; return;   //A-DP
    case 0x98: W = B; B = A; A = W; return;     //B-A
    case 0x9a: W = B; B = CC; CC = W; return;   //B-CC
    case 0x9b: W = B; B = DP; DP = W; return;   //B-DP
    case 0xa8: W = CC; CC = A; A = W; return;   //CC-A
    case 0xa9: W = CC; CC = B; B = W; return;   //CC-B
    case 0xab: W = CC; CC = DP; DP = W; return; //CC-DP
    case 0xb8: W = DP; DP = A; A = W; return;   //DP-A
    case 0xb9: W = DP; DP = B; B = W; return;   //DP-B
    case 0xba: W = DP; DP = CC; CC = W; return; //DP-CC
  }
}

static void Tfr(char c)
{
  switch(c & 0xff)
  {
    case 0x01: X = D; return;
    case 0x02: Y = D; return;
    case 0x03: U = D; return;
    case 0x04: S = D; return;
    case 0x05: PC = D; return;
    case 0x10: D = X; return;
    case 0x12: Y = X; return;
    case 0x13: U = X; return;
    case 0x14: S = X; return;
    case 0x15: PC = X; return;
    case 0x20: D = Y; return;
    case 0x21: X = Y; return;
    case 0x23: U = Y; return;
    case 0x24: S = Y; return;
    case 0x25: PC = Y; return;
    case 0x30: D = U; return;
    case 0x31: X = U; return;
    case 0x32: Y = U; return;
    case 0x34: S = U; return;
    case 0x35: PC = U; return;
    case 0x40: D = S; return;
    case 0x41: X = S; return;
    case 0x42: Y = S; return;
    case 0x43: U = S; return;
    case 0x45: PC = S; return;
    case 0x50: D = PC; return;
    case 0x51: X = PC; return;
    case 0x52: Y = PC; return;
    case 0x53: U = PC; return;
    case 0x54: S = PC; return;
    case 0x89: B = A; return;
    case 0x8a: CC = A; return;
    case 0x8b: DP = A; return;
    case 0x98: A = B; return;
    case 0x9a: CC = B; return;
    case 0x9b: DP = B; return;
    case 0xa8: A = CC; return;
    case 0xa9: B = CC; return;
    case 0xab: DP = CC; return;
    case 0xb8: A = DP; return;
    case 0xb9: B = DP; return;
    case 0xba: CC = DP; return;
  }
}

// CLR, NEG, COM, INC, DEC  (CC=EFHINZVC) /////////////////////////////////////
static char Clr(void)
{
  CC &= 0xf0;
  CC |= CC_Z;
  return 0;
}

static char Neg(char c)
{
  CC &= 0xf0;
  if(c == -128) CC |= CC_V;
  c = - c;
  if(c != 0) CC |= CC_C;
  if(c < 0) CC |= CC_N;
  if(c == 0) CC |= CC_Z;
  return c;
}

static char Com(char c)
{
  CC &= 0xf0;
  c = ~c;
  CC |= CC_C;
  if(c < 0) CC |= CC_N;
  if(c == 0) CC |= CC_Z;
  return c;
}

static char Inc(char c)
{
  CC &= 0xf1;
  if(c == 127) CC |= CC_V;
  c++;
  if(c < 0) CC |= CC_N;
  if(c == 0) CC |= CC_Z;
  return c;
}

static char Dec(char c)
{
  CC &= 0xf1;
  if(c == -128) CC |= CC_V;
  c--;
  if(c < 0) CC |= CC_N;
  if(c == 0) CC |= CC_Z;
  return c;
}

// Registers operations  (CC=EFHINZVC) ////////////////////////////////////////
static void Mul(void)
{
  D = (A & 0xff) * (B & 0xff);
  CC &= 0xf2;
  if(D < 0) CC |= CC_C;
  if(D == 0) CC |= CC_Z;
}

static void Addc(char *r, char c)
{
  int i = *r + c;
  CC &= 0xd0;
  if(((*r & 0x0f) + (c & 0x0f)) & 0x10) CC |= CC_H;
  if(((*r & 0xff) + (c & 0xff)) & 0x100) CC |= CC_C;
  *r = i & 0xff;
  if(*r != i) CC |= CC_V;
  if(*r < 0) CC |= CC_N;
  if(*r == 0) CC |= CC_Z;
}

static void Adc(char *r, char c)
{
  int carry = (CC & CC_C);
  int i = *r + c + carry;
  CC &= 0xd0;
  if(((*r & 0x0f) + (c & 0x0f) + carry) & 0x10) CC |= CC_H;
  if(((*r & 0xff) + (c & 0xff) + carry) & 0x100) CC |= CC_C;
  *r = i & 0xff;
  if(*r != i) CC |= CC_V;
  if(*r < 0) CC |= CC_N;
  if(*r == 0) CC |= CC_Z;
}

static void Addw(short *r, short word)
{
  int i = *r + word;
  CC &= 0xf0;
  if(((*r & 0xffff) + (word & 0xffff)) & 0xf0000) CC |= CC_C;
  *r = i & 0xffff;
  if(*r != i) CC |= CC_V;
  if(*r < 0) CC |= CC_N;
  if(*r == 0) CC |= CC_Z;
}

static void Subc(char *r, char c)
{
  int i = *r - c;
  CC &= 0xf0;
  if(((*r & 0xff) - (c & 0xff)) & 0x100) CC |= CC_C;
  *r = i & 0xff;
  if(*r != i) CC |= CC_V;
  if(*r < 0) CC |= CC_N;
  if(*r == 0) CC |= CC_Z;
}

static void Sbc(char *r, char c)
{
  int carry = (CC & CC_C);
  int i = *r - c - carry;
  CC &= 0xf0;
  if(((*r & 0xff) - (c & 0xff) - carry) & 0x100) CC |= CC_C;
  *r = i & 0xff;
  if(*r != i) CC |= CC_V;
  if(*r < 0) CC |= CC_N;
  if(*r == 0) CC |= CC_Z;
}

static void Subw(short *r, short word)
{
  int i = *r - word;
  CC &= 0xf0;
  if(((*r & 0xffff) - (word & 0xffff)) & 0x10000) CC |= CC_C;
  *r = i & 0xffff;
  if(*r != i) CC |= CC_V;
  if(*r < 0) CC |= CC_N;
  if(*r == 0) CC |= CC_Z;
}

static void Daa(void)
{
  int i = A & 0xff;
  if((CC & CC_H) || ((i & 0x00f) > 0x09)) i += 0x06;
  if((CC & CC_C) || ((i & 0x1f0) > 0x90)) i += 0x60;
  A = i & 0xff;
  i = (i >> 1 & 0xff) | (CC << 7);
  CC &= 0xf0;
  if(i & 0x80) CC |= CC_C;
  if((A ^ i) & 0x80) CC |= CC_V;
  if(A < 0) CC |= CC_N;
  if(A == 0) CC |= CC_Z;
}

// Shift and rotate  (CC=EFHINZVC) ////////////////////////////////////////////
static char Lsr(char c)
{
  CC &= 0xf2;
  if(c & 1) CC |= CC_C;
  c = (c & 0xff) >> 1;
  if(c == 0) CC |= CC_Z;
  return c;
}

static char Ror(char c)
{
  int carry = CC & CC_C;
  CC &= 0xf2;
  if(c & 1) CC |= CC_C;
  c = ((c & 0xff) >> 1) | (carry << 7);
  if(c < 0) CC |= CC_N;
  if(c == 0) CC |= CC_Z;
  return c;
}

static char Rol(char c)
{
  int carry = CC & CC_C;
  CC &= 0xf0;
  if(c < 0) CC |= CC_C;
  c = ((c & 0x7f) << 1) | carry;
  if((c >> 7 & 1) ^ (CC & CC_C)) CC |= CC_V;
  if(c < 0) CC |= CC_N;
  if(c == 0) CC |= CC_Z;
  return c;
}

static char Asr(char c)
{
  CC &= 0xf2;
  if(c & 1) CC |= CC_C;
  c = ((c & 0xff) >> 1) | (c & 0x80);
  if(c < 0) CC |= CC_N;
  if(c == 0) CC |= CC_Z;
  return c;
}

static char Asl(char c)
{
  CC &= 0xf0;
  if(c < 0) CC |= CC_C;
  c = (c & 0xff) << 1;
  if((c >> 7 & 1) ^ (CC & CC_C)) CC |= CC_V;
  if(c < 0) CC |= CC_N;
  if(c == 0) CC |= CC_Z;
  return c;
}

// Test and compare  (CC=EFHINZVC) ////////////////////////////////////////////
static void Tstc(char c)
{
  CC &= 0xf1;
  if(c < 0) CC |= CC_N;
  if(c == 0) CC |= CC_Z;
}

static void Tstw(short word)
{
  CC &= 0xf1;
  if(word < 0) CC |= CC_N;
  if(word == 0) CC |= CC_Z;
}

static void Cmpc(char *reg, char c)
{
  char r = *reg;
  int i = *reg - c;
  CC &= 0xf0;
  if(((r & 0xff) - (c & 0xff)) & 0x100) CC |= CC_C;
  r = i & 0xff;
  if(r != i) CC |= CC_V;
  if(r < 0) CC |= CC_N;
  if(r == 0) CC |= CC_Z;
}

static void Cmpw(short *reg, short word)
{
  short r = *reg;
  int i = *reg - word;
  CC &= 0xf0;
  if(((r & 0xffff) - (word & 0xffff)) & 0x10000) CC |= CC_C;
  r = i & 0xffff;
  if(r != i) CC |= CC_V;
  if(r < 0) CC |= CC_N;
  if(r == 0) CC |= CC_Z;
}

// Interrupt requests  (CC=EFHINZVC) //////////////////////////////////////////
static int Nmi(void)  //non masquable interrupt     a verifier !!!!
{
  CC |= CC_E;
  Pshs(0xff);
  CC |= CC_F;
  CC |= CC_I;
  PC = GETW(0xfffc);
  return 1;
}

static int Firq(void) //fast interrupt request
{
  if((CC & CC_F) == 0)          //si les FIRQs ne sont pas masquees
  {
    CC &= ~CC_E;
    Pshs(0x81);
    CC |= CC_F;
    CC |= CC_I;
    PC = GETW(0xfff6);
    return 1;
  }
  else return 0;
}

int Irq(void) //interrupt request
{
  if(dc6809_sync == 1) dc6809_sync = 2; //si attente synchro, indicateur d'interruption
  if((CC & CC_I) == 0)          //si les IRQs ne sont pas masquees
  {
    CC |= CC_E;
    Pshs(0xff);
    CC |= CC_I;
    PC = GETW(0xfff8);
    return 1;
  }
  else return 0;
}

static void Swi(int n) //software interrupt
{
  CC |= CC_E;
  Pshs(0xff);
  CC |= CC_I;
  if(n == 1) {PC = GETW(0xfffa); return;}
  if(n == 2) {PC = GETW(0xfff4); return;}
  if(n == 3) {PC = GETW(0xfff2); return;}
}

// RTI ////////////////////////////////////////////////////////////////////////
static void Rti(void)
{
  Puls(0x01);
  if(CC & CC_E) Puls(0xfe); else Puls(0x80);
}

// SYNC ///////////////////////////////////////////////////////////////////////
static void Sync(void)
{
  //positionner le flag d'attente de synchronisation
  if(dc6809_sync == 0) dc6809_sync = 1;
  //boucle si aucune interruption n'a ete detectee
  if(dc6809_sync != 2) PC--;
  //si interruption detectee: remise a zero du flag d'attente de synchronisation
  if(dc6809_sync == 2) dc6809_sync = 0;
}

// Execute one operation at pc address and set pc to next opcode address //////
/*
Return value is set to :
- cycle count for the executed instruction when operation code is legal
- negative value (-code) when operation code is illegal
 */
int Run6809(void)
{
  int precode, code;

  N = 0; //initialisation du nombre de cycles additionnels
  precode = 0; //par defaut l'instruction n'a pas de precode
  if(dc6809_nmi) if(Nmi()) return 7 + N;   //traitement NMI
  if(dc6809_firq) if(Firq()) return 7 + N; //traitement FIRQ
  if(dc6809_irq) if(Irq()) return 7 + N;   //traitement IRQ

  //lecture du code de l'instruction
  while(1)
  {
    code = GETC(PC++) & 0xff;
    if(code == 0x10) {precode = 0x1000; continue;}
    if(code == 0x11) {precode = 0x1100; continue;}
    code |= precode; break;
  }

  //execution de l'instruction
  switch(code)
  {
    case 0x00: DIRECT; PUTC(DA, Neg(GETC(DA))); return 6;       /* NEG  /$ */
    case 0x01: DIRECT; return 3;                                /* undoc BRN */
#ifdef THEODORE_UNDOC_OPCODES
    case 0x02: DIRECT;
     if (CC&CC_C) {PUTC(DA, Neg(GETC(DA))); return 6;}          /* undoc COM  /$ */
     else {PUTC(DA, Com(GETC(DA))); return 6;}                  /* undoc NEG  /$ */
#endif
    case 0x03: DIRECT; PUTC(DA, Com(GETC(DA))); return 6;       /* COM  /$ */
    case 0x04: DIRECT; PUTC(DA, Lsr(GETC(DA))); return 6;       /* LSR  /$ */
#ifdef THEODORE_UNDOC_OPCODES
    case 0x05: DIRECT; PUTC(DA, Lsr(GETC(DA))); return 6;       /* undoc LSR  /$ */
#endif
    case 0x06: DIRECT; PUTC(DA, Ror(GETC(DA))); return 6;       /* ROR  /$ */
    case 0x07: DIRECT; PUTC(DA, Asr(GETC(DA))); return 6;       /* ASR  /$ */
    case 0x08: DIRECT; PUTC(DA, Asl(GETC(DA))); return 6;       /* ASL  /$ */
    case 0x09: DIRECT; PUTC(DA, Rol(GETC(DA))); return 6;       /* ROL  /$ */
    case 0x0a: DIRECT; PUTC(DA, Dec(GETC(DA))); return 6;       /* DEC  /$ */
#ifdef THEODORE_UNDOC_OPCODES
    case 0x0b: DIRECT; PUTC(DA, Dec(GETC(DA))); return 6;       /* undoc DEC  /$ */
#endif
    case 0x0c: DIRECT; PUTC(DA, Inc(GETC(DA))); return 6;       /* INC  /$ */
    case 0x0d: DIRECT; Tstc(GETC(DA)); return 6;                /* TST  /$ */
    case 0x0e: DIRECT; PC = DA; return 3;                       /* JMP  /$ */
    case 0x0f: DIRECT; PUTC(DA, Clr()); return 6;               /* CLR  /$ */

    case 0x12: return 2;                                        /* NOP     */
    case 0x13: Sync(); return 4;                                /* SYNC    */
    case 0x16: PC += GETW(PC) + 2; return 5;                    /* LBRA    */
    case 0x17: EXTENDED; Pshs(0x80); PC += W; return 9;         /* LBSR    */
    case 0x19: Daa(); return 2;                                 /* DAA     */
    case 0x1a: CC |= GETC(PC); PC++; return 3;                  /* ORCC #$ */
    case 0x1c: CC &= GETC(PC); PC++; return 3;                  /* ANDC #$ */
    case 0x1d: Tstw(D = B); return 2;                           /* SEX     */
    case 0x1e: PC++; Exg(GETC(PC - 1)); return 8;               /* EXG     */
    case 0x1f: PC++; Tfr(GETC(PC - 1)); return 6;               /* TFR     */

    case 0x20: BRANCH; PC++; return 3;                          /* BRA     */
    case 0x21: PC++; return 3;                                  /* BRN     */
    case 0x22: if(CC_BHI) BRANCH; PC++; return 3;               /* BHI     */
    case 0x23: if(CC_BLS) BRANCH; PC++; return 3;               /* BLS     */
    case 0x24: if(CC_BCC) BRANCH; PC++; return 3;               /* BCC     */
    case 0x25: if(CC_BCS) BRANCH; PC++; return 3;               /* BCS     */
    case 0x26: if(CC_BNE) BRANCH; PC++; return 3;               /* BNE     */
    case 0x27: if(CC_BEQ) BRANCH; PC++; return 3;               /* BEQ     */
    case 0x28: if(CC_BVC) BRANCH; PC++; return 3;               /* BVC     */
    case 0x29: if(CC_BVS) BRANCH; PC++; return 3;               /* BVS     */
    case 0x2a: if(CC_BL)  BRANCH; PC++; return 3;               /* BL      */
    case 0x2b: if(CC_BMI) BRANCH; PC++; return 3;               /* BMI     */
    case 0x2c: if(CC_BGE) BRANCH; PC++; return 3;               /* BGE     */
    case 0x2d: if(CC_BLT) BRANCH; PC++; return 3;               /* BLT     */
    case 0x2e: if(CC_BGT) BRANCH; PC++; return 3;               /* BGT     */
    case 0x2f: if(CC_BLE) BRANCH; PC++; return 3;               /* BLE     */

    case 0x30: INDIRECT; X = W; SET_Z; return 4 + N;            /* LEAX    */
    case 0x31: INDIRECT; Y = W; SET_Z; return 4 + N;            /* LEAY    */
    //d'apres Prehisto, LEAX et LEAY positionnent aussi le bit N de CC
    //il faut donc modifier l'emulation de ces deux instructions !!!
    case 0x32: INDIRECT; S = W; return 4 + N; /*CC not set*/    /* LEAS    */
    case 0x33: INDIRECT; U = W; return 4 + N; /*CC not set*/    /* LEAU    */
    case 0x34: PC++; Pshs(GETC(PC - 1)); return 5 + N;          /* PSHS    */
    case 0x35: PC++; Puls(GETC(PC - 1)); return 5 + N;          /* PULS    */
    case 0x36: PC++; Pshu(GETC(PC - 1)); return 5 + N;          /* PSHU    */
    case 0x37: PC++; Pulu(GETC(PC - 1)); return 5 + N;          /* PULU    */
    case 0x39: Puls(0x80); return 5;                            /* RTS     */
    case 0x3a: X += B & 0xff; return 3;                         /* ABX     */
    case 0x3b: Rti(); return 4 + N;                             /* RTI     */
    case 0x3c: CC &= GETC(PC); PC++; CC |= CC_E; return 20;     /* CWAI    */
    case 0x3d: Mul(); return 11;                                /* MUL     */
    case 0x3f: Swi(1); return 19;                               /* SWI     */

    case 0x40: A = Neg(A); return 2;                            /* NEGA    */
    case 0x43: A = Com(A); return 2;                            /* COMA    */
    case 0x44: A = Lsr(A); return 2;                            /* LSRA    */
    case 0x46: A = Ror(A); return 2;                            /* RORA    */
    case 0x47: A = Asr(A); return 2;                            /* ASRA    */
    case 0x48: A = Asl(A); return 2;                            /* ASLA    */
    case 0x49: A = Rol(A); return 2;                            /* ROLA    */
    case 0x4a: A = Dec(A); return 2;                            /* DECA    */
    case 0x4c: A = Inc(A); return 2;                            /* INCA    */
    case 0x4d: Tstc(A); return 2;                               /* TSTA    */
    case 0x4f: A = Clr(); return 2;                             /* CLRA    */

    case 0x50: B = Neg(B); return 2;                            /* NEGB    */
    case 0x53: B = Com(B); return 2;                            /* COMB    */
    case 0x54: B = Lsr(B); return 2;                            /* LSRB    */
#ifdef THEODORE_UNDOC_OPCODES
    case 0x55: B = Lsr(B); return 2;                            /* undoc LSRB */
#endif
    case 0x56: B = Ror(B); return 2;                            /* RORB    */
    case 0x57: B = Asr(B); return 2;                            /* ASRB    */
    case 0x58: B = Asl(B); return 2;                            /* ASLB    */
    case 0x59: B = Rol(B); return 2;                            /* ROLB    */
    case 0x5a: B = Dec(B); return 2;                            /* DECB    */
    case 0x5c: B = Inc(B); return 2;                            /* INCB    */
    case 0x5d: Tstc(B); return 2;                               /* TSTB    */
    case 0x5f: B = Clr(); return 2;                             /* CLRB    */

    case 0x60: INDIRECT; PUTC(W, Neg(GETC(W))); return 6 + N;   /* NEG  IX */
    case 0x63: INDIRECT; PUTC(W, Com(GETC(W))); return 6 + N;   /* COM  IX */
    case 0x64: INDIRECT; PUTC(W, Lsr(GETC(W))); return 6 + N;   /* LSR  IX */
    case 0x66: INDIRECT; PUTC(W, Ror(GETC(W))); return 6 + N;   /* ROR  IX */
    case 0x67: INDIRECT; PUTC(W, Asr(GETC(W))); return 6 + N;   /* ASR  IX */
    case 0x68: INDIRECT; PUTC(W, Asl(GETC(W))); return 6 + N;   /* ASL  IX */
    case 0x69: INDIRECT; PUTC(W, Rol(GETC(W))); return 6 + N;   /* ROL  IX */
    case 0x6a: INDIRECT; PUTC(W, Dec(GETC(W))); return 6 + N;   /* DEC  IX */
    case 0x6c: INDIRECT; PUTC(W, Inc(GETC(W))); return 6 + N;   /* INC  IX */
    case 0x6d: INDIRECT; Tstc(GETC(W)); return 6 + N;           /* TST  IX */
    case 0x6e: INDIRECT; PC = W; return 3 + N;                  /* JMP  IX */
    case 0x6f: INDIRECT; PUTC(W, Clr()); return 6 + N;          /* CLR  IX */

    case 0x70: EXTENDED; PUTC(W, Neg(GETC(W))); return 7;       /* NEG  $  */
    case 0x73: EXTENDED; PUTC(W, Com(GETC(W))); return 7;       /* COM  $  */
    case 0x74: EXTENDED; PUTC(W, Lsr(GETC(W))); return 7;       /* LSR  $  */
    case 0x76: EXTENDED; PUTC(W, Ror(GETC(W))); return 7;       /* ROR  $  */
    case 0x77: EXTENDED; PUTC(W, Asr(GETC(W))); return 7;       /* ASR  $  */
    case 0x78: EXTENDED; PUTC(W, Asl(GETC(W))); return 7;       /* ASL  $  */
    case 0x79: EXTENDED; PUTC(W, Rol(GETC(W))); return 7;       /* ROL  $  */
    case 0x7a: EXTENDED; PUTC(W, Dec(GETC(W))); return 7;       /* DEC  $  */
    case 0x7c: EXTENDED; PUTC(W, Inc(GETC(W))); return 7;       /* INC  $  */
    case 0x7d: EXTENDED; Tstc(GETC(W)); return 7;               /* TST  $  */
    case 0x7e: EXTENDED; PC = W; return 4;                      /* JMP  $  */
    case 0x7f: EXTENDED; PUTC(W, Clr()); return 7;              /* CLR  $  */

    case 0x80: Subc(AP, GETC(PC)); PC++; return 2;              /* SUBA #$ */
    case 0x81: Cmpc(AP, GETC(PC)); PC++; return 2;              /* CMPA #$ */
    case 0x82: Sbc(AP, GETC(PC)); PC++; return 2;               /* SBCA #$ */
    case 0x83: EXTENDED; Subw(&D, W); return 4;                 /* SUBD #$ */
    case 0x84: Tstc(A &= GETC(PC)); PC++; return 2;             /* ANDA #$ */
    case 0x85: Tstc(A & GETC(PC)); PC++; return 2;              /* BITA #$ */
    case 0x86: Tstc(A = GETC(PC)); PC++; return 2;              /* LDA  #$ */
    case 0x88: Tstc(A ^= GETC(PC)); PC++; return 2;             /* EORA #$ */
    case 0x89: Adc(AP, GETC(PC)); PC++; return 2;               /* ADCA #$ */
    case 0x8a: Tstc(A |= GETC(PC)); PC++; return 2;             /* ORA  #$ */
    case 0x8b: Addc(AP, GETC(PC)); PC++; return 2;              /* ADDA #$ */
    case 0x8c: EXTENDED; Cmpw(&X, W); return 4;                 /* CMPX #$ */
    case 0x8d: DIRECT; Pshs(0x80); PC += DD; return 7;          /* BSR     */
    case 0x8e: EXTENDED; Tstw(X = W); return 3;                 /* LDX  #$ */

    case 0x90: DIRECT; Subc(AP, GETC(DA)); return 4;            /* SUBA /$ */
    case 0x91: DIRECT; Cmpc(AP, GETC(DA)); return 4;            /* CMPA /$ */
    case 0x92: DIRECT; Sbc(AP, GETC(DA)); return 4;             /* SBCA /$ */
    case 0x93: DIRECT; Subw(&D, GETW(DA));return 6;             /* SUBD /$ */
    case 0x94: DIRECT; Tstc(A &= GETC(DA)); return 4;           /* ANDA /$ */
    case 0x95: DIRECT; Tstc(A & GETC(DA)); return 4;            /* BITA /$ */
    case 0x96: DIRECT; Tstc(A = GETC(DA)); return 4;            /* LDA  /$ */
    case 0x97: DIRECT; PUTC(DA, A); Tstc(A); return 4;          /* STA  /$ */
    case 0x98: DIRECT; Tstc(A ^= GETC(DA)); return 4;           /* EORA /$ */
    case 0x99: DIRECT; Adc(AP, GETC(DA)); return 4;             /* ADCA /$ */
    case 0x9a: DIRECT; Tstc(A |= GETC(DA)); return 4;           /* ORA  /$ */
    case 0x9b: DIRECT; Addc(AP, GETC(DA)); return 4;            /* ADDA /$ */
    case 0x9c: DIRECT; Cmpw(&X, GETW(DA)); return 6;            /* CMPX /$ */
    case 0x9d: DIRECT; Pshs(0x80); PC = DA; return 7;           /* JSR  /$ */
    case 0x9e: DIRECT; Tstw(X = GETW(DA)); return 5;            /* LDX  /$ */
    case 0x9f: DIRECT; PUTW(DA, X); Tstw(X); return 5;          /* STX  /$ */

    case 0xa0: INDIRECT; Subc(AP, GETC(W)); return 4 + N;       /* SUBA IX */
    case 0xa1: INDIRECT; Cmpc(AP, GETC(W)); return 4 + N;       /* CMPA IX */
    case 0xa2: INDIRECT; Sbc(AP, GETC(W)); return 4 + N;        /* SBCA IX */
    case 0xa3: INDIRECT; Subw(&D, GETW(W)); return 6 + N;       /* SUBD IX */
    case 0xa4: INDIRECT; Tstc(A &= GETC(W)); return 4 + N;      /* ANDA IX */
    case 0xa5: INDIRECT; Tstc(GETC(W) & A); return 4 + N;       /* BITA IX */
    case 0xa6: INDIRECT; Tstc(A = GETC(W)); return 4 + N;       /* LDA  IX */
    case 0xa7: INDIRECT; PUTC(W, A); Tstc(A); return 4 + N;     /* STA  IX */
    case 0xa8: INDIRECT; Tstc(A ^= GETC(W)); return 4 + N;      /* EORA IX */
    case 0xa9: INDIRECT; Adc(AP, GETC(W)); return 4 + N;        /* ADCA IX */
    case 0xaa: INDIRECT; Tstc(A |= GETC(W)); return 4 + N;      /* ORA  IX */
    case 0xab: INDIRECT; Addc(AP, GETC(W)); return 4 + N;       /* ADDA IX */
    case 0xac: INDIRECT; Cmpw(&X, GETW(W)); return 4 + N;       /* CMPX IX */
    case 0xad: INDIRECT; Pshs(0x80); PC = W; return 5 + N;      /* JSR  IX */
    case 0xae: INDIRECT; Tstw(X = GETW(W)); return 5 + N;       /* LDX  IX */
    case 0xaf: INDIRECT; PUTW(W, X); Tstw(X); return 5 + N;     /* STX  IX */

    case 0xb0: EXTENDED; Subc(AP, GETC(W)); return 5;           /* SUBA $  */
    case 0xb1: EXTENDED; Cmpc(AP, GETC(W)); return 5;           /* CMPA $  */
    case 0xb2: EXTENDED; Sbc(AP, GETC(W)); return 5;            /* SBCA $  */
    case 0xb3: EXTENDED; Subw(&D, GETW(W)); return 7;           /* SUBD $  */
    case 0xb4: EXTENDED; Tstc(A &= GETC(W)); return 5;          /* ANDA $  */
    case 0xb5: EXTENDED; Tstc(A & GETC(W)); return 5;           /* BITA $  */
    case 0xb6: EXTENDED; Tstc(A = GETC(W)); return 5;           /* LDA  $  */
    case 0xb7: EXTENDED; PUTC(W, A); Tstc(A); return 5;         /* STA  $  */
    case 0xb8: EXTENDED; Tstc(A ^= GETC(W)); return 5;          /* EORA $  */
    case 0xb9: EXTENDED; Adc(AP, GETC(W)); return 5;            /* ADCA $  */
    case 0xba: EXTENDED; Tstc(A |= GETC(W)); return 5;          /* ORA  $  */
    case 0xbb: EXTENDED; Addc(AP, GETC(W)); return 5;           /* ADDA $  */
    case 0xbc: EXTENDED; Cmpw(&X, GETW(W)); return 7;           /* CMPX $  */
    case 0xbd: EXTENDED; Pshs(0x80); PC = W; return 8;          /* JSR  $  */
    case 0xbe: EXTENDED; Tstw(X = GETW(W)); return 6;           /* LDX  $  */
    case 0xbf: EXTENDED; PUTW(W, X); Tstw(X); return 6;         /* STX  $  */

    case 0xc0: Subc(BP, GETC(PC)); PC++; return 2;              /* SUBB #$ */
    case 0xc1: Cmpc(BP, GETC(PC)); PC++; return 2;              /* CMPB #$ */
    case 0xc2: Sbc(BP, GETC(PC)); PC++; return 2;               /* SBCB #$ */
    case 0xc3: EXTENDED; Addw(&D, W); return 4;                 /* ADDD #$ */
    case 0xc4: Tstc(B &= GETC(PC)); PC++; return 2;             /* ANDB #$ */
    case 0xc5: Tstc(B & GETC(PC)); PC++; return 2;              /* BITB #$ */
    case 0xc6: Tstc(B = GETC(PC)); PC++; return 2;              /* LDB  #$ */
    case 0xc8: Tstc(B ^= GETC(PC)); PC++; return 2;             /* EORB #$ */
    case 0xc9: Adc(BP, GETC(PC)); PC++; return 2;               /* ADCB #$ */
    case 0xca: Tstc(B |= GETC(PC)); PC++; return 2;             /* ORB  #$ */
    case 0xcb: Addc(BP, GETC(PC)); PC++;return 2;               /* ADDB #$ */
    case 0xcc: EXTENDED; Tstw(D = W); return 3;                 /* LDD  #$ */
    case 0xce: EXTENDED; Tstw(U = W); return 3;                 /* LDU  #$ */

    case 0xd0: DIRECT; Subc(BP, GETC(DA)); return 4;            /* SUBB /$ */
    case 0xd1: DIRECT; Cmpc(BP, GETC(DA)); return 4;            /* CMPB /$ */
    case 0xd2: DIRECT; Sbc(BP, GETC(DA)); return 4;             /* SBCB /$ */
    case 0xd3: DIRECT; Addw(&D, GETW(DA)); return 6;            /* ADDD /$ */
    case 0xd4: DIRECT; Tstc(B &= GETC(DA)); return 4;           /* ANDB /$ */
    case 0xd5: DIRECT; Tstc(GETC(DA) & B); return 4;            /* BITB /$ */
    case 0xd6: DIRECT; Tstc(B = GETC(DA)); return 4;            /* LDB  /$ */
    case 0xd7: DIRECT; PUTC(DA,B); Tstc(B); return 4;           /* STB  /$ */
    case 0xd8: DIRECT; Tstc(B ^= GETC(DA)); return 4;           /* EORB /$ */
    case 0xd9: DIRECT; Adc(BP, GETC(DA)); return 4;             /* ADCB /$ */
    case 0xda: DIRECT; Tstc(B |= GETC(DA)); return 4;           /* ORB  /$ */
    case 0xdb: DIRECT; Addc(BP, GETC(DA)); return 4;            /* ADDB /$ */
    case 0xdc: DIRECT; Tstw(D = GETW(DA)); return 5;            /* LDD  /$ */
    case 0xdd: DIRECT; PUTW(DA, D); Tstw(D); return 5;          /* STD  /$ */
    case 0xde: DIRECT; Tstw(U = GETW(DA)); return 5;            /* LDU  /$ */
    case 0xdf: DIRECT; PUTW(DA, U); Tstw(U); return 5;          /* STU  /$ */

    case 0xe0: INDIRECT; Subc(BP, GETC(W)); return 4 + N;       /* SUBB IX */
    case 0xe1: INDIRECT; Cmpc(BP, GETC(W)); return 4 + N;       /* CMPB IX */
    case 0xe2: INDIRECT; Sbc(BP, GETC(W)); return 4 + N;        /* SBCB IX */
    case 0xe3: INDIRECT; Addw(&D, GETW(W)); return 6 + N;       /* ADDD IX */
    case 0xe4: INDIRECT; Tstc(B &= GETC(W)); return 4 + N;      /* ANDB IX */
    case 0xe5: INDIRECT; Tstc(B & GETC(W)); return 4 + N;       /* BITB IX */
    case 0xe6: INDIRECT; Tstc(B = GETC(W)); return 4 + N;       /* LDB  IX */
    case 0xe7: INDIRECT; PUTC(W, B); Tstc(B); return 4 + N;     /* STB  IX */
    case 0xe8: INDIRECT; Tstc(B ^= GETC(W)); return 4 + N;      /* EORB IX */
    case 0xe9: INDIRECT; Adc(BP, GETC(W)); return 4 + N;        /* ADCB IX */
    case 0xea: INDIRECT; Tstc(B |= GETC(W)); return 4 + N;      /* ORB  IX */
    case 0xeb: INDIRECT; Addc(BP, GETC(W)); return 4 + N;       /* ADDB IX */
    case 0xec: INDIRECT; Tstw(D = GETW(W)); return 5 + N;       /* LDD  IX */
    case 0xed: INDIRECT; PUTW(W, D); Tstw(D); return 5 + N;     /* STD  IX */
    case 0xee: INDIRECT; Tstw(U = GETW(W)); return 5 + N;       /* LDU  IX */
    case 0xef: INDIRECT; PUTW(W, U); Tstw(U); return 5 + N;     /* STU  IX */

    case 0xf0: EXTENDED; Subc(BP, GETC(W)); return 5;           /* SUBB $  */
    case 0xf1: EXTENDED; Cmpc(BP, GETC(W)); return 5;           /* CMPB $  */
    case 0xf2: EXTENDED; Sbc(BP, GETC(W)); return 5;            /* SBCB $  */
    case 0xf3: EXTENDED; Addw(&D, GETW(W)); return 7;           /* ADDD $  */
    case 0xf4: EXTENDED; Tstc(B &= GETC(W)); return 5;          /* ANDB $  */
    case 0xf5: EXTENDED; Tstc(B & GETC(W)); return 5;           /* BITB $  */
    case 0xf6: EXTENDED; Tstc(B = GETC(W)); return 5;           /* LDB  $  */
    case 0xf7: EXTENDED; PUTC(W, B); Tstc(B); return 5;         /* STB  $  */
    case 0xf8: EXTENDED; Tstc(B ^= GETC(W)); return 5;          /* EORB $  */
    case 0xf9: EXTENDED; Adc(BP, GETC(W)); return 5;            /* ADCB $  */
    case 0xfa: EXTENDED; Tstc(B |= GETC(W)); return 5;          /* ORB  $  */
    case 0xfb: EXTENDED; Addc(BP, GETC(W)); return 5;           /* ADDB $  */
    case 0xfc: EXTENDED; Tstw(D = GETW(W)); return 6;           /* LDD  $  */
    case 0xfd: EXTENDED; PUTW(W, D); Tstw(D); return 6;         /* STD  $  */
    case 0xfe: EXTENDED; Tstw(U = GETW(W)); return 6;           /* LDU  $  */
    case 0xff: EXTENDED; PUTW(W, U); Tstw(U); return 6;         /* STU  $  */

    case 0x1021: PC += 2; return 5;                             /* LBRN    */
    case 0x1022: if(CC_BHI) LBRANCH; PC += 2; return 5 + N;     /* LBHI    */
    case 0x1023: if(CC_BLS) LBRANCH; PC += 2; return 5 + N;     /* LBLS    */
    case 0x1024: if(CC_BCC) LBRANCH; PC += 2; return 5 + N;     /* LBCC    */
    case 0x1025: if(CC_BCS) LBRANCH; PC += 2; return 5 + N;     /* LBCS    */
    case 0x1026: if(CC_BNE) LBRANCH; PC += 2; return 5 + N;     /* LBNE    */
    case 0x1027: if(CC_BEQ) LBRANCH; PC += 2; return 5 + N;     /* LBEQ    */
    case 0x1028: if(CC_BVC) LBRANCH; PC += 2; return 5 + N;     /* LBVC    */
    case 0x1029: if(CC_BVS) LBRANCH; PC += 2; return 5 + N;     /* LBVS    */
    case 0x102a: if(CC_BL)  LBRANCH; PC += 2; return 5 + N;     /* LBL    */
    case 0x102b: if(CC_BMI) LBRANCH; PC += 2; return 5 + N;     /* LBMI    */
    case 0x102c: if(CC_BGE) LBRANCH; PC += 2; return 5 + N;     /* LBGE    */
    case 0x102d: if(CC_BLT) LBRANCH; PC += 2; return 5 + N;     /* LBLT    */
    case 0x102e: if(CC_BGT) LBRANCH; PC += 2; return 5 + N;     /* LBGT    */
    case 0x102f: if(CC_BLE) LBRANCH; PC += 2; return 5 + N;     /* LBLE    */
    case 0x103f: Swi(2); return 20;                             /* SWI2    */

    case 0x1083: EXTENDED; Cmpw(&D, W); return 5;               /* CMPD #$ */
    case 0x108c: EXTENDED; Cmpw(&Y, W); return 5;               /* CMPY #$ */
    case 0x108e: EXTENDED; Tstw(Y = W); return 4;               /* LDY  #$ */
    case 0x1093: DIRECT; Cmpw(&D, GETW(DA)); return 7;          /* CMPD /$ */
    case 0x109c: DIRECT; Cmpw(&Y, GETW(DA)); return 7;          /* CMPY /$ */
    case 0x109e: DIRECT; Tstw(Y = GETW(DA)); return 6;          /* LDY  /$ */
    case 0x109f: DIRECT; PUTW(DA, Y); Tstw(Y); return 6;        /* STY  /$ */
    case 0x10a3: INDIRECT; Cmpw(&D, GETW(W)); return 7 + N;     /* CMPD IX */
    case 0x10ac: INDIRECT; Cmpw(&Y, GETW(W)); return 7 + N;     /* CMPY IX */
    case 0x10ae: INDIRECT; Tstw(Y = GETW(W)); return 6 + N;     /* LDY  IX */
    case 0x10af: INDIRECT; PUTW(W, Y); Tstw(Y); return 6 + N;   /* STY  IX */
    case 0x10b3: EXTENDED; Cmpw(&D, GETW(W)); return 8;         /* CMPD $  */
    case 0x10bc: EXTENDED; Cmpw(&Y, GETW(W)); return 8;         /* CMPY $  */
    case 0x10be: EXTENDED; Tstw(Y = GETW(W)); return 7;         /* LDY  $  */
    case 0x10bf: EXTENDED; PUTW(W, Y); Tstw(Y); return 7;       /* STY  $  */
    case 0x10ce: EXTENDED; Tstw(S = W); return 4;               /* LDS  #$ */
    case 0x10de: DIRECT; Tstw(S = GETW(DA)); return 6;          /* LDS  /$ */
    case 0x10df: DIRECT; PUTW(DA, S); Tstw(S); return 6;        /* STS  /$ */
    case 0x10ee: INDIRECT; Tstw(S = GETW(W)); return 6 + N;     /* LDS  IX */
    case 0x10ef: INDIRECT; PUTW(W, S); Tstw(S); return 6 + N;   /* STS  IX */
    case 0x10fe: EXTENDED; Tstw(S = GETW(W)); return 7;         /* LDS  $  */
    case 0x10ff: EXTENDED; PUTW(W, S); Tstw(S); return 7;       /* STS  $  */

    case 0x113f: Swi(3); return 20;                             /* SWI3    */
    case 0x1183: EXTENDED; Cmpw(&U, W); return 5;               /* CMPU #$ */
    case 0x118c: EXTENDED; Cmpw(&S, W); return 5;               /* CMPS #$ */
    case 0x1193: DIRECT; Cmpw(&U, GETW(DA)); return 7;          /* CMPU /$ */
    case 0x119c: DIRECT; Cmpw(&S, GETW(DA)); return 7;          /* CMPS /$ */
    case 0x11a3: INDIRECT; Cmpw(&U, GETW(W)); return 7 + N;     /* CMPU IX */
    case 0x11ac: INDIRECT; Cmpw(&S, GETW(W)); return 7 + N;     /* CMPS IX */
    case 0x11b3: EXTENDED; Cmpw(&U, GETW(W)); return 8;         /* CMPU $  */
    case 0x11bc: EXTENDED; Cmpw(&S, GETW(W)); return 8;         /* CMPS $  */

    default: return -code;                                      /* Illegal */
  }
}

unsigned int cpu_serialize_size(void)
{
  return sizeof(dc6809_cycles) + sizeof(dc6809_sync) + sizeof(dc6809_irq)
      + sizeof(dc6809_firq) + sizeof(dc6809_nmi) + sizeof(dc6809_w)
      + sizeof(dc6809_cc) + sizeof(dc6809_pc) + sizeof(dc6809_d)
      + sizeof(dc6809_x) + sizeof(dc6809_y) + sizeof(dc6809_u)
      + sizeof(dc6809_s) + sizeof(dc6809_da);
}

#include <string.h>

void cpu_serialize(void *data)
{
  int offset = 0;
  char *buffer = (char *) data;
  memcpy(buffer+offset, &dc6809_cycles, sizeof(dc6809_cycles));
  offset += sizeof(dc6809_cycles);
  memcpy(buffer+offset, &dc6809_sync, sizeof(dc6809_sync));
  offset += sizeof(dc6809_sync);
  memcpy(buffer+offset, &dc6809_irq, sizeof(dc6809_irq));
  offset += sizeof(dc6809_irq);
  memcpy(buffer+offset, &dc6809_firq, sizeof(dc6809_firq));
  offset += sizeof(dc6809_firq);
  memcpy(buffer+offset, &dc6809_nmi, sizeof(dc6809_nmi));
  offset += sizeof(dc6809_nmi);
  memcpy(buffer+offset, &dc6809_w, sizeof(dc6809_w));
  offset += sizeof(dc6809_w);
  memcpy(buffer+offset, &dc6809_cc, sizeof(dc6809_cc));
  offset += sizeof(dc6809_cc);
  memcpy(buffer+offset, &dc6809_pc, sizeof(dc6809_pc));
  offset += sizeof(dc6809_pc);
  memcpy(buffer+offset, &dc6809_d, sizeof(dc6809_d));
  offset += sizeof(dc6809_d);
  memcpy(buffer+offset, &dc6809_x, sizeof(dc6809_x));
  offset += sizeof(dc6809_x);
  memcpy(buffer+offset, &dc6809_y, sizeof(dc6809_y));
  offset += sizeof(dc6809_y);
  memcpy(buffer+offset, &dc6809_u, sizeof(dc6809_u));
  offset += sizeof(dc6809_u);
  memcpy(buffer+offset, &dc6809_s, sizeof(dc6809_s));
  offset += sizeof(dc6809_s);
  memcpy(buffer+offset, &dc6809_da, sizeof(dc6809_da));
}

void cpu_unserialize(const void *data)
{
  int offset = 0;
  const char *buffer = (const char *) data;
  memcpy(&dc6809_cycles, buffer+offset, sizeof(dc6809_cycles));
  offset += sizeof(dc6809_cycles);
  memcpy(&dc6809_sync, buffer+offset, sizeof(dc6809_sync));
  offset += sizeof(dc6809_sync);
  memcpy(&dc6809_irq, buffer+offset, sizeof(dc6809_irq));
  offset += sizeof(dc6809_irq);
  memcpy(&dc6809_firq, buffer+offset, sizeof(dc6809_firq));
  offset += sizeof(dc6809_firq);
  memcpy(&dc6809_nmi, buffer+offset, sizeof(dc6809_nmi));
  offset += sizeof(dc6809_nmi);
  memcpy(&dc6809_w, buffer+offset, sizeof(dc6809_w));
  offset += sizeof(dc6809_w);
  memcpy(&dc6809_cc, buffer+offset, sizeof(dc6809_cc));
  offset += sizeof(dc6809_cc);
  memcpy(&dc6809_pc, buffer+offset, sizeof(dc6809_pc));
  offset += sizeof(dc6809_pc);
  memcpy(&dc6809_d, buffer+offset, sizeof(dc6809_d));
  offset += sizeof(dc6809_d);
  memcpy(&dc6809_x, buffer+offset, sizeof(dc6809_x));
  offset += sizeof(dc6809_x);
  memcpy(&dc6809_y, buffer+offset, sizeof(dc6809_y));
  offset += sizeof(dc6809_y);
  memcpy(&dc6809_u, buffer+offset, sizeof(dc6809_u));
  offset += sizeof(dc6809_u);
  memcpy(&dc6809_s, buffer+offset, sizeof(dc6809_s));
  offset += sizeof(dc6809_s);
  memcpy(&dc6809_da, buffer+offset, sizeof(dc6809_da));
}
