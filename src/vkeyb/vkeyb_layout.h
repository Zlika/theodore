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

/* Virtual keyboard layout */

#ifndef __VKEYB_LAYOUT_H
#define __VKEYB_LAYOUT_H

// A key on the virtual keyboard
struct VKey
{
  int scancode;       // scancode of the key
  int x;              // x coordinate of the upper left corner of the key
  int y;              // y coordinate of the upper left corner of the key
  int width;          // width of the key
  int height;         // height of the key
  const struct VKey *left;  // next key at the left side
  const struct VKey *right; // next key at the right side
  const struct VKey *up;    // next key at the up side
  const struct VKey *down;  // next key at the down side
};

extern const struct VKey mo5_kb[];

#endif /* __VKEYB_LAYOUT_H */
