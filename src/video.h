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

/* Graphical display functions of the emulator */

#ifndef __VIDEO_H
#define __VIDEO_H

#define XBITMAP 672
#define YBITMAP 432

void SetLibRetroVideoBuffer(void *video_buffer);

//pointeur fonction decodage memoire video
extern void (*Decodevideo)(void);

// Creation d'un segment de ligne d'ecran
void Displaysegment(void);
// Changement de ligne ecran
void Nextline(void);
// Modification de la palette
void Palette(int n, int r, int v, int b);
// Decodage octet video mode 320x16 standard
void Decode320x16(void);
// Decodage octet video mode bitmap4 320x200 4 couleurs
void Decode320x4(void);
// Decodage octet video mode bitmap4 special 320x200 4 couleurs
void Decode320x4special(void);
// Decodage octet video mode bitmap16 160x200 16 couleurs
void Decode160x16(void);
// Decodage octet video mode 640x200 2 couleurs
void Decode640x2(void);

#endif /* __VIDEO_H */
