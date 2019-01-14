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

#ifndef __6809CPU_H
#define __6809CPU_H

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
//U register
extern short dc6809_u;
//S register
extern short dc6809_s;
//Program Counter
extern unsigned short dc6809_pc;

//pointer to A register
extern char *dc6809_a;
//pointer to B register
extern char *dc6809_b;
//pointer to DP register
extern char *dc6809_dp;

//irq trigger  (0=disabled, 1=enabled)
extern int dc6809_irq;
//interrupt request
extern int Irq(void);

// Processor reset
void Reset6809(void);
// Execute one operation at pc address and set pc to next opcode address
// Return value is set to :
// - cycle count for the executed instruction when operation code is legal
// - negative value (-code) when operation code is illegal
int Run6809(void);

// The following functions are used for libretro's save states feature.
// Returns the amount of data required to serialize the CPU's internal state.
unsigned int cpu_serialize_size(void);
// Serializes the CPU's internal state.
void cpu_serialize(void *data);
// Unserializes the CPU's internal state.
void cpu_unserialize(const void *data);

#endif /* __6809CPU_H */
