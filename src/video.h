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

/* Graphical display functions of the emulator */

#ifndef __VIDEO_H
#define __VIDEO_H

#include <stdint.h>

#define XBITMAP 672
#define YBITMAP 432

// Sets the framebuffer to use
int SetLibRetroVideoBuffer(uint32_t *video_buffer);

// List of available video modes
enum VideoMode { VIDEO_320X16, VIDEO_320X4, VIDEO_320X4_SPECIAL,
                 VIDEO_160X16, VIDEO_640X2 };

// Sets the video mode
void SetVideoMode(enum VideoMode mode);

// Creation d'un segment de ligne d'ecran
void Displaysegment(void);
// Changement de ligne ecran
void Nextline(void);
// Modification de la palette
void Palette(int n, int r, int v, int b);

// The following functions are used for libretro's save states feature.
// Returns the amount of data required to serialize the internal state of the video module.
int video_serialize_size(void);
// Serializes the internal state of the video module.
void video_serialize(void *data);
// Unserializes the internal state of the video module.
void video_unserialize(const void *data);

#endif /* __VIDEO_H */
