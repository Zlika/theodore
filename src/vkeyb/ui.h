/*
 * This file is part of theodore, a Thomson emulator
 * (https://github.com/Zlika/theodore).
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

/* Utility functions to write UI elements on the screen */

#ifndef __UI_H
#define __UI_H

#include <stdint.h>

// Draw an image at the given position
extern void draw_bmp(int x, int y, const uint16_t *img, int img_width, int img_height);
// Draw a colored box at the given position
extern void draw_box(int x, int y, int width, int height, int thickness, uint16_t color);

#endif /* __UI_H */
