//////////////////////////////////////////////////////////////////////////
// DC6809EMUL.H - Motorola 6809 micropocessor emulation
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
int dc6809_irq;

// Processor initialisation //////////////////////////////////////////////////
void Init6809();

void (*Mputc)(unsigned short a, char c);

#endif /* __DC6809EMUL_H */
