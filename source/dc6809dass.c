//////////////////////////////////////////////////////////////////////////
// DC6809DASS.C - Motorola 6809 disassembler
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcto8.free.fr
//
// This file is part of DCTO8D.
//
// DCTO8D is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DCTO8D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DCTO8D. If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include "dc6809dass.h"

#define GETBYTE i=Mgetc(pc)&0x00ff;pc+=1;sprintf(w,"%02X",i);strcat(hexa,w)
#define GETWORD i=Mgetw(pc)&0xffff;pc+=2;sprintf(w,"%04X",i);strcat(hexa,w)

int n;          //nombre de cycles
int pc;         //adresse desassemblage
char w[10];     //zone de travail
char hexa[20];  //adresse et dump hexa
char param[20]; //parametres

//external pointer to function to read 1 byte from address
extern char (*Mgetc)(unsigned short address);
//external function to read 2 bytes from address
extern short Mgetw(unsigned short address);

//Creation d'une liste de registres //////////////////////////////////////////
void Reglist(int type)
{
// string: pointeur vers la chaine de retour de la liste de registres
// type: 0=EXG-TFR 1=PSHS 2=PULS 3=PSHU 4=PULU
 int i, c;
 const char s[8][4] = {"PC,","U,","Y,","X,","DP,","B,","A,","CC,"};
 const char u[8][4] = {"PC,","S,","Y,","X,","DP,","B,","A,","CC,"};
 const char r[16][4] = {"D,","X,","Y,","U,","S,","PC,","?,","?,",
                        "A,","B,","CC,","DP,","?,","?,","?,","?,"};
 param[0] = 0;
 c = Mgetc(pc++) & 0xff;
 sprintf(w, "%02X", c); strcat(hexa, w);
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
void Indexed()
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
  default : reg = ' ';
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

// Desassembly ///////////////////////////////////////////////////////////////
int Dasm6809(char *string, int a)
{// string = pointeur vers une chaine recevant la ligne desassemblee
 //      a = adresse des caracteres a desassembler
 // retour = adresse de l'instruction suivante
 int i, precode, code;
 //initialisations
 pc = a; //adresse de desassemblage
 n = 0;  //initialisation du nombre de cycles
 sprintf(hexa, "%04X ", a);
 //lecture du code instruction
 GETBYTE; code = i; precode = 0;
 if(code == 0x10) {precode = 0x100; i = Mgetc(pc) & 0xff;}
 if(code == 0x11) {precode = 0x200; i = Mgetc(pc) & 0xff;}
 if(precode > 0) if((i != 0x10) && (i != 0x11)) {GETBYTE; code = precode | i;}
 //decodage de l'instruction
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
  default:   param[0] = 0; break;
 }
 //composition de la ligne desassemblee
 strcpy(string, hexa);
 for(i = strlen(string); i < 16; i++) strcat(string, " ");
 strcat(string, mnemonique[instruction[code] >> 16]);
 for(i = strlen(string); i < 23; i++) strcat(string, " ");
 strcat(string, param);
 n += instruction[code] & 0xff;
 if(n == 0) strcpy(w, "?");
 if(n > 0) sprintf(w, "%i", n);
 if(n > 50) sprintf(w, "%i/%i", n / 10, n - 10 * (n / 10));
 for(i = strlen(string); i < 45 - strlen(w); i++) strcat(string, " ");
 strcat(string, w);
 //retour adresse prochaine instruction
 return(pc);
}

