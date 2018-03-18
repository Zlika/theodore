//////////////////////////////////////////////////////////////////////////
// DC6809DASS.H - Header for Motorola 6809 micropocessor desassembly
// Author   : Daniel Coulom - danielcoulom@gmail.com
// Web site : http://dcto9p.free.fr
// Created  : 2007-12
// Version  : 2007-12-20
//
// This file is part of DCTO9P v11.
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
// along with DCTO8D.  If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////

#ifndef __DCTO8DASS_H
#define __DCTO8DASS_H

// Desassembly
// string = pointeur vers une chaine recevant la ligne desassemblee
//      a = adresse des caracteres a desassembler
// retour = adresse de l'instruction suivante
int Dasm6809(char *string, int a);

#endif
