//////////////////////////////////////////////////////////////////////////
// DC6809EMUL.H - Motorola 6809 microprocessor emulation
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcto8.free.fr
//
// This file is part of DCTO8D.
//
// DCTO8D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DCTO8D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DCTO8D. If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////

#ifndef __DC6809EMUL_H
#define __DC6809EMUL_H

//pointeurs vers fonctions d'acces memoire
extern char (*Mgetc)(unsigned short a);
extern void (*Mputc)(unsigned short a, char c);

// function to read 2 bytes from address
extern short Mgetw(unsigned short a);
// function to write 2 bytes at an address
extern void Mputw(unsigned short a, short w);

//6809 registers
//condition code
extern char  dc6809_cc;
//X register
extern short dc6809_x;
//Y register
extern short dc6809_y;

//pointer to A register
extern char *dc6809_a;
//pointer to B register
extern char *dc6809_b;

//irq trigger  (0=inactif)
extern int dc6809_irq;

// Processor initialisation
void Init6809();
// Processor reset
void Reset6809();
// Execute one operation at pc address and set pc to next opcode address
/*
Return value is set to :
- cycle count for the executed instruction when operation code is legal
- negative value (-code) when operation code is illegal
*/
int Run6809();

#endif /* __DC6809EMUL_H */
