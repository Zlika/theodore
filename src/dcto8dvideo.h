///////////////////////////////////////////////////////////////////////////////
// DCTO8DVIDEO.H - Fonctions d'affichage pour dcto8d
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

#ifndef __DCTO8DVIDEO_H
#define __DCTO8DVIDEO_H

#include <SDL.h>
#include <stdbool.h>
#include "dcto8dglobal.h"

#define YSTATUS 20

//surface d'affichage de l'ecran
extern SDL_Surface *screen;

//abscisse souris dans fenetre utilisateur
extern int xmouse;
//ordonnee souris dans fenetre utilisateur
extern int ymouse;

//pointeur fonction decodage memoire video
extern void (*Decodevideo)();

// Resize screen
void Resizescreen(int x, int y);
//Display screen
void Displayscreen();
// Creation d'un segment de ligne d'ecran
void Displaysegment();
// Changement de ligne ecran
void Nextline();
// Modification de la palette
void Palette(int n, int r, int v, int b);
// Decodage octet video mode 320x16 standard
void Decode320x16();
// Decodage octet video mode bitmap4 320x200 4 couleurs
void Decode320x4();
// Decodage octet video mode bitmap4 special 320x200 4 couleurs
void Decode320x4special();
// Decodage octet video mode bitmap16 160x200 16 couleurs
void Decode160x16();
// Decodage octet video mode 640x200 2 couleurs
void Decode640x2();
// Switch between fullscreen and windowed modes
void SwitchFullScreenMode();
// Returns true if fullscreen mode is enabled, false otherwise
bool IsFullScreenMode();

#endif /* __DCTO8DVIDEO_H */
