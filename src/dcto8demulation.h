///////////////////////////////////////////////////////////////////////////////
// DCTO8DEMULATION.C - Emulateur Thomson TO8D portable
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
///////////////////////////////////////////////////////////////////////////////

#ifndef __DCTO8DEMULATION_H
#define __DCTO8DEMULATION_H

// pointer to function to read 1 byte from address
extern char (*Mgetc)(unsigned short);
// function to read 2 bytes from address
extern short Mgetw(unsigned short a);

// memory
//espace cartouche 4x16K
extern char car[0x10000];
//ram 512K
extern char ram[0x80000];
//ports d'entree/sortie
extern char port[0x40];

//flags cartouche
//type de cartouche (0=simple 1=switch bank, 2=os-9)
extern int cartype;
//bits0,1,4=bank, 2=cart-enabled, 3=write-enabled
extern int carflags;

//etat touches to8d
extern int touche[];
//lightpen coordinates
extern int xpen, ypen;
//lightpen button state
extern int penbutton;

//niveau du haut-parleur
extern int sound;
// mute flag
extern int mute;

//affichage
//compteur ligne (0-63)
extern int videolinecycle;
//numero de ligne video affichee (0-311)
extern int videolinenumber;
//nombre de vbl entre deux affichages
extern int vblnumbermax;
//couleur de la bordure de l'écran
extern int bordercolor;
//pointeur page video affichee
extern char *pagevideo;

// Joystick emulation ////////////////////////////////////////////////////////
void Joysemul(int i, int state);
// Emulation du clavier TO8 ///////////////////////////////////////////////////
void TO8key(int n);
// Initialisation programme de l'ordinateur emule ////////////////////////////
void Initprog();
// Execution n cycles processeur 6809 ////////////////////////////////////////
int Run(int ncyclesmax);
// Hardreset de l'ordinateur emule ///////////////////////////////////////////
void Hardreset();

#endif /* __DCTO8DEMULATION_H */
